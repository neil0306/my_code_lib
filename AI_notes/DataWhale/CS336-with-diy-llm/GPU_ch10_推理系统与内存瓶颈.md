# 推理系统与内存瓶颈

---

## 1. 训练与推理的系统差异

- 训练：可在序列维并行（teacher forcing + causal mask），高吞吐大矩阵计算；
- 推理：自回归逐 token，时间维串行，且每步依赖历史上下文。

所以在硬件上常见现象是：

- 训练阶段更像 compute-bound；
- 推理阶段更像 memory-bound。

---

## 2. 自回归推理两阶段：Prefill 与 Decode

### 2.1 Prefill（首轮上下文编码）

- 一次性处理 prompt；
- 计算并缓存各层 K/V。

### 2.2 Decode（逐 token 生成）

每一步：

1. 用最新 token 生成 Q/K/V；
2. 用 Q 与历史 K 做注意力；
3. 用历史 V 聚合输出；
4. 采样下一个 token；
5. 把新 K/V 追加进缓存。

---

## 3. KV Cache：收益与代价

注意力核心：

$$
\text{Attention}(Q,K,V)=\text{softmax}\left(\frac{QK^\top}{\sqrt{d_k}}\right)V
$$

### 3.1 收益

历史 token 不必重复做 K/V 投影，显著减少重复计算。

### 3.2 代价

随着序列增长，缓存体积和读取压力持续增长。

可粗略理解为：

$$
\text{KV memory}\propto \text{layers}\times\text{seq\_len}\times\text{heads}\times\text{head\_dim}
$$

这就是长上下文推理容易变慢、变贵的根因之一。

### 3.3 代码示例：KV Cache 的最小可读实现

```python
def softmax(x):
    # 数值稳定写法：先减 max，避免 exp 溢出
    e = np.exp(x - np.max(x))
    return e / e.sum()

def generate_kv_readable(prefix, max_len=10, min_len=3):
    # 把 token 转成 id，后续都在 id 空间运行
    ids = [token2id[w] for w in prefix]
    # 教学实现：用可读数组显式维护 K/V cache
    K_cache = np.zeros((0, d_model))
    V_cache = np.zeros((0, d_model))

    for _ in range(max_len):
        last_id = ids[-1]
        # 取最后一个 token 的 embedding
        x = E[last_id]
        q, k, v = x, x, x  # 示例中简化为同一表示

        # 关键动作：追加新 token 的 K/V 到 cache
        K_cache = np.vstack([K_cache, k])
        V_cache = np.vstack([V_cache, v])

        # 与全部历史 K 做打分（这一步随序列增长变重）
        att_scores = K_cache @ q / np.sqrt(d_model)
        att_weights = softmax(att_scores)
        context = att_weights @ V_cache

        # 教学简化：直接用 last_id 的 logits 表
        logits = manual_logits[last_id]
        probs = softmax(logits)
        next_id = int(np.argmax(probs))
        ids.append(next_id)
```

**常见误用 -> 正确写法**
- 误用：把教学版 `np.vstack` 当生产实现。
  正确：生产使用预分配或 paged KV，避免频繁重分配。
- 误用：不固定采样策略就比较速度。
  正确：固定 greedy/top-k/top-p 后再测延迟吞吐。

这段代码不是高性能实现，但非常直观地揭示了 KV cache 的“累计增长 + 重复读取”模式。

```python
# 最小调用示例（原文对应）
generate_kv_readable(["I"])
generate_kv_readable(["I", "deep"])
```

**idea**
- 最小代码把“为什么 decode 越跑越重”可视化：K/V 缓存长度单调增长，每步读取变多。

**工程注意**
- 真实系统不会用 `np.vstack` 持续扩容，而是预分配/分页管理（paged KV）降低重分配成本。
- 解码策略（greedy/top-k/top-p）会影响吞吐与延迟，评估时要固定。

---

## 4. 算术强度视角：为什么 decode 容易掉进内存墙

算术强度定义：

$$
I=\frac{\text{FLOPs}}{\text{Bytes Transferred}}
$$

对 `X(B×D) @ W(D×F)`：

- FLOPs 约 `2BDF`；
- 传输约 `BD + DF + BF`（再乘元素字节）。

当 `B=1`（典型在线单请求 decode）：

- `DF`（读权重）难以摊薄；
- `I` 偏低；
- 更容易 memory-bound。

---

## 5. 代码片段：算术强度函数（详细注释）

```python
def arithmetic_intensity(B, D, F, bytes_per_elem=2):
    # 输入激活读取：X 形状 [B, D]
    read_X = bytes_per_elem * B * D
    # 权重读取：W 形状 [D, F]，decode 时常是主要固定成本
    read_W = bytes_per_elem * D * F
    # 输出写回：Y 形状 [B, F]
    write_Y = bytes_per_elem * B * F

    # 总访存量（忽略缓存命中与额外临时张量）
    bytes_transferred = read_X + read_W + write_Y
    # GEMM 近似 FLOPs
    flops = 2 * B * D * F
    return flops / bytes_transferred
```

**常见误用 -> 正确写法**
- 误用：把该 AI 公式当精确性能预测器。
  正确：把它当方向判断工具，再用 profiler 验证。
- 误用：忽略 dtype 导致 bytes 估计错误。
  正确：按 FP16/BF16/FP8 等真实字节数代入。

