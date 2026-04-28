# GPU 与 GPU 优化笔记（Chapter 6）

本笔记整理自 `diy-llm/docs/chapter6`

---

## 本章学习地图

你可以把这一章理解为三条主线：

1. **硬件主线**：GPU 为什么适合大模型（CPU vs GPU、SM/Warp、分层内存）。
2. **优化主线**：为什么训练推理越来越“内存墙化”，以及常见优化手段（低精度、融合、重计算、合并访问、Tiling）。
3. **Attention 主线**：FlashAttention / PageAttention 分别优化了哪一层问题。

---

## 1. GPU 从图形芯片到 AI 引擎

### 1.1 CPU vs GPU：本质不是谁强，而是目标不同

![GPU 和 CPU 的结构](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-1-GPU和CPU的结构.png)

- **CPU 目标**：最小化单任务延迟（latency）。
- **GPU 目标**：最大化总体吞吐（throughput）。

直觉上：

- CPU 像“全能指挥官”，控制逻辑很重（分支预测、乱序执行、缓存一致性）。
- GPU 像“海量工人车间”，每个工人做简单重复的乘加，靠数量取胜。

这就是为什么矩阵乘法（大量同构乘加）天然更匹配 GPU。

---

### 1.2 GPU 的历史脉络（浓缩版）

- 早期显卡：主要做显示输出。
- 3D 加速时代：固定功能流水线，开始硬件几何/光照。
- 1999 GeForce 256：GPU 概念正式提出。
- 2006 CUDA：GPU 进入 GPGPU（通用计算）时代。
- 2012 AlexNet：GPU 成为深度学习基础设施。

---

## 2. A100 结构与执行模型

### 2.1 A100 的层级结构（看懂“算力从哪来”）

![GPU 的结构](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-2-GPU的结构.png)

![GPU 核心架构](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-3-GPU核心的架构.png)

层级可以粗看成：`GPU -> GPC -> TPC -> SM -> CUDA Core / Tensor Core`。

其中最关键的是 **SM（Streaming Multiprocessor）**，因为它是 GPU 的“执行原子单元”。

![SM 结构](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-4-SM的架构.png)

SM 内部有：

- 标量/向量执行单元（CUDA cores）
- Tensor Cores（矩阵乘专用）
- 寄存器文件
- 共享内存/L1

---

### 2.2 Warp / Block / Thread：GPU 怎么并行

![SM 执行](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-5-SM的执行.png)

- **Thread**：最小计算单元。
- **Warp**：32 个线程，SM 调度最小单位。
- **Block**：线程块，映射到某个 SM 执行。

直觉类比：

- Thread = 工人
- Warp = 一辆必须同进同退的班车
- Block = 一个施工队
- SM = 一个工地

#### 为什么分支会慢（Warp Divergence）

同一个 Warp 内如果线程走不同分支，硬件会“分批串行执行”各分支。结果：

- 有的线程在当前分支被 mask 掉（空转）
- 总耗时接近“各分支耗时之和”

所以 GPU 代码里常说“尽量减少分支发散”。

---

## 3. GPU 分层内存模型

![GPU 内存模型](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-6-GPU的内存模型.png)

### 3.1 记忆口诀

**越靠近 SM，越快、越小、越贵。**

从慢到快（也从大到小）大致是：

`HBM/Global -> L2 -> Shared/L1 -> Register`

### 3.2 为什么一定要分层

因为“速度、容量、成本”不可能三角：

- 全用寄存器：快但贵且太小。
- 全用显存：大但慢，算力会饿死在等数据。

分层本质是：

- 用便宜大内存存全集
- 用小而快内存缓存热点
- 把重复访存变成片上复用

这其实就是后面 Tiling / FlashAttention 的底层思想来源。

---

## 4. Roofline 与“内存墙”

![计算与内存扩展失衡](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-9-计算速度扩展的比内存扩展速度更快.png)

![屋顶线模型](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-10-屋顶线模型.png)

### 4.1 公式直觉

Roofline 的核心关系可以写成：

$$
\text{Throughput} \le \min\left(\text{Peak Compute},\ \text{Memory Bandwidth} \times \text{Operational Intensity}\right)
$$

其中：

- `Operational Intensity = FLOPs / Bytes`（每搬 1 字节做多少计算）

### 4.2 为什么这个公式好用

- 当 `OI` 低时，`Bandwidth * OI` 小，系统在斜线区，**memory-bound**。
- 当 `OI` 高时，碰到计算屋顶，**compute-bound**。

所以优化通常就两件事：

