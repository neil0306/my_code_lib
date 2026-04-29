# 大模型训练流程：PT / SFT / RLHF / DPO

---

## 1. 训练流程全景

典型流程：

1. 预训练（PT）：学习语言与知识底座；
2. 监督微调（SFT）：学习指令遵循与输出格式；
3. 偏好对齐（RLHF 或 DPO）：对齐人类偏好和产品行为。

这三步是能力、可用性、可控性的分工。

---

## 2. 预训练（PT）

目标：next-token 预测。

$$
\max_	heta \sum_t \log p_	heta(x_t|x_{<t})
$$

直觉：

- 模型通过海量文本学“语言统计规律 + 世界知识”；
- 得到的是“很会续写”的基础模型，而非天然会对话的助手。

工程重点：

- 数据配方（覆盖/质量/多样性）；
- 训练稳定性（优化器、学习率、并行策略）；
- 成本控制（吞吐、故障恢复、检查点策略）。

---

## 3. 监督微调（SFT）

目标：让模型学会“按人希望的方式回答”。

常见格式：

- 单轮：`instruction/input/output`；
- 多轮：`messages[{role, content}]`。

关键机制：

- 只对 assistant token 计算损失；
- system/user token 常用 mask，不参与训练目标。

直觉：

- PT 决定“懂多少”；
- SFT 决定“怎么说、怎么做”。

---

## 4. 偏好对齐：RLHF 与 DPO

### 4.1 RLHF（常见三段）

- 先 SFT；
- 再训奖励模型（偏好对）；
- 再用 PPO 优化策略。

优点：

- 能直接优化“偏好得分”；
- 适合复杂行为约束。

难点：

- 训练复杂、超参敏感；
- 奖励欺骗与分布漂移风险高。

### 4.2 DPO

思想：把偏好优化转为监督学习式目标，绕过在线 RL 的复杂环节。

优点：

- 实现简单、训练更稳；
- 工程成本低于 PPO。

局限：

- 仍受偏好数据质量上限约束；
- 对复杂长期信用分配不如 RL 直接。

---

## 5. SFT 数据质量为什么比数量更敏感

经验规律：

- 少量高质量数据，常优于大量噪声数据；
- 数据格式、风格一致性、事实正确率，对行为塑形影响极大。

原因：

- SFT 阶段样本规模小、梯度信号集中；
- 错误模式会被高效蒸馏进模型行为。

---

## 6. 幻觉与灾难性遗忘（工程视角）

### 幻觉

- 诱因：强制回答未知问题、错误示范、奖励偏差；
- 缓解：不确定性表达、检索增强、可验证任务监督。

### 灾难性遗忘

- 诱因：SFT/对齐阶段偏离预训练分布过大；
- 缓解：混入 LM loss、回放预训练样本、控制学习率与更新步数。

---

## 7. 常见误用 -> 正确写法

- 误用：把 PT 模型直接当产品模型。  
  正确：至少经 SFT，再做安全与偏好对齐。
- 误用：只看 SFT 数据量，不看质量。  
  正确：先做质量门控，再扩规模。
- 误用：对齐阶段只追分数。  
  正确：同时监控幻觉率、拒答率、越狱成功率。
- 误用：一次性重对齐覆盖全部场景。  
  正确：按场景分层对齐并滚动迭代。

---

## 8. 可执行训练蓝图

1. 预训练底座完成并通过基础评估；
2. 设计 SFT 数据规范与质量门控；
3. 小步 SFT + 行为验收（格式、遵循、安全）；
4. 选择 RLHF 或 DPO 做偏好对齐；
5. 做离线评测 + 在线灰度 + 失败样本回流；
6. 周期性迭代数据与策略。


## 9. 代码化模板：PT/SFT/RLHF/DPO 训练配置（可直接改）

```yaml
# train_plan.yaml
stages:
  pt:
    enabled: true
    objective: "next_token"
    max_steps: 300000
    lr: 3e-4
  sft:
    enabled: true
    data_format: "chatml"
    max_steps: 5000
    lr: 1e-5
    assistant_only_loss: true
  rlhf:
    enabled: false
    algo: "ppo"
    kl_coef: 0.02
    rollout_batch_size: 256
  dpo:
    enabled: true
    beta: 0.1
    max_steps: 3000

acceptance:
  min_task_score: 0.72
  max_hallucination_rate: 0.08
  min_safety_refusal_precision: 0.90
```

**常见误用 -> 正确写法**
- 误用：SFT 与 DPO 同时打开但无优先级。  
  正确：明确流水顺序（通常 SFT -> DPO/RLHF）。

## 10. 代码化模板：阶段验收脚本（伪代码）

```python
def stage_gate(stage_name, metrics, gates):
    failed = []
    if metrics["task_score"] < gates["min_task_score"]:
        failed.append("task_score too low")
    if metrics["hallucination_rate"] > gates["max_hallucination_rate"]:
        failed.append("hallucination too high")
    if metrics["safety_refusal_precision"] < gates["min_safety_refusal_precision"]:
        failed.append("safety precision too low")
    return failed


def can_promote_to_next_stage(stage_name, metrics, gates):
    return len(stage_gate(stage_name, metrics, gates)) == 0
```

**工程注意**
- 每阶段都要保存“可回滚检查点”；
- 失败样本要回流数据池（SFT / 偏好对）做下一轮迭代。

---

## 11. 一句话总结

训练流程的本质是分层优化：  
**PT 学能力，SFT 学行为，对齐学边界。**