### 直觉

增大 batch 时，`W` 可被同批样本复用，固定读权重成本被分摊，`I` 提升，吞吐改善。

**工程注意**
- 该模型是“上界近似”，真实 kernel 还受 cache 命中、算子融合、调度策略影响。
- 线上推理常由 SLA 限制 batch，理论最优 batch 不一定可用。

### 5.1 代码补充：硬件临界算术强度

```python
def h100_intensity():
    flops_per_second = 989e12     # FP16 峰值计算吞吐（示例取值）
    memory_bandwidth = 3.35e12    # HBM 峰值带宽（示例取值）
    return flops_per_second / memory_bandwidth
```

**常见误用 -> 正确写法**
- 误用：直接套厂商峰值作结论。
  正确：峰值只作上界，结合实测 effective bandwidth。
- 误用：跨硬件复用同一临界值。
  正确：每个硬件代际单独计算临界 AI。

用途：用“任务算术强度 vs 硬件临界值”快速判断 compute-bound 还是 memory-bound。

```python
if __name__ == "__main__":
    B, D, F = 1, 4096, 11008
    ai = arithmetic_intensity(B, D, F)
    h100_ai = h100_intensity()

    print(f"AI={ai:.4f}, H100 临界值={h100_ai:.1f}")
    if ai > h100_ai:
        print("结论：更可能 compute-bound")
    else:
        print("结论：更可能 memory-bound")

    for B_test in [1, 16, 64, 256, 512]:
        ai_test = arithmetic_intensity(B_test, D, F)
        print(f"B={B_test:4d} -> AI={ai_test:.2f}")
```

**常见误用 -> 正确写法**
- 误用：离线吞吐最优 batch 直接搬到线上。
  正确：结合 TTFT/SLA 约束选可用 batch。
- 误用：只看平均 token latency。
  正确：同时看 p95/p99 与队列等待时间。

这段补全了原文“batch 增大 -> 算术强度提升”的可执行验证逻辑。

**idea**
- 用“任务 AI vs 硬件临界 AI”做一眼判断，再决定优化方向（提算力利用 or 降访存）。

**工程注意**
- 峰值 FLOPs/带宽是理想值，实测通常偏低；判定时要预留安全裕度。
- 不同精度（FP16/BF16/FP8）和不同硬件代际临界值不同，不能直接复用常数。

---

## 6. 延迟、吞吐、TTFT 的三角权衡

推理系统常见指标：

- `TTFT`：首 token 延迟（交互感受最敏感）；
- `Latency`：后续 token 间隔；
- `Throughput`：系统单位时间总 token 产出。

常见冲突：

- 大 batch 有利于吞吐；
- 但排队/拼批会恶化单请求延迟。

因此线上系统通常采用动态批处理与调度策略做折中。

---

## 7. Prompt 压缩与推理效率

过长 prompt 会带来：

- prefill 计算成本上升；
- KV 初始体积更大；
- 后续 decode 每步注意力访问更重。

所以“有效 prompt”不是越长越好，而是信息密度更重要。

---

## 8. 线性注意力与混合注意力的意义

线性注意力/混合结构的价值是：

- 降低长序列注意力复杂度；
- 在某些场景降低缓存与访存压力。

但要注意：

- 自回归因果性仍必须满足；
- 并不是所有任务都能无损替代 softmax 注意力。

### 8.1 代码示例：线性注意力中的因果性实现

```python
def phi(x):
    return np.maximum(x, 0)  # 示例核函数

# 错误方式：一次性使用全局 KV，可能“看到未来”
KV_all = (phi(K).T @ V)
Y_wrong = (phi(Q) @ KV_all) / (phi(Q) @ phi(K).sum(axis=0, keepdims=True).T)

# 正确方式：前缀累加，仅使用历史信息
S = np.zeros((d, d))
Z = np.zeros((d, 1))
Y_correct = []
for i in range(L):
    ki, vi, qi = phi(K[i:i+1]).T, V[i:i+1].T, phi(Q[i:i+1]).T
    # 只累加到当前位置，保持因果性
    S += ki @ vi.T
    Z += ki
    y_i = (qi.T @ S) / (qi.T @ Z)
    Y_correct.append(y_i.flatten())
```

**常见误用 -> 正确写法**
- 误用：线性注意力一次性用全局 KV。
  正确：自回归必须按前缀递推，严格因果。
- 误用：分母不加稳定项。
  正确：在 `qi.T @ Z` 处加 `eps` 防数值爆炸。

这个对照是关键知识点：  
线性注意力做自回归生成时，必须按前缀递推，不能一次性混入全序列信息。

**工程注意**
- 任何“全序列并行”的线性注意力实现都要先确认是否破坏因果掩码。
- 前缀递推里要处理分母接近 0 的数值稳定问题（如加 `eps`）。

---

## 9. 知识衔接

- roofline 与 memory wall 提供理论底座；
- profiling 与 kernel 优化提供局部性能优化路径；
- 多卡通信与并行提供系统级扩展路径；
- 推理系统将这些原则统一到线上场景中。

---

## 10. 一句话总结

推理优化的关键不是“会不会算注意力”，而是：  
**在自回归约束下，如何降低每步 memory traffic，并在延迟与吞吐之间做系统级平衡。**