1. 提高 OI（减少无效访存，做更多片上复用）。
2. 提升可用算力/带宽（硬件或内核级优化）。

---

## 5. 常见 GPU 性能优化（工程视角）

## 5.1 避免串行化（减少 Warp Divergence）

```c
// 同一个 warp 里的线程如果分支不同，会被串行化执行
if (x < 4) {
    A;
    B;
} else {
    X;
    Y;
}
Z;
```

优化方向：

- 让同一个 warp 处理“控制流相似”的数据。
- 条件逻辑改写为查表、掩码运算、分组 kernel（视场景而定）。

---

### 5.2 低精度：用数值位宽换吞吐

![低精度提升速度](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-11-低精度提升速度.png)

核心原因是三重叠加：

1. **算术单元更便宜**：低位宽电路更小，同面积可堆更多计算单元。
2. **访存更省**：FP16/BF16 数据量约是 FP32 一半。
3. **Tensor Core 专门加速**：低精度矩阵乘可走更高吞吐路径。

实践要点：

- 常见是“低精度乘 + 高精度累加”（如 FP16 输入、FP32 accumulate）。
- 需要 loss scaling / 动态缩放避免 underflow/overflow。

---

### 5.3 算子融合（Operator Fusion）

![算子融合演示](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-13-算子融合演示.png)

![算子融合示例](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-14-算子融合示例.png)

直觉：

- 不融合：每一步都“写回全局内存 -> 再读回来”。
- 融合后：中间结果尽量留在寄存器/共享内存，最后一次性写回。

收益来自：

- 减少 HBM 往返
- 减少 kernel launch 开销

---

### 5.4 重计算（Activation Recomputation / Checkpointing）

![反向传播](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-15-反向传播算法.png)

![sigmoid 重计算示意](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-16-三个sigmoid函数堆叠.png)

核心 trade-off：

- 少存中间激活（省显存/带宽）
- 反向时重算前向片段（多算力）

当系统处于 memory-bound 时，这个交易通常划算。

---

### 5.5 内存合并访问（Memory Coalescing）

![突发模式](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-17-突发模式.png)

![同线程块合并访问](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-18-合并同一个线程块中的内存访问.png)

硬件按突发段取数，若 warp 内线程访问地址连续，就能合并成少量事务；地址离散就会事务爆炸。

矩阵类 kernel 中，线程映射方式（行优先/列优先）对性能影响可达数量级。

---

### 5.6 分块（Tiling）

![矩阵乘法分块](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-20-矩阵乘法分块.png)

![矩阵乘法 Tiling 化](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-21-矩阵乘法分块（Tiling）化.png)

![Tiling 数学分析](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter6/images/6-22-矩阵乘法分块的数学分析.png)

**idea**：先把小块搬到共享内存，在片上反复复用后再换下一块。

若 tile 大小为 `T`，可把大量“全局内存重复读”转化为“共享内存重复读”，典型收益量级与 `T` 同阶。

同时要注意：

- tile 太大：超共享内存/寄存器预算
- tile 太小：复用不够
- 维度不对齐：破坏合并访问

---

## 6. FlashAttention：不改 O(N^2)，但大幅减少 IO

<div align="center">
<img width="1188" height="438" src="https://github.com/user-attachments/assets/947f2883-e686-4b5e-afa5-90f8fa2caa95" />
<p>图 6.24 FlashAttention V1 原理图</p>
</div>

### 6.1 标准 Attention 的瓶颈

标准实现常显式构造 `N x N` attention score / probability，中间张量会频繁读写 HBM。

长序列下，瓶颈通常不在“算不动”，而在“搬不动”。

### 6.2 FlashAttention 的核心思想

- 分块计算 `QK^T`
- 在块内做 online softmax
- 直接与 `V` 累积输出
- 不把完整 attention matrix 落到 HBM

结果：减少 IO，提升数据复用，很多场景从 memory-bound 向 compute-bound 靠近。

---

### 6.3 online softmax：公式与直觉

对第 `i` 个 Q 块，维护三类状态：

- `m_i`：当前全局最大值（数值稳定关键）
- `l_i`：归一化分母累加
- `O_i`：加权输出累加

更新式：

$$
m_{new} = \max(m_i, m_{ij})
$$

$$
l_i \leftarrow e^{m_i - m_{new}} l_i + \sum e^{S_{ij} - m_{new}}
$$

$$
O_i \leftarrow e^{m_i - m_{new}} O_i + e^{S_{ij} - m_{new}}V_j
$$

最后：

$$
O_i \leftarrow O_i / l_i
$$

