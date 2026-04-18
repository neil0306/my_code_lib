# PyTorch 与资源核算

本章深入**模型训练的机制**：不直接展开完整 Transformer 架构，而是通过简单的线性模型，掌握训练深度学习模型时通用的「原语」（Primitives）。

**核心目标：**

1. **机制（Mechanics）：** 掌握 PyTorch 的基础操作（自底向上：张量 → 构建模型 → 优化器 → 训练循环）。
2. **思维模式（Mindset）：** 养成**资源核算**的习惯——清楚每一行代码大致消耗多少内存（GB）与算力（FLOPs）。
3. **直觉（Intuitions）：** 理解大模型为何依赖特定硬件与算法优化。

----------

## 3.1 为什么需要资源核算？

训练大型语言模型时，资源消耗直接转化为时间与成本。下面用两个典型问题体会其重要性。

### 3.1.1 场景一：时间估算

> **问题：** 在 1024 张 H100 上训练 70B（700 亿参数）模型，数据量 15T（15 万亿 tokens），大约要多久？

若只靠「写代码实测」来回答，往往几天甚至数月才能得到可用结论，因此需要 **Napkin math（餐巾纸估算）**。

#### 第一步：总工作量（FLOPs）

FLOPs（Floating Point Operations）衡量算法执行中浮点运算次数。训练本质是大规模 FLOPs。常用经验式：

$$
\text{总计算量} \approx 6 \times \text{参数量} \times \text{Token 数量}
$$

> 为何是 **6** 倍？
>
> - **前向传播：** 约 $2 \times$ 参数量（乘加各计）。
> - **反向传播：** 求梯度约 $4 \times$ 参数量（约为前向的 2 倍工作量）。

代入：

$$
6 \times (70 \times 10^9) \times (15 \times 10^{12}) \approx 6.3 \times 10^{24}\ \text{FLOPs}
$$

注意单位换算:
| 前缀   | 符号 | 数值              | 举例                       |
| ---- | -- | --------------- | ------------------------ |
| Kilo | K  | $10^3$      | 1K tokens = 1,000 tokens |
| Mega | M  | $10^6$      | 1M tokens = 100万 tokens  |
| Giga | G  | $10^9$      | 1B tokens = 10亿 tokens   |
| Tera | T  | $10^{12}$ | 15T tokens = 15万亿 tokens |


#### 第二步：硬件峰值算力

