# RLVR：从 PPO 到 GRPO 的可验证奖励强化学习

---

## 1. 为什么需要 RLVR

RLHF 的瓶颈：

- 人类偏好标注昂贵且噪声大；
- 奖励模型可被投机优化；
- 难以支撑超大规模稳定迭代。

RLVR 的切入点：

- 在“答案可自动验证”的任务上（数学、代码、证明），
- 用客观奖励替代主观偏好。

---

## 2. RLVR 奖励定义

最简形式：

$$
R(z)=
\begin{cases}
1, & z \text{ 正确} \\ 
0, & z \text{ 错误}
\end{cases}
$$

也可扩展到过程奖励：

- 分步正确性；
- 测试用例通过比例；
- 结构合法性约束。

核心收益：

- 高信噪比；
- 可规模化；
- 可重复验证。

---

## 3. PPO 核心机制（回顾）

概率比：

$$
r_t(\theta)=\frac{\pi_\theta(a_t|s_t)}{\pi_{\theta_{old}}(a_t|s_t)}
$$

裁剪目标：

$$
L^{\text{clip}}(\theta)=\mathbb{E}_t\left[\min\left(r_tA_t,\ \text{clip}(r_t,1-\epsilon,1+\epsilon)A_t\right)\right]
$$

直觉：

- 优势为正的动作提高概率；
- 优势为负的动作降低概率；
- 裁剪防止单次更新过猛导致策略崩溃。

---

## 4. 为什么从 PPO 走向 GRPO

PPO 在大模型推理训练中的问题：

- 实现链路复杂（value head、GAE、KL、归一化）；
- 训练稳定性高度依赖细节；
- 计算和调参成本高。

GRPO 的动机：

- 利用组内比较构造相对优势；
- 降低对独立价值网络的依赖；
- 更适配可验证任务的批量采样范式。

---

## 5. RLVR 训练管线（实战视角）

1. 任务采样：数学题/代码题；
2. 多候选生成：同题生成多条轨迹；
3. 自动验证：判题器给 reward；
4. 构造优势：组内相对好坏；
5. 策略更新：PPO/GRPO 目标优化；
6. 周期评估：正确率、长度、稳定性。

---

## 6. 冷启动数据与 CoT 的作用

冷启动数据价值：

- 给策略一个可行初始分布；
- 降低 RL 初期“全错无梯度”问题。

CoT（思维链）价值：

- 为复杂推理提供中间结构；
- 在可验证任务中提升成功率。

风险：

- 冗长 CoT 可能被奖励函数误导；
- 需配合长度控制与答案校验。

---

## 7. 常见误用 -> 正确写法

- 误用：把 RLVR 用到不可验证开放任务。  
  正确：优先用于可判对错任务，再与 RLHF 互补。
- 误用：只看最终正确率。  
  正确：同时监控长度膨胀、格式崩坏、拒答率。
- 误用：奖励函数过窄，催生投机策略。  
  正确：多维验证（正确性 + 格式 + 约束）联合打分。
- 误用：忽略验证器质量。  
  正确：把判题器当“关键模型”做回归测试与红队。

---

## 8. PPO/GRPO 工程检查清单

- 采样温度与 top-p 是否固定；
- 奖励归一化策略是否一致；
- KL 惩罚系数是否自适应；
- 长度惩罚是否防止无意义长输出；
- 失败样本是否回流到 SFT/对齐数据池。


## 9. 代码化模板：RLVR 训练前检查脚本（伪代码）

```python
def preflight_check(cfg, verifier):
    errors = []
    if cfg["task_type"] not in {"math", "code", "formal_proof"}:
        errors.append("task not verifiable")
    if cfg["samples_per_prompt"] < 2:
        errors.append("need group samples for relative advantage")
    if not verifier.is_deterministic:
        errors.append("verifier must be deterministic")
    if cfg["max_response_tokens"] > 4096 and not cfg.get("length_penalty"):
        errors.append("missing length control")
    return errors
```

**常见误用 -> 正确写法**
- 误用：验证器有随机性还直接用于训练。  
  正确：先做 determinism 测试，再接入训练环路。

## 10. 代码化模板：rollout -> verify -> update 伪代码

```python
def rlvr_train_step(policy, prompts, verifier, optimizer):
    # 1) rollout: 每个 prompt 采样多条候选
    candidates = [policy.sample_group(p, k=4) for p in prompts]

    # 2) verify: 自动判分
    rewards = [[verifier.score(p, c) for c in group] for p, group in zip(prompts, candidates)]

    # 3) 构造组内相对优势（GRPO 思路）
    advantages = []
    for group_rewards in rewards:
        mean_r = sum(group_rewards) / len(group_rewards)
        advantages.append([r - mean_r for r in group_rewards])

    # 4) 计算策略损失并更新
    loss = compute_policy_loss(policy, prompts, candidates, advantages)
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()
    return {"loss": float(loss.detach())}
```

**工程注意**
- 每轮训练记录“长度分布 + 正确率 + 拒答率”，防止奖励黑客；
- 失败样本要分桶（解析失败、逻辑失败、格式失败）以便回流修复。

## 11. 代码化模板：RLVR 运行配置（可直接改）

```yaml
# rlvr_config.yaml
task_type: "math"
samples_per_prompt: 4
max_response_tokens: 2048
length_penalty: 0.01
optimizer:
  lr: 1e-6
  grad_clip_norm: 1.0
verifier:
  name: "gsm8k_exact_match"
  deterministic: true
monitor:
  log_interval: 20
  track:
    - "accuracy"
    - "avg_response_len"
    - "format_error_rate"
    - "reward_std"
```

---

## 12. 一句话总结

RLVR 的关键不在“用了 RL”，而在：  
**把奖励从主观偏好改成可验证信号，并把训练闭环做成可审计工程系统。**