#### 为什么这样“work”

- 每次引入新块时，最大值可能改变；旧统计要按新基准重标定（`exp(m_i - m_new)`）。
- 这保证了“分块累计 == 全局一次性 softmax”（除浮点误差）。
- 避免了显式保存整块 attention matrix。

---

### 6.4 FlashAttention V1 / V2 / V3 演进

- **V1**：解决 IO（分块 + online softmax）。
- **V2**：提升并行调度（split-KV 等），减少串行依赖造成的 Tensor Core 空转。
- **V3**：面向 H100，异步流水 + WGMMA + FP8 混合精度，进一步提升重叠度和吞吐。

一句话：

从“少搬运”到“更并行”再到“搬运与计算重叠”。

---

### 6.5 Flashattention.ipynb 代码整理（详细注释版）

> 说明：下面代码基于 `chapter6/Flashattention.ipynb`，保留核心结构并补充注释。

```python
import torch
import time
import torch.nn.functional as F
from torch.nn.attention import sdpa_kernel, SDPBackend

# ------------------------------------------------------------
# 1) 两种 attention 后端包装
# ------------------------------------------------------------

def attention_math(q, k, v, is_causal=True):
    """
    标准数学后端（MATH）：
    - 通常更通用
    - 不一定最快
    - 常作为结果对照基线
    """
    with sdpa_kernel(SDPBackend.MATH):
        return F.scaled_dot_product_attention(q, k, v, is_causal=is_causal)


def attention_flash(q, k, v, is_causal=True):
    """
    FlashAttention 后端（FLASH_ATTENTION）：
    - 仅是“偏好选择”而非绝对强制
    - 若形状/硬件/数据类型等条件不满足，PyTorch 可能回退到其他后端
    """
    with sdpa_kernel(SDPBackend.FLASH_ATTENTION):
        return F.scaled_dot_product_attention(q, k, v, is_causal=is_causal)


# ------------------------------------------------------------
# 2) 数值一致性验证
# ------------------------------------------------------------

# B: batch, H: num_heads, L: seq_len, D: head_dim
B, H, L, D = 16, 32, 512, 128

# 真实测试里应放到 GPU：device="cuda"
q = torch.randn(B, H, L, D, device="cuda", dtype=torch.float16)
k = torch.randn(B, H, L, D, device="cuda", dtype=torch.float16)
v = torch.randn(B, H, L, D, device="cuda", dtype=torch.float16)

out_math = attention_math(q, k, v)
out_flash = attention_flash(q, k, v)

# 这里看最大绝对误差，一般会在 1e-6 ~ 1e-3（与 dtype/后端有关）
max_diff = (out_math - out_flash).abs().max().item()
print("max diff:", max_diff)


# ------------------------------------------------------------
# 3) 基准测试函数（注意 GPU 计时规范）
# ------------------------------------------------------------

def benchmark_attention(mode, q, k, v, warmup=5, repeats=20):
    """
    用 CUDA Event 计时，流程要点：
    1) 预热：让 kernel / cache / 频率状态稳定
    2) synchronize：确保计时前没有未完成异步任务
    3) event.record：记录 GPU 时间戳
    4) 再次 synchronize：确保任务真正执行完再读时间
    """

    # 预热（不计时）
    for _ in range(warmup):
        if mode == "math":
            attention_math(q, k, v)
        elif mode == "flash":
            attention_flash(q, k, v)
        else:
            raise ValueError(f"unknown mode: {mode}")

    torch.cuda.synchronize()

    start = torch.cuda.Event(enable_timing=True)
    end = torch.cuda.Event(enable_timing=True)

    start.record()
    for _ in range(repeats):
        if mode == "math":
            attention_math(q, k, v)
        else:
            attention_flash(q, k, v)
    end.record()

    torch.cuda.synchronize()

    # 返回平均毫秒
    return start.elapsed_time(end) / repeats


# ------------------------------------------------------------
# 4) 不同序列长度下的速度对比
# ------------------------------------------------------------

SEQ_LENS = [64, 128, 256, 512, 1024, 2048, 4096]
B, H, D = 4, 8, 64

print(f"Params: B={B}, H={H}, D={D}")
print(f"{'SeqLen':>8} | {'Math(ms)':>10} | {'Flash(ms)':>10} | {'Speedup':>8}")

for L in SEQ_LENS:
    q = torch.randn(B, H, L, D, device="cuda", dtype=torch.float16)
    k = torch.randn(B, H, L, D, device="cuda", dtype=torch.float16)
    v = torch.randn(B, H, L, D, device="cuda", dtype=torch.float16)

    t_math = benchmark_attention("math", q, k, v)
    t_flash = benchmark_attention("flash", q, k, v)
    speedup = t_math / t_flash

    print(f"{L:8d} | {t_math:10.2f} | {t_flash:10.2f} | {speedup:8.2f}x")
```