查阅 [NVIDIA H100 产品/白皮书](https://www.nvidia.com/en-sg/data-center/h100/)：FP16/BF16 峰值约 **1979 TFLOPS**（每秒万亿次浮点运算）。

![](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter3/images/3-1-H100数值细览.png)

> **注意：** 该峰值对应 FP16/BF16 且启用**结构化稀疏（Structured Sparsity）**时的理论吞吐。训练常见**稠密**模型（如 LLaMA 系列、Qwen3 等多数量级 checkpoint）时，可粗略按**峰值减半**使用，约 **990 TFLOPS**。

![](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter3/images/3-2-H100性能明细.png)

> **结构化稀疏**是一种压缩思路：常对稠密模型做约 50% 稀疏度的剪枝，模式为 **n:m**（连续 m 个权重中固定剪掉 n 个），例如 2:4、4:8、8:16。非结构化剪枝可按任意比例置零，灵活但通常更难在硬件上跑出同等效率。详见文末「深度答疑」第 1 节。

![](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter3/images/3-3-剪枝方法介绍.png)

990 TFLOPS 仍是**理论峰值**。实际受通信、访存、kernel 启动等影响，**模型算力利用率（MFU, Model FLOPs Utilization）** 很少接近 100%，工程上常按 **30%–60%** 估算；下文取 **50%**。

- 单卡有效算力 $\approx (990 \times 10^{12}) \times 0.5 \approx 5 \times 10^{14}$ FLOPS/s  
- 1024 卡合计 $\approx 5 \times 10^{17}$ FLOPS/s  

#### 第三步：时间

$$
\text{时间} \approx \frac{6.3 \times 10^{24}}{5 \times 10^{17}} \approx 1.26 \times 10^7\ \text{秒} \approx \textbf{146 天}
$$

这是量级意义上的「不间断理想训练」估算；真实排期还需叠加故障恢复、并行策略、checkpoint 等。

----------

### 3.1.2 场景二：内存估算

> **问题：** 8 张 H100（每张 80GB），用 **AdamW**、**朴素 FP32**（无混合精度、无压缩），大约能训多大的模型？

初学者易误以为：总显存 640GB，每参数 4 字节 → $640\text{GB}/4 \approx 160$B 参数。**这不成立。**

训练时显存除**参数**外，至少还有：

- **梯度：** 与参数同形状，朴素 FP32 再占 4 字节/参数。
- **优化器状态（以 AdamW 为例）：** 每个参数通常再存两个与参数同形状的 FP32 张量（一阶矩、二阶矩的指数滑动估计），共 8 字节/参数。含义见文末「深度答疑」第 2、3 节。

因此**仅这三项**的粗算为：

| 项目 | 字节/参数（朴素 FP32） |
|------|------------------------|
| 参数 | 4 |
| 梯度 | 4 |
| Adam 状态（$m,v$） | 8 |
| **合计** | **16** |

$$
\text{最大参数量（仅上式）} \approx \frac{640 \times 10^9\ \text{bytes}}{16\ \text{bytes/param}} \approx \textbf{40B}
$$

> 该估算**仍未计入** 随 **batch size、序列长度** 变化的 **激活(activations)** 显存与其它开销；无混合精度时，**实际可训规模往往小于** 40B。分析思路见「深度答疑」第 4 节。

----------

## 3.2 张量（Tensors）

### 3.2.1 张量基础

张量是深度学习中的基础数据结构：参数、梯度、优化器状态、输入与中间激活等都以张量表示。官方文档：[PyTorch Tensors](https://docs.pytorch.org/docs/stable/tensors.html)。

```python
import torch
import torch.nn as nn

# 基础创建方式
x = torch.tensor([[1., 2, 3], [4, 5, 6]])  # 从 Python 列表创建
x = torch.zeros(4, 8)   # 4×8 全零
x = torch.ones(4, 8)   # 4×8 全 1
x = torch.randn(4, 8)  # 4×8 标准正态随机

# 分配存储但不初始化（随后再自定义初始化）
x = torch.empty(4, 8)
nn.init.trunc_normal_(x, mean=0, std=1, a=-2, b=2)
# 截断正态：落在 [-2, 2] 外会重采样，常用于权重初始化
```

### 3.2.2 张量的操作

多数张量由对其它张量的运算得到；每个运算都对应显存与算力成本。

#### 张量视图（view）

许多操作只改变「解释方式」而不复制数据——即**视图（view）**。修改共享存储会在多个别名间可见。

```python
def same_storage(a: torch.Tensor, b: torch.Tensor) -> bool:
    """教学用：两 tensor 是否共享同一块底层存储。"""
    return a.untyped_storage().data_ptr() == b.untyped_storage().data_ptr()


x = torch.tensor([[1., 2, 3], [4, 5, 6]])

# 以下操作通常不复制数据，只改变解释方式（视图）
row0 = x[0]
col1 = x[:, 1]
x_view = x.view(3, 2)
x_t = x.transpose(1, 0)

assert same_storage(x, x_t)  # 转置结果常与 x 共享存储

# 修改 x 会影响与其共享存储的视图
x[0, 0] = 100
assert row0[0].item() == 100
```

> **连续（contiguous）内存：** 若元素在内存中不是按行主序紧密排列，则张量为**非连续**。`transpose` 常产生**非连续张量**。

```python
x = torch.tensor([[1., 2, 3], [4, 5, 6]])
y = x.transpose(1, 0)
assert not y.is_contiguous()
```

非连续张量可能无法直接 `view` 成某些形状：

```python
try:
    y.view(2, 3)
except RuntimeError as e:
    assert "view size is not compatible" in str(e)
```

解决：先 `.contiguous()` 再 `view`（会**复制**到新存储）。

```python
y = x.transpose(1, 0).contiguous().view(2, 3)
assert not same_storage(x, y)  # 一般已为新存储
```

#### 逐元素操作（element-wise）

对每个位置独立运算，不涉及矩阵乘法的「行×列」组合。

```python
x = torch.tensor([1, 4, 9], dtype=torch.float32)
assert torch.equal(x.pow(2), torch.tensor([1, 16, 81]))
assert torch.equal(x.sqrt(), torch.tensor([1, 2, 3]))
assert torch.equal(x.rsqrt(), torch.tensor([1, 1 / 2, 1 / 3]))  # 1/sqrt(x)

assert torch.equal(x + x, torch.tensor([2, 8, 18]))
assert torch.equal(x * 2, torch.tensor([2, 8, 18]))
assert torch.equal(x / 0.5, torch.tensor([2, 8, 18]))
```

#### `triu` 与因果（causal）掩码

**因果语言模型**在位置 $j$ 预测时不能看见「未来」token，需在注意力分数上屏蔽非法位置。`torch.triu` 常用来构造 **上三角（含对角）为允许、下三角为禁止** 的 0/1 掩码（具体与 `scores + mask` 的符号约定有关，见「深度答疑」第 5 节）。

```python
# 上三角示例：常用于构造 causal attention mask 的骨架
x = torch.ones(3, 3).triu()
# tensor([[1., 1., 1.],
#         [0., 1., 1.],
#         [0., 0., 1.]])
```

> 原讲义表述「$M[i,j]$ 为位置 $i$ 对位置 $j$ 的贡献」易与「越靠后贡献越大」混淆；索引含义与注意力中 query/key 的对应关系见「深度答疑」第 5 节。

----------

## 深度答疑（扩展阅读）

以下各节：**直觉 → 形式化/伪代码 → 常见误区 → 参考文献（可点击 URL）**。文献类型包括厂商技术文档、同行评审论文与工程博客，使用时注意语境与假设。

### 1. 结构化稀疏、半结构化（2:4）在做什么？剪枝为何可能有效？为何常先训稠密？

**直觉：** 「稀疏」即让许多权重为 0，减少乘加。**非结构化**稀疏可任意位置为 0，对通用 GEMM 不友好。**半结构化 / 细粒度结构化**（如 **2:4**）要求每连续 4 个权重中**恰好 2 个为 0**（50% 稀疏），模式固定，便于 GPU **Sparse Tensor Core** 走专用数据通路，从而在满足模式时接近**翻倍**有效吞吐（相对稠密峰值口径需对照厂商表）。NVIDIA 对 Ampere 及后续架构的说明见技术博客与架构白皮书。

**伪代码（推理侧剪枝到 2:4 的示意）：** 将权重展平或按硬件要求分组，每组 4 个元素保留绝对值最大的 2 个，其余置零；再（可选）微调恢复精度。

```text
算法: 幅度剪枝到 2:4（示意）
输入: 权重向量 W，长度可被 4 整除
输出: 满足 2:4 模式的稀疏权重 W'

for 每组 g in 将 W 划分为长度 4 的连续组:
    idx = argsort(|g|)  # 按绝对值排序的下标
    keep = idx 中最大的两个下标
    对 k in {0,1,2,3}:
        若 k 不在 keep: g[k] ← 0
    写回 W'
```

**为何剪枝有时仍能保持精度（谨慎表述）：** 过参数化网络中存在冗余；重要连接可用较少非零权重近似。相关讨论包括**彩票假设**（Large Random Networks 中存在可训练的稀疏子网络）等，但具体任务与训练预算下结论需实验验证。

- Frankle & Carbin, *The Lottery Ticket Hypothesis*, [arXiv:1803.03635](https://arxiv.org/abs/1803.03635)

**为何常「先稠密训练再结构化剪枝」而非从头稀疏训练？**

- 稠密优化问题相对成熟，收敛路径更稳；一开始就强加 2:4 等硬约束会使可行集高度非凸，优化更难。
- 许多流程是 **dense pre-train → structured prune → finetune**，用稠密阶段得到的好表示与权重尺度，再投影到硬件友好稀疏模式并微调补偿误差。

**参考文献：**

- NVIDIA, *Structured Sparsity in the NVIDIA Ampere Architecture*, [developer.nvidia.com/blog/structured-sparsity-in-the-nvidia-ampere-architecture-and-applications-in-search-engines/](https://developer.nvidia.com/blog/structured-sparsity-in-the-nvidia-ampere-architecture-and-applications-in-search-engines/)
- PyTorch, *Semi-Structured (2:4) Sparsity*, [pytorch.org/blog/accelerating-neural-network-training/](https://pytorch.org/blog/accelerating-neural-network-training/)
- NVIDIA, *A100 Tensor Core GPU Architecture*（含 Fine-Grained Structured Sparsity 章节）, [images.nvidia.com/aem-dam/en-zz/Solutions/data-center/nvidia-ampere-architecture-whitepaper.pdf](https://images.nvidia.com/aem-dam/en-zz/Solutions/data-center/nvidia-ampere-architecture-whitepaper.pdf)

### 2. 如何理解 AdamW 的「一阶矩估计」「二阶矩估计」与计算原理？

**符号：** 第 $t$ 步梯度 $g_t = \nabla_\theta L(\theta_{t-1})$（与参数同形状的张量；下面公式对逐坐标成立）。

**一阶矩估计 $m_t$：** 梯度 $g_t$ 的**指数移动平均（EMA）**，刻画「最近一段时间梯度指向哪里」，与带动量的 SGD 思想类似。

**二阶矩估计 $v_t$：** $g_t^2$（逐坐标平方）的 EMA，刻画「每个坐标梯度尺度有多大」，用于**自适应缩放**步长（与 RMSprop 类似）。

**更新（Adam 核心，带偏差修正）：** 给定 $\beta_1,\beta_2 \in (0,1)$，学习率 $\alpha$，数值稳定项 $\epsilon$。

```text
初始化: m ← 0, v ← 0, t ← 0
重复直到收敛:
  t ← t + 1
  g ← 当前 mini-batch 梯度
  m ← β1 * m + (1 - β1) * g
  v ← β2 * v + (1 - β2) * (g * g)     # g*g 为逐元素平方
  m_hat ← m / (1 - β1^t)               # 偏差修正
  v_hat ← v / (1 - β2^t)
  θ ← θ - α * m_hat / (sqrt(v_hat) + ε)
```

**存储：** 朴素实现中 $m$、$v$ 与 $\theta$ 同形状，各 FP32 时约 **8 字节/参数**（与正文 16 B/param 粗算一致）。

**参考文献：**

- Kingma & Ba, *Adam: A Method for Stochastic Optimization*, [arXiv:1412.6980](https://arxiv.org/abs/1412.6980)

### 3. AdamW 的核心思想？大模型训练是否都用它？为何常用？

**相对 Adam + L2 正则：** 标准 Adam 常把 L2 正则项并入梯度，再被自适应分母缩放，使**权重衰减强度与自适应尺度耦合**，调参困难。 **AdamW** 将 **weight decay 与梯度更新解耦**：在参数更新时直接对 $\theta$ 乘 $(1 - \lambda)$（或等价形式），**不把 decay 混进 $m,v$ 的自适应除法里**，经验上泛化与超参搜索更友好。

**伪代码（在 Adam 更新后追加解耦衰减，示意）：**

```text
# 在完成 θ ← θ - α * m_hat / (sqrt(v_hat) + ε) 之后:
θ ← θ - λ * θ    # λ 为 weight decay 系数（实现细节因框架而异，常见等价写法）
# 实际库中常合并到一步更新里；关键是 decay 不通过修改 g 进入 m、v 的构造
```

**是否「大模型都在用 AdamW」：** **主流预训练与微调仍以 AdamW 及其变体为默认强基线**，但**不是唯一**选择。为省显存可用 **Adafactor**（因子化二阶统计量）；也有 **Lion**、**Muon** 等在特定设定下探索替代。表述上应写「默认常用 + 仍有替代路线」。

**参考文献：**

- Loshchilov & Hutter, *Decoupled Weight Decay Regularization* (AdamW), [arXiv:1711.05101](https://arxiv.org/abs/1711.05101)

### 4. 「未考虑批次大小和序列长度的激活值内存」是什么意思？如何分析影响？

**含义：** 正文 **16 字节/参数**只覆盖 **参数 + 梯度 + Adam 状态**。训练还需在前向中物化（或在反向中重计算）**激活**：各层输入输出、归一化统计、注意力中间结果等。这部分**不**由「参数量」单独决定，而强烈依赖 **batch size $B$**、**序列长度 $S$**、隐藏维 $h$、层数 $L$、是否使用 Flash Attention、是否 gradient checkpointing 等。

**量级直觉：**

- 许多激活张量尺寸正比于 **$B$** 与 **$S$**（以及 $h,L$），故增大 batch 或上下文长度会**近似线性**抬高多处激活峰值。
- **自注意力**若物化完整 $S\times S$ 的 score / prob 等，显存会出现 **$O(S^2)$** 项；**FlashAttention** 类算法通过分块与重计算将注意力相关显存压到更接近 **$O(S)$**（仍与实现与是否重计算有关）。

**如何分析 batch：** 在其它条件固定时，**成比例增大 $B$** 往往成比例增大「与 batch 维广播/堆叠」的激活占用，直至 OOM；可用减小 $B$、**梯度累积**维持等效大 batch。

**如何分析序列长度：** 拉长 $S$ 会同时抬高与 $S$ 成比例的张量与（未优化时的）$S^2$ 注意力项；长上下文训练常配合 **activation checkpointing**、**上下文并行**等。NVIDIA 对长上下文训练中的内存技术有系统介绍。

**参考文献：**

- NVIDIA, *Scaling to Millions of Tokens with Efficient Long-Context LLM Training*, [developer.nvidia.com/blog/scaling-to-millions-of-tokens-with-efficient-long-context-llm-training/](https://developer.nvidia.com/blog/scaling-to-millions-of-tokens-with-efficient-long-context-llm-training/)
- Dao 等, *FlashAttention: Fast and Memory-Efficient Exact Attention*, [arXiv:2205.14135](https://arxiv.org/abs/2205.14135)

### 5. `triu` 与「$M[i,j]$」「$i>j$」到底在说什么？是否「越靠后贡献越大」？

**先澄清误解：** **不是**「句子中越靠后的词本身贡献更大」。**因果约束**指的是：**在序列索引（时间）上，靠后的位置不能作为「未来信息」被靠前位置看见**。

**与注意力对齐的一种记号：** 设 query 位置为列索引 $j$，key 位置为行索引 $i$。因果掩码要求：query $j$ **只能**看见 key $i \le j$。因此 **$i > j$** 表示 **key 在 query 的未来**——必须屏蔽（注意力权重为 0）。这与「贡献随位置单调变大」无关；合法位置上权重由内容通过 softmax 决定。

**与 `torch.triu` 的关系：** `triu` 保留**上三角含对角**、抹掉下三角。若用 `mask` 与 `scores` 相加，常见约定是：允许位置加 0，禁止位置加 $-\infty$，再 softmax。具体哪一侧三角为「允许」取决于 `scores` 的行列维如何对应 query/key，**实现前务必统一约定**。

**参考文献：**

- Vaswani 等, *Attention Is All You Need*（因果掩码与自回归建模）, [arXiv:1706.03762](https://arxiv.org/abs/1706.03762)
- PyTorch, `torch.triu`, [docs.pytorch.org/docs/stable/generated/torch.triu.html](https://docs.pytorch.org/docs/stable/generated/torch.triu.html)