---

## 7. PageAttention：解决 KV Cache 的“碎片化”

<div align="center">
<img width="1362" height="276" src="https://github.com/user-attachments/assets/3ba4f70d-6c25-4c31-b4a2-5de6da8570cd" />
<p>图 6.26 传统 KV Cache 分布</p>
</div>

<div align="center">
<img width="800" height="480" src="https://github.com/user-attachments/assets/61ad87a3-7544-477e-84cc-2c385589f2e0" />
<p>图 6.27 显存占用分析</p>
</div>

### 7.1 它解决的不是“算得慢”，而是“存得乱”

传统做法常给每个请求预留连续 KV 空间：

- 会有**内部碎片**（分多了没用完）
- 会有**外部碎片**（空闲总量够但不连续，分不出来）

PageAttention 思路：

- 把 KV 按固定 page 切块
- 用 block table 维护逻辑地址 -> 物理地址映射
- 逻辑连续、物理可不连续

即：

`虚拟地址 -> 页表 -> 物理地址`

这样显著提高显存利用率与并发吞吐。

---

## 8. FlashAttention vs PageAttention（一句话区分）

- **FlashAttention**：算子级优化，目标是减少 attention 计算中的 IO 压力（算得更快）。
- **PageAttention**：系统级优化，目标是提升 KV cache 管理效率（存得更省）。

两者通常是互补关系，在推理系统中可叠加使用。

---

## 9. 国产 GPU 速览（阅读建议）

chapter6 原文已覆盖沐曦、昆仑芯、海光、天数智芯、摩尔线程、平头哥、昇腾等。

阅读建议：

1. 把“峰值算力”与“实测吞吐、生态兼容、迁移成本”分开看。
2. 优先关注框架兼容（PyTorch/Transformers）、算子覆盖、集群通信能力。
3. 对公开参数保持时间敏感：芯片代际和规格变化非常快。

---

## 10. 补充知识点（额外整理）

### 10.1 为什么 BF16 训练常比 FP16 更稳

- FP16 指数位少，动态范围窄，梯度易下溢/上溢。
- BF16 保留与 FP32 同级指数位，动态范围大很多，通常更稳。
- 代价是尾数精度低一些，但大模型训练往往能接受。

### 10.2 FlashAttention 并非“总是更快”

在小模型、小 batch、短序列时，瓶颈可能是 launch-bound 或 compute-bound，FlashAttention 的额外调度成本可能抵消收益；在长序列/大 batch 的 memory-bound 场景，优势通常更明显。

### 10.3 内存墙时代的通用优化顺序（实战）

1. 先确认瓶颈（profile）
2. 再做低精度/融合/Tiling
3. 然后调 kernel 映射与访存模式
4. 最后做系统级并发与缓存策略

---

## 参考资料

### Chapter 6 原始资料

- Datawhale chapter6 原文（GPU 与优化）
- `Flashattention.ipynb`（代码实验）

### 论文与官方文档（补充）

- FlashAttention: [https://arxiv.org/abs/2205.14135](https://arxiv.org/abs/2205.14135)
- FlashAttention-2: [https://arxiv.org/abs/2307.08691](https://arxiv.org/abs/2307.08691)
- FlashAttention-3（Tri Dao 博客与论文链接汇总）: [https://tridao.me/blog/2024/flash3/](https://tridao.me/blog/2024/flash3/)
- vLLM / PagedAttention 论文：[https://arxiv.org/abs/2309.06180](https://arxiv.org/abs/2309.06180)
- NVIDIA A100 架构白皮书：[https://resources.nvidia.com/en-us-tensor-core/nvidia-ampere-architecture-whitepaper](https://resources.nvidia.com/en-us-tensor-core/nvidia-ampere-architecture-whitepaper)
- NVIDIA H100 架构白皮书：[https://resources.nvidia.com/en-us-tensor-core/nvidia-hopper-architecture-whitepaper](https://resources.nvidia.com/en-us-tensor-core/nvidia-hopper-architecture-whitepaper)
- Mixed Precision Training（NVIDIA）: [https://docs.nvidia.com/deeplearning/performance/mixed-precision-training/](https://docs.nvidia.com/deeplearning/performance/mixed-precision-training/)

