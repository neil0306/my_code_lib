# 混合专家模型（MoE）：技术细节整理（Chapter 5）

本笔记整理自 datawhale 的 `diy-llm/docs/chapter5/chapter5_混合专家模型.md`，行文风格沿用前几章惯例：**先抓主线直觉，再落到公式/形状/工程取舍**，公式侧重"idea 从哪来、解决了什么问题"，代码侧重"每一行干了什么"，最后配合提问/误区强化记忆。

---

## 本章应学会什么

- **MoE 的核心思想**：如何把"参数容量"与"每-token 计算量"解耦，以及为什么这件事重要。
- **路由机制（Router）**：TC (Token Choice) 与 EC (Expert Choice) 两种主流模式的原理、区别、各自优劣。
- **负载均衡**：为什么专家容易"偏科"，有哪些工程手段（辅助损失、噪声门控、容量限制等）解决。
- **典型变体**：Switch Transformer 与 DeepSeekMoE 的核心创新点。
- **MoE vs Dense**：什么时候用 MoE，什么时候用稠密模型，以及 upcycling 路线的工程坑。

---

## 5.1 为什么需要 MoE？——参数容量与计算量的矛盾

### 5.1.1 稠密模型的瓶颈

在标准（稠密）Transformer 中，**每个 token 的前向传播都会经过模型所有参数**。这意味着：

- **想提升模型能力** → 通常只能加宽（更大 $d_{model}$、更宽 FFN）或加深（更多层），这两条路都会**线性甚至超线性地增加 FLOPs**。
- **同等算力下的天花板**：每 token FLOPs 受限，参数规模被 FLOPs 预算锁死。

### 5.1.2 MoE 的核心 idea：稀疏激活

MoE（Mixture of Experts，混合专家模型）的核心解法是：

> **模型总体包含大量参数，但每个 token 只激活其中一小部分（少数专家）。**

这样就实现了 **参数容量（model capacity）** 与 **每-token 计算量（FLOPs per token）** 在一定程度上的**解耦**：

| 维度 | 稠密（Dense） | 稀疏（MoE） |
|------|---------------|-------------|
| 总参数量 $P$ | 较小 | 可以很大（多个专家叠加）|
| 每-token FLOPs | $\propto P$ | $\approx$ 少数 $k$ 个专家的 FLOPs |
| 参数利用率 | 每步用全部 | 每步只用 $k/N$ 的专家 |

**直觉类比**：MoE 像一个拥有大量书架的图书馆——读者（token）不需要翻遍所有书架，而是由图书管理员（路由器）引导，只去相关书架（少数专家）取书。整个图书馆的知识容量很大，但每次检索只访问一小部分。

> **重要前提**：MoE 在**参数规模较大、数据和计算资源充足**时优势最明显。在小规模或资源受限场景下，其表现可能不如对应的稠密模型，且具体效果依赖任务类型、数据量和实现细节。

---

## 5.2 路由机制（Router）

### 5.2.1 通用门控公式

设共有 $N$ 个专家，输入为 $x$（token 的隐藏状态），$E_i(\cdot)$ 表示第 $i$ 个专家的输出，门控函数 $G(\cdot)$ 决定每个专家的权重，则 MoE 层的输出为：

$$
y = \sum_{i \in \mathcal{T}} G_i(x) \cdot E_i(x)
$$

**关键点**：$\mathcal{T}$ 是通过 **Top-$k$** 选出的稀疏激活专家子集，$N - k$ 个未被选中的专家权重为 0，**完全不参与本次计算**，这正是"稀疏激活"的含义。

门控得分的计算分两步：

1. **打分（Scoring）**：$h(x) = x \cdot W_g$，其中 $W_g$ 是可学习的路由投影矩阵，将 token 特征映射为长度为 $N$ 的得分向量（logits）。
2. **稀疏化（Sparsification）**：保留得分最高的 $k$ 个专家，对这 $k$ 个得分做 Softmax 归一化作为权重；其余 $N-k$ 个专家权重强制置零。

> **$W_g$ 的物理意义**：它是"专家特长档案"，将 token 的特征映射到"与各专家匹配程度"的打分空间。路由器通过学习 $W_g$ 来习得"这类 token 该找哪个专家"的判断能力。

### 5.2.2 TC 模式：Token 主动选专家（Token Chooses）

**核心思路**：对每一个 token（矩阵的每一行），在专家维度（列维度）上选 Top-K 个专家。  
**直觉**：像"学生主动找导师"——每个学生（token）根据自己的需求，选择最合适的 $k$ 位老师（专家）。

![图 5.1 词元选择模式（TC）](https://github.com/user-attachments/assets/648d1892-b01e-4d40-9c2b-50478d2eeccf)

**优势**：每个 token 都会被至少 $k$ 个专家处理，语义完整性高，不会出现 token 被遗漏的情况。

**劣势**：专家**负载不均衡**。热门专家可能被大量 token 选中，冷门专家长期闲置，导致"偏科"现象——高频领域表现良好，低频领域能力薄弱。

#### TC 简易实现

```python
import torch
import torch.nn as nn
import torch.nn.functional as F
import math

# ============================================================
# 专家网络：每个专家是一个独立的小型 FFN
# 结构：线性层 → ReLU → 线性层（先升维后降维）
# ============================================================
class Expert(nn.Module):
    def __init__(self, dim):
        super().__init__()
        self.ffn = nn.Sequential(
            nn.Linear(dim, dim * 4),   # 升维：扩展特征表达空间
            nn.ReLU(),                  # 非线性激活：引入非线性变换能力
            nn.Linear(dim * 4, dim)    # 降维：还原到原始隐藏维度
        )

    def forward(self, x):
        return self.ffn(x)


# ============================================================
# TC（Token Chooses）MoE 层
# 每个 token 主动选择自己最合适的 Top-K 个专家
# ============================================================
class TC_MoE(nn.Module):
    def __init__(self, dim, num_experts, k):
        super().__init__()
        self.num_experts = num_experts  # 总专家数 N
        self.k = k                      # 每个 token 激活的专家数 k
        # 路由器：将 dim 维特征映射到 num_experts 维得分向量
        self.router = nn.Linear(dim, num_experts)
        # 专家列表：每个专家是独立的 FFN 模块
        self.experts = nn.ModuleList([Expert(dim) for _ in range(num_experts)])

    def forward(self, x):
        # x: [B, D]，B 是 token 总数（批次×序列长度展平后），D 是特征维度
        B, D = x.shape

        # 步骤 1：路由器计算每个 token 对每个专家的匹配得分
        # router(x): [B, N]（N = num_experts），softmax 归一化为概率分布
        gate_scores = F.softmax(self.router(x), dim=-1)  # [B, N]

        # 步骤 2：每个 token 从 N 个专家中选出得分最高的 k 个
        # topk_scores: [B, k]，选中的专家权重
        # topk_idx:    [B, k]，选中的专家索引
        topk_scores, topk_idx = gate_scores.topk(self.k, dim=-1)

        # 步骤 3：初始化输出张量，逐 Top-K 位置累加各专家的加权输出
        out = torch.zeros_like(x)  # [B, D]

        # 遍历每个 Top-K 选择位置（第 i 个选择）
        for i in range(self.k):
            expert_ids = topk_idx[:, i]       # 每个 token 第 i 个选择的专家编号，[B]
            expert_weight = topk_scores[:, i]  # 对应权重，[B]

            # 当前位置的专家输出累加容器
            expert_output = torch.zeros_like(x)

            # 遍历所有专家，让各自处理被分配到的 token
            for e_id, expert in enumerate(self.experts):
                # 构建掩码：哪些 token 的第 i 个选择是专家 e_id
                # mask: [B, 1]，用于在乘法中过滤不属于该专家的 token
                mask = (expert_ids == e_id).float().unsqueeze(1)

                if mask.sum() == 0:
                    continue  # 该专家本轮没有 token，跳过

                # 将属于该专家的 token（其他 token 被掩码置 0）送入专家网络
                # 注意：x * mask 保持张量形状不变，保留反向传播梯度路径
                expert_output += expert(x * mask)

            # 按该位置的路由权重加权，累加到最终输出
            # expert_weight.unsqueeze(1): [B, 1]，广播乘到 [B, D]
            out += expert_output * expert_weight.unsqueeze(1)

        # out：每个 token 在其 Top-K 专家的加权聚合表征
        # 相比单一 FFN，融合了多个专家各自的语义变换能力
        return out
```

### 5.2.3 EC 模式：专家主动选 Token（Expert Chooses）

**核心思路**：对每一个专家（矩阵的每一列），在 token 维度（行维度）上选 Top-K 个 token。  
**直觉**：像"导师主动挑学生"——每个专家只处理与自己最匹配的 $k$ 个 token。

![图 5.2 专家选择模式（EC）](https://github.com/user-attachments/assets/d665c6bd-88be-4b35-9199-71dbfe74b9ba)

**优势**：每个专家处理 token 数量上限是固定的 $k$，天然实现**负载均衡**，避免"偏科"。

**劣势**：部分 token 可能完全没有被任何专家选中（被"丢弃"），导致语义信息缺失，影响理解能力。

> 实测案例（`EC_MoE(dim=32, num_experts=10, k=2)`，输入两句中文）：  
> 所有专家的负载均为 2 token，但部分 token（如"混"、"合"、"模"、"型"）完全未被处理。

#### EC 简易实现

```python
class EC_MoE(nn.Module):
    def __init__(self, dim, num_experts, k):
        super().__init__()
        self.num_experts = num_experts  # 专家总数
        self.k = k                      # 每个专家最多选 k 个 token
        self.router = nn.Linear(dim, num_experts)  # 路由器
        self.experts = nn.ModuleList([Expert(dim) for _ in range(num_experts)])

    def forward(self, x):
        # x: [B_total, D]，B_total = 批次 × 序列长度
        B_total, D = x.shape

        # 步骤 1：路由器计算所有 token 对所有专家的匹配得分
        # 形状：[B_total, num_experts]
        gate_scores = F.softmax(self.router(x), dim=-1)

        # 步骤 2：EC 视角——转置后变成"每个专家对所有 token 的得分"
        # scores_T[e][t] = 专家 e 对 token t 的评分
        scores_T = gate_scores.transpose(0, 1)  # [num_experts, B_total]

        # 每个专家挑选得分最高的 Top-K 个 token
        # topk_idx:    [num_experts, k]，每个专家选中 token 的索引
        # topk_scores: [num_experts, k]，对应的路由得分
        topk_scores, topk_idx = scores_T.topk(min(self.k, B_total), dim=-1)

        # 步骤 3：构建 dispatch_weights 矩阵
        # dispatch_weights[t][e] = 专家 e 给 token t 分配的权重（未选中则为 0）
        dispatch_weights = x.new_zeros((B_total, self.num_experts))  # [B_total, N]

        for e in range(self.num_experts):
            for t_idx, s in zip(topk_idx[e].tolist(), topk_scores[e].tolist()):
                dispatch_weights[t_idx, e] = s  # 填入专家 e 对 token t 的权重

        # 步骤 4：每个专家只处理其选中的 token，按权重加权后累加到输出
        out = torch.zeros_like(x)
        for e_id, expert in enumerate(self.experts):
            # 该专家是否选中了 token：[B_total, 1]
            mask = (dispatch_weights[:, e_id] > 0).float().unsqueeze(1)

            if mask.sum() == 0:
                continue  # 该专家没有选到任何 token

            # 只处理被选中的 token（未选中的 token 被掩码置 0）
            expert_out = expert(x * mask)

            # 按该专家对各 token 的权重加权，累加到输出
            out += expert_out * dispatch_weights[:, e_id].unsqueeze(1)

        return out
```

### 5.2.4 TC vs EC 对比总结

| 维度 | TC（Token 选专家） | EC（专家选 Token）|
|------|-------------------|------------------|
| 选择方向 | token → Top-K 专家 | 专家 → Top-K token |
| 语义完整性 | ✅ 每个 token 必被处理 | ❌ 部分 token 可能被丢弃 |
| 负载均衡 | ❌ 易产生热门专家扎堆 | ✅ 每专家处理量固定 |
| 常用场景 | 追求语义覆盖 | 追求负载均衡/推理效率 |

**本质**：两种模式代表了稀疏专家系统在**信息完整性 vs 负载均衡**上的典型权衡。

---

## 5.3 负载均衡：为什么难，有哪些解法

### 5.3.1 为什么负载会失衡？（路由崩溃的直觉）

如果没有任何约束，路由器会自然倾向于"走捷径"：总把 token 送到已经训练好（损失更低）的专家，形成**马太效应**：

- 被频繁选中的专家 → 得到更多梯度 → 进一步变强
- 长期被冷落的专家 → 几乎得不到梯度 → 能力停滞

这种极端状态被称为**路由崩溃（Routing Collapse）**，模型退化为只有少数几个"大专家"在工作，MoE 的参数效率优势完全丧失。

### 5.3.2 辅助负载均衡损失（Switch Transformer 经典方案）

**来源**：[Switch Transformers: Scaling to Trillion Parameter Models（2021）](https://arxiv.org/abs/2101.03961)

**idea**：在训练 loss 中加入一个正则项，惩罚"某些专家承接的 token 比例远高于其他专家"这件事。

设共有 $N$ 个专家，一个批次 $\mathcal{B}$ 共含 $T$ 个 token，辅助损失为：

$$
\mathcal{L}_{aux} = \alpha \cdot N \cdot \sum_{i=1}^{N} f_i \cdot P_i
$$

其中：

$$
f_i = \frac{1}{T} \sum_{x \in \mathcal{B}} \mathbf{1}\{\text{argmax}\; p(x) = i\}
\quad \text{（专家 $i$ 实际接到的 token 比例）}
$$

$$
P_i = \frac{1}{T} \sum_{x \in \mathcal{B}} p_i(x)
\quad \text{（路由器分配给专家 $i$ 的平均概率）}
$$

**从直觉理解这个公式**：

- $f_i$ 是"硬分配"的比例（用 argmax 决定每个 token 去哪，是离散的、不可导的）。
- $P_i$ 是路由器输出的软概率（可导的，梯度可以流过它）。
- 两者的乘积 $f_i \cdot P_i$ 在均匀分布下（每个专家接 $1/N$ 的 token）最小：$\sum_{i=1}^N \frac{1}{N} \cdot \frac{1}{N} = \frac{1}{N}$。
- 乘以 $N$ 是为了让损失值不随专家数 $N$ 变化（归一化）。
- 超参 $\alpha$（常用 $10^{-2}$）控制辅助损失的强度：太大会干扰主任务 loss，太小则约束不住路由崩溃。

**为什么 $f_i$ 不可导却还有用？**：$f_i$ 用于"监测"，$P_i$ 用于"反传梯度"。当某个专家 $i$ 的 $f_i$ 很高时，乘积 $f_i \cdot P_i$ 也偏大，产生较大梯度，推动路由器降低给该专家的 $P_i$（减少送过去的概率），从而间接减少 $f_i$。

总训练损失：
$$
\mathcal{L}_{total} = \mathcal{L}_{CE} + \mathcal{L}_{aux}
$$

### 5.3.3 噪声门控（Noisy Gating）

**idea**：在路由 logits 上加入随机噪声，使路由决策"不那么确定"，从而让不同专家有机会在训练早期被探索到。

$$
h(x) = x \cdot W_g + \epsilon, \quad \epsilon \sim \mathcal{N}(0, \sigma^2(x))
$$

其中噪声标准差 $\sigma(x) = \text{Sigmoid}(x \cdot W_{noise})$ 是输入相关的（数据依赖），由一个独立的可学习矩阵 $W_{noise}$ 控制。

**直觉**：在训练初期，路由器还没有充分学习，加入噪声相当于给"不太自信的路由决策"增加随机性，鼓励更多专家被探索，从而减缓早期负载偏斜。

```python
def _noisy_logits(self, x):
    """计算带噪声的路由 logits（仅在训练模式启用）"""
    logits = self.w_gating(x)                     # 基础得分：[N, E]

    if self.noisy_gating and self.training:
        # 用 sigmoid 把 w_noise 的输出限制到 (0, 1) 作为噪声标准差
        # 这是"数据依赖的噪声"：不同 token 的噪声强度不同
        noise_std = torch.sigmoid(self.w_noise(x))

        # 加上对应标准差的正态分布随机噪声
        logits = logits + torch.randn_like(logits) * noise_std

    return logits
```

### 5.3.4 容量限制（Capacity Factor）

**idea**：给每个专家设定一个最大容量，超额的 token 被"丢弃"（直接通过残差传递到下一层，不经过该专家处理）。

$$
C_{expert} = \left\lceil \frac{N_{tokens}}{N_{experts}} \times \text{capacity\_factor} \right\rceil
$$

- $\text{capacity\_factor} > 1.0$：允许一定程度的超载（缓冲），避免过多 token 被丢弃。
- $\text{capacity\_factor} = 1.0$：严格均衡，但可能丢弃较多 token。

**直觉**：强制给每位"专家老师"设定"招生名额"，防止无限接收学生导致质量下降，但也确实存在部分学生（token）得不到处理的风险。

```python
# 计算容量上限
capacity = int((N / self.n_experts) * self.capacity_factor) + 1

for e in range(self.n_experts):
    idx = torch.nonzero(dispatch_mask[:, e], as_tuple=False).squeeze(-1)
    if idx.numel() > capacity:
        idx = idx[:capacity]  # 超出容量的 token 被截断（丢弃）
    expert_inputs.append(x_flat[idx])
```

### 5.3.5 其他负载均衡策略一览

| 策略 | 核心思路 | 特点 |
|------|----------|------|
| 辅助负载均衡损失 | 在 loss 中加正则项，惩罚路由不均 | 经典方法，Switch Transformer 等采用 |
| 容量控制 + overflow 机制 | 设上限，超额 token 走备用路径 | 保证推理时间可预测 |
| 动态无辅助损失均衡 | 基于历史负载统计为专家加 bias 动态调整路由分数 | 不引入额外梯度，最近研究更稳定 |
| 共享专家（DeepSeekMoE） | 部分专家所有 token 都激活，保证覆盖 | 覆盖率 + 专家特长的折中 |
| 哈希路由（非学习） | 固定哈希函数映射 token → 专家 | 天然负载均衡，语义灵活性弱 |

---

## 5.4 哈希路由：非学习式路由的简介

部分研究表明，复杂的可学习路由器并非绝对必要。**哈希路由**通过固定哈希函数将 token 映射到专家，不需要训练路由器，天然具有较好的负载均衡特性。

以 **LSH（局部敏感哈希）** 为例：

$$
h_i(x) = \left\lfloor \frac{a_i^\top x + b_i}{\epsilon} \right\rfloor
$$

其中 $a_i \in \mathbb{R}^d$ 是随机投影向量，$b_i$ 是随机偏置，$\epsilon$ 是桶宽度（控制每桶容量）。

> **符号哈希变体**：实际代码中常见"随机超平面符号哈希"——将投影结果取正负符号，编码为比特串后取模得到专家索引。上式公式与代码实现在细节上属同一 LSH 家族，但具体形式略有不同。

![图 5.3 哈希路由示意图](https://github.com/user-attachments/assets/e5b160fb-1410-418d-aa48-f790095a5f01)

**LSH 的"弱语义"特性**：由于局部敏感性，相似的 token（嵌入向量接近）更可能落入同一哈希桶（即同一专家），这是一种天然的弱聚合效果，但比可学习路由的精细语义区分要差得多。

#### LSH 路由 MoE 实现

```python
class LSHRouter(nn.Module):
    """
    基于随机超平面符号哈希的 LSH 路由器。
    核心原理：将 token 嵌入投影到多个随机方向，
    取投影结果的正负符号（0/1），拼接为比特串，
    再取模映射到专家索引。
    """
    def __init__(self, d_model, num_experts, n_hashes=8):
        super().__init__()
        self.num_experts = num_experts
        self.n_hashes = n_hashes
        # register_buffer：随机投影矩阵不参与梯度更新，但随模型保存
        # shape: [n_hashes, d_model]，每行是一个随机超平面法向量
        self.register_buffer(
            "random_vectors",
            torch.randn(n_hashes, d_model)
        )

    def forward(self, x):
        # x: [B, D]
        # 步骤 1：计算 token 在每个随机方向上的投影值
        projections = x @ self.random_vectors.T  # [B, n_hashes]

        # 步骤 2：取符号（正→1，负→0），生成 n_hashes 位的二进制向量
        signs = (projections > 0).long()          # [B, n_hashes]

        # 步骤 3：把二进制向量转为整数哈希值（类似二进制转十进制）
        # 1 << torch.arange(n_hashes)：[1, 2, 4, 8, ...]（每位的权重）
        hashes = signs @ (1 << torch.arange(self.n_hashes, device=x.device))  # [B]

        # 步骤 4：取模映射到专家索引
        expert_ids = hashes % self.num_experts    # [B]

        return hashes, expert_ids
```

**MoE 路由方式对比**：

| 路由方式 | 是否可学习 | 语义灵活性 | 负载均衡 | 典型用途 |
|---------|-----------|-----------|---------|---------|
| Top-K（TC/EC） | ✅ 是 | ✅ 高 | ❌ 需额外机制 | DeepSeek、Qwen、Switch Transformer |
| 哈希路由 | ❌ 否 | ⚠️ 弱语义 | ✅ 天然均衡 | 轻量 MoE、推理加速实验 |

---

## 5.5 MoE 变体：Switch Transformer 与 DeepSpeedMoE

### 5.5.1 Switch Transformer（Top-1 路由 + 稳定训练技巧）

**论文**：[Switch Transformers: Scaling to Trillion Parameter Models（Google，2021）](https://arxiv.org/abs/2101.03961)

**核心创新**：把 Top-K 简化为 **Top-1**（每个 token 只路由到一个专家），在大幅简化实现、减少通信开销的同时，证明了在 T5 系列上能获得 7x 的训练加速。

![图 5.6 Switch Transformer 结构示意图](https://github.com/user-attachments/assets/33892936-0c5c-4743-8047-6e65d9d85401)

**关键设计**：

1. **Top-1 路由**：每个 token 只选 1 个专家，softmax 概率主要用于统计/辅助损失，实际前向计算只走选中专家。比 Top-K 更简单、通信更少。

2. **容量因子 + 超额丢弃**：若某专家接收 token 超过容量，超出部分直接通过残差层传递到下一层（不执行该专家的 FFN，也不产生梯度）。

3. **Router Z-loss**（稳定训练）：惩罚路由 logits 的幅度，减少极端值：
$$
\mathcal{L}_{z} = \lambda \cdot \log^2 Z, \quad Z = \sum_{i=1}^{N} \exp(z_i)
$$
直觉：不让 softmax 的"分母规模" $Z$ 变得极端，避免低精度（bfloat16）下数值溢出。

4. **较小初始化**：从截断正态分布初始化权重（均值为 0），降低训练初期的梯度方差，提升稳定性。

5. **层顺序：Self-Attention → FFN/MoE**：
   - **先 Self-Attention**：让每个 token 充分融合全局上下文信息，得到包含丰富语境的隐藏表示。
   - **后 MoE**：路由器基于已有丰富上下文的特征分配专家，判断更准确，专家也能做更精细的语义增强。
   - 若顺序颠倒（FFN 先于 Self-Attention），路由器只能看到孤立的 token 嵌入，分配会更随机，专家特化效果变差。

### 5.5.2 DeepSpeedMoE（系统级优化全栈设计）

**核心贡献**：在**模型结构（PR-MoE）、训练系统（灵活并行）、推理加速（分层通信）**上的全栈优化，目标是让 MoE 比稠密模型在训练成本、部署效率上更具优势。

- **PR-MoE（Pyramid-Residual MoE）**：用固定 MLP（通用语义）+ 专家残差（特化纠错）减少参数与通信，并用"金字塔式专家数量"把更多专家集中在深层（更高层次语义的专家化更有价值）。

- **MoS（MoE with Student）**：通过"两阶段蒸馏"压缩 PR-MoE：
  - **阶段 1**（蒸馏稳定期）：用教师模型信号引导学生，稳定早期学习分布。
  - **阶段 2**（自主泛化期）：关闭蒸馏，只优化语言模型损失，让学生具备自主泛化能力。
  - 实践中可将模型尺寸再缩小约 3.7 倍，推理速度比质量相当的稠密模型更快。

- **系统优化**：
  - 自适应并行（专家并行 + 数据并行 + 张量切片的动态组合）
  - 分层 All-to-All（先节点内高速通信，再跨节点通信，降低延迟）
  - All-to-All 复杂度从 $O(p)$ 降到 $O(p/L)$（通过张量切片）

> **分层 All-to-All**：在 MoE 中，原本需要一次全局所有 GPU 间的 token 通信，按硬件拓扑拆成"先节点内 → 再跨节点"的分级通信，显著减少跨机器的高延迟通信量。

---

## 5.6 DeepSeekMoE：迈向极致专家专业化

**论文**：[DeepSeekMoE: Towards Ultimate Expert Specialization（2024）](https://arxiv.org/pdf/2401.06066)

### 5.6.1 传统 MoE 的两个问题

1. **知识混合（Knowledge Hybridity）**：分配给某个专家的 token 可能涵盖多种不同语义类型，导致专家难以在任何一个方向深度专精——像老师被要求同时教数学、历史和美术。

2. **知识重复（Knowledge Redundancy）**：不同专家处理的 token 可能有大量语义重叠，不同专家都在学习相似的通用知识，无法形成真正的"专家分工"。

### 5.6.2 细粒度专家分割（Fine-Grained Expert Segmentation）

**idea**：在保持总参数量和计算量不变的前提下，把每个"大专家"进一步切成 $m$ 个"小专家"，再同时激活更多小专家来保持计算量恒定。

![图 5.4 DeepSeekMoE 结构示意图](https://github.com/user-attachments/assets/6aab083e-c9b6-48a2-9f7d-28d833c7860a)

**数学上**：

- 原来：$N$ 个专家，每次 Top-K 激活 $k$ 个
- 切割后：$mN$ 个小专家（每个参数量是原来的 $1/m$），每次激活 $mk$ 个小专家

**为什么细粒度化有效？（三重直觉）**：

1. **更灵活的专家组合**：可供路由器选择的专家组合数从 $C_N^k$ 爆增到 $C_{mN}^{mk}$，路由器有更大的自由度精准匹配每个 token 的语义需求。

2. **更细致的知识分工**：每个小专家参数量更少，"被迫"聚焦于更窄的语义子空间，更难容纳杂糅知识。

3. **组合空间爆炸式增长的量级示例**：
   - 原始：$N=16$，Top-2 → $C_{16}^2 = 120$ 种组合
   - 细粒度（每个切成 4 份）：$mN=64$，激活 8 个 → $C_{64}^8 \approx 44$ 亿种组合

**代价**：专家数量增多会带来更大的路由通信量和调度复杂度，性能提升存在边际递减，需通过消融实验确定最优粒度。

### 5.6.3 共享专家隔离（Shared Expert Isolation）

**idea**：在路由专家池之外，额外设置 $K_s$ 个**共享专家**，所有 token 都会强制经过这些共享专家（不需要路由），由它们捕获通用知识；路由专家专注于捕获特化知识。

![图 5.5 总参数和激活参数相同的对比实验](https://github.com/user-attachments/assets/d4f713ba-e9c5-4d57-95cd-82a914610828)

**完整 DeepSeekMoE 层的输出**：

$$
y = \sum_{i=1}^{K_s} E_i^{shared}(x) + \sum_{i \in \mathcal{T}} G_i(x) \cdot E_i^{routed}(x)
$$

其中路由专家总数为 $mN - K_s$，非零门数为 $mK - K_s$。

**经验比例**：论文消融实验表明，共享专家与激活的路由专家保持约 **1:3 的比例**时，在基准任务上效果最好。

**为什么共享专家有效**：

- 路由专家不再需要为"所有 token 都会遇到的通用知识"分配参数容量，可以把更多参数用在专化方向。
- 即使路由阶段某个 token 被"冷落"，共享专家也能保证它获得基本的语义处理，减少 token 丢失风险。

### 5.6.4 负载均衡策略

DeepSeekMoE 引入两级辅助损失：

- **专家级平衡损失 $\mathcal{L}_{ExpBal}$**：最小化各专家接收 token 比例的不均匀性（与 Switch Transformer 的辅助损失思路类似）。

- **设备级平衡损失 $\mathcal{L}_{DevBal}$**：当专家分布在多个设备时（如 DeepSeek-V3），确保跨设备计算负载均衡，优化并行效率。

> **DeepSeek-V3 的通信优化**：训练时把激活量用 FP8 格式量化（省带宽），但把专家输出"合并"的关键计算仍用 BF16（保稳定性）。**简洁记忆：传输用低精度（省带宽），关键计算用中等精度（保稳定）**。

---

## 5.7 在 Mini LLM 中嵌入 MoE

### 5.7.1 完整架构回顾

`Mini LLM + MoE = Token Embedding + 位置编码 + Transformer Layers（部分层替换为 MoE）+ 输出投影`

其中关键组件：

- **字节级分词器**（ByteTokenizer）：词表大小 259 = 256 字节 + 3 个特殊 token（`<bos>`, `<eos>`, `<pad>`）。
- **TransformerBlock**：支持在传统 FFN 和 MoE 层之间切换（通过 `use_moe` 参数）。
- **输出层权重共享**：`lm_head.weight = tok_emb.weight`（减少参数量，提升训练稳定性）。

### 5.7.2 带容量限制的 MoE 层完整实现

```python
class MoELayer(nn.Module):
    """
    带噪声门控和容量限制的 MoE 层。
    支持 Top-1 和 Top-2 两种激活策略。
    """
    def __init__(self, d_model, d_ff, n_experts=4, k=1, capacity_factor=1.25, noisy_gating=True):
        super().__init__()
        assert k in (1, 2), "当前简化实现仅支持 Top-1 或 Top-2"
        self.d_model = d_model
        self.n_experts = n_experts
        self.k = k
        self.capacity_factor = capacity_factor
        self.noisy_gating = noisy_gating

        # 门控网络：输入 d_model 维特征，输出 n_experts 维得分
        self.w_gating = nn.Linear(d_model, n_experts, bias=False)

        if noisy_gating:
            # 噪声网络：学习每个 token 的噪声强度（数据依赖的随机性）
            self.w_noise = nn.Linear(d_model, n_experts, bias=False)

        # 专家网络列表：每个专家是独立的 FFN（Linear → GELU → Linear）
        self.experts = nn.ModuleList([
            nn.Sequential(
                nn.Linear(d_model, d_ff),   # 升维
                nn.GELU(),                   # 平滑激活（比 ReLU 更常用于现代 LLM）
                nn.Linear(d_ff, d_model)    # 降维
            ) for _ in range(n_experts)
        ])

    def _noisy_logits(self, x):
        """计算带噪声的路由 logits，训练时才加噪声"""
        logits = self.w_gating(x)   # [N, E]

        if self.noisy_gating and self.training:
            # 噪声标准差由输入决定：Sigmoid 把输出限制到 (0,1)
            noise_std = torch.sigmoid(self.w_noise(x))   # [N, E]
            logits = logits + torch.randn_like(logits) * noise_std

        return logits

    def forward(self, x, mask=None):
        B, T, D = x.shape
        N = B * T                          # 批次中所有 token 的总数
        x_flat = x.view(N, D)              # 展平：[B, T, D] → [N, D]

        # 路由：计算得分并归一化
        logits = self._noisy_logits(x_flat)
        scores = F.softmax(logits, dim=-1)  # [N, E]，每个 token 对各专家的路由概率

        # ——— Top-1 路径 ———
        if self.k == 1:
            top1 = torch.argmax(scores, dim=-1)                 # [N]，每个 token 选中的专家
            # one_hot 编码：[N, E]，标记每个 token 选中哪个专家
            dispatch_mask = F.one_hot(top1, num_classes=self.n_experts).to(x.dtype)
            # 提取选中专家的路由概率作为最终权重
            combine_weights = torch.gather(scores, 1, top1.unsqueeze(1)).squeeze(1)  # [N]
            # 每个专家的容量上限
            capacity = int((N / self.n_experts) * self.capacity_factor) + 1

            expert_inputs, expert_indices = [], []
            for e in range(self.n_experts):
                # 找出应该送给专家 e 的 token 索引
                idx = torch.nonzero(dispatch_mask[:, e], as_tuple=False).squeeze(-1)
                if idx.numel() > capacity:
                    idx = idx[:capacity]    # 超出容量的 token 被丢弃（不经过该专家处理）
                expert_inputs.append(x_flat[idx])
                expert_indices.append(idx)

            out_flat = torch.zeros_like(x_flat)
            for e in range(self.n_experts):
                if expert_inputs[e].size(0) == 0:
                    continue
                y = self.experts[e](expert_inputs[e])
                out_flat[expert_indices[e]] = y   # 把专家输出放回原始序列位置

            # 乘以路由权重（每个 token 乘以其路由概率）
            out_flat = out_flat * combine_weights.unsqueeze(1)
            return out_flat.view(B, T, D)

        # ——— Top-2 路径（简化实现）———
        else:
            topk_vals, topk_idx = torch.topk(scores, k=2, dim=-1)   # [N, 2]
            capacity = int((N / self.n_experts) * self.capacity_factor) + 1
            expert_buckets = [[] for _ in range(self.n_experts)]

            for i in range(N):
                for j in range(2):
                    e = int(topk_idx[i, j].item())
                    w = float(topk_vals[i, j].item())
                    expert_buckets[e].append((i, w))   # 存储 (token 索引，路由权重)

            out_flat = torch.zeros_like(x_flat)
            for e in range(self.n_experts):
                bucket = expert_buckets[e]
                if not bucket:
                    continue
                if len(bucket) > capacity:
                    bucket = bucket[:capacity]   # 超额丢弃

                idxs = torch.tensor([i for i, _ in bucket], device=x.device, dtype=torch.long)
                weights = torch.tensor([w for _, w in bucket], device=x.device, dtype=x.dtype)
                y = self.experts[e](x_flat[idxs])  # 专家处理
                # 加权累加到输出：Top-2 时同一 token 可能来自两个专家，使用 += 叠加
                out_flat[idxs] += y * weights.unsqueeze(1)

            return out_flat.view(B, T, D)
```

### 5.7.3 Transformer Block（支持 FFN/MoE 切换）

```python
class TransformerBlock(nn.Module):
    """
    一个 Transformer Block，支持在传统 FFN 和 MoE 之间切换。
    结构：Pre-LN（更稳定）+ Self-Attention + Pre-LN + FFN/MoE + 残差连接
    """
    def __init__(self, d_model, nhead, d_ff, use_moe=False, moe_params=None, dropout=0.1):
        super().__init__()
        self.attn = SimpleSelfAttention(d_model, nhead)
        self.ln1 = nn.LayerNorm(d_model)    # 注意力层前的 LayerNorm（Pre-LN 范式）
        self.ln2 = nn.LayerNorm(d_model)    # FFN/MoE 层前的 LayerNorm
        self.dropout = nn.Dropout(dropout)
        self.use_moe = use_moe

        if use_moe:
            # 第二子层：MoE 层（稀疏激活）
            self.moe = MoELayer(**moe_params)
        else:
            # 第二子层：传统 FFN（稠密计算）
            self.ffn = nn.Sequential(
                nn.Linear(d_model, d_ff),
                nn.GELU(),
                nn.Linear(d_ff, d_model)
            )

    def forward(self, x, mask=None):
        # Pre-LN + Self-Attention + 残差
        attn_out = self.attn(self.ln1(x), mask=mask)
        x = x + self.dropout(attn_out)

        # Pre-LN + FFN/MoE + 残差
        if self.use_moe:
            moe_out = self.moe(self.ln2(x), mask=mask)
            x = x + self.dropout(moe_out)
        else:
            ffn_out = self.ffn(self.ln2(x))
            x = x + self.dropout(ffn_out)

        return x
```

### 5.7.4 Mini LLM + MoE 完整模型

```python
class MiniMoELLModel(nn.Module):
    """
    mini LLM = Token Embedding + 位置编码 + Transformer Layers + 输出投影
    支持选择性地在某些层使用 MoE（通过 use_moe_layer_index 指定）
    """
    def __init__(self, vocab_size, d_model=256, nhead=4, n_layers=4, d_ff=1024,
                 use_moe_layer_index=None, moe_params=None):
        super().__init__()
        self.vocab_size = vocab_size
        self.d_model = d_model

        # Token 嵌入 + 位置编码（可学习的，最大上下文长度 4096）
        self.tok_emb = nn.Embedding(vocab_size, d_model)
        self.pos_emb = nn.Embedding(4096, d_model)

        # 构建 Transformer 层堆叠
        use_moe_layer_index = set(use_moe_layer_index or [])
        if moe_params is not None:
            moe_params = moe_params.copy()
            moe_params.setdefault("d_model", d_model)
            moe_params.setdefault("d_ff", d_ff)

        self.layers = nn.ModuleList([
            TransformerBlock(
                d_model=d_model, nhead=nhead, d_ff=d_ff,
                use_moe=(i in use_moe_layer_index),   # 按索引决定是否使用 MoE
                moe_params=moe_params
            ) for i in range(n_layers)
        ])

        # 最终 LayerNorm：稳定最后隐藏状态的尺度，让 lm_head 更稳定
        self.ln_f = nn.LayerNorm(d_model)

        # 语言模型头：投影到词表维度（logits）
        self.lm_head = nn.Linear(d_model, vocab_size, bias=False)
        # 权重共享：lm_head 和 tok_emb 共享权重，减少参数量并提升训练效率
        self.lm_head.weight = self.tok_emb.weight

    def forward(self, idx, mask=None):
        B, T = idx.shape
        pos = torch.arange(T, device=idx.device).unsqueeze(0)   # [1, T]

        # 输入嵌入 = Token Embedding + Position Embedding
        x = self.tok_emb(idx) + self.pos_emb(pos)   # [B, T, D]

        for blk in self.layers:
            x = blk(x, mask=mask)

        x = self.ln_f(x)           # 最终归一化：稳定进入 lm_head 的特征尺度
        logits = self.lm_head(x)   # [B, T, vocab_size]

        return logits
```

> **为什么在 lm_head 前加 LayerNorm？**  
> 各 Transformer 层在不同方向上对隐藏表示做了大量非线性变换，最终输出的尺度可能不一致。LayerNorm 把最终隐藏状态规范化到相对统一的尺度，让 lm_head 能更稳定地把特征映射到词表 logits，避免少数维度值过大导致的预测偏差。

---

## 5.8 MoE vs 稠密模型：何时选哪个

### 5.8.1 核心优劣势对比

**MoE 的优势**：
- **参数容量 vs 计算量解耦**：可以在接近稠密模型计算量的前提下放入更多参数，提升模型表示能力。
- **天然支持专家级并行**：各专家 FFN 可分布到不同设备，路由器只负责指派 token，计算在对应设备上独立完成。

![图 5.7 MoE vs 稠密模型收敛对比](https://github.com/user-attachments/assets/b9945e0c-9a88-4127-a267-2f1c0b62d132)

**MoE 的劣势**：
- **路由与负载均衡困难**：专家易出现"冷热不均"，需要额外机制（辅助损失、噪声门控等）。
- **分布式通信复杂**：all-to-all token 交换带来显著的通信代价和工程复杂度。
- **小 batch/低并发推理**：专家利用率低，延迟更抖动。
- **训练不稳定**：路由崩溃风险，以及路由器与专家的共同演化需要精心调试。

**选型建议**：

| 需求 | 推荐方案 |
|------|---------|
| 训练稳定、工程简单、推理延迟可控 | **稠密（Dense）模型** |
| 大规模训练追求更高参数效率（同 FLOPs 更强） | **MoE**（接受更高工程复杂度） |

### 5.8.2 从零训练 vs upcycling（稠密→稀疏升级）

**Upcycling**（稠密→MoE 升级）：复用已训练好的稠密模型权重，改造为 MoE。

**从零训练**：从随机初始化开始训练 MoE，专家与路由器共同演化。

**两路径的追赶点差异**：

| 实验 | 路由策略 | 从零训练追赶 upcycling 需要的计算量 |
|------|---------|-----------------------------------|
| OLMoE | TC | 约原稠密训练量的 **25%** |
| Komatsuzaki 等 | EC | 约原稠密训练量的 **120%** |

**为什么差异如此大？**

1. **路由策略（TC vs EC）**：TC 与 EC 在负载均衡、专家分化速度和早期训练动态上存在本质差异，显著影响从零训练的收敛速度。

2. **upcycling 的两类阻碍因素**（OLMoE 实验指出）：
   - **遗忘 + 重学困难**：旧权重编码了较强通用能力，MoE 化后需要"遗忘"部分表征以重塑专家专业化能力，但历史分布干扰新学习信号。
   - **路由器学得太晚**：路由器随机初始化，早期近似随机分配；等路由器稳定时，学习率往往已衰减，专家分工仍不清晰。

> **成功案例**：Qwen1.5-MoE 通过 upcycling 从已有稠密模型改造为 MoE，在激活参数更少的条件下匹配更大稠密模型的表现，说明路线本身可行，关键在于实施细节。

---

## 5.9 超大规模 MoE 的工程挑战与优化

### 5.9.1 主要挑战

超大规模 MoE（以 Kimi-K2 等约 1T 参数量级为例）面临的特有挑战：

- **路由不均衡**：部分专家过度调用，部分闲置。
- **通信压力**：不同 GPU/节点间 token 路由交换频繁。
- **并行布局复杂**：tensor/pipeline/专家/序列并行的组合优化空间巨大。
- **训练推理不一致**：可能导致专家重要性比例突然失衡。

### 5.9.2 工程优化方法

1. **混合并行设计**：合理安排 tensor 并行、流水线并行、专家并行的组合，减少通信开销。

2. **截断重要性采样修正（Clipped IS）**：防止少数专家在强化学习训练阶段过载（来自 Kimi-K2 的经验）。

3. **自适应并行调度器**：基于实时指标（GPU 利用率、内存占用、步长时间）自动调整并行策略。

4. **LoRA + 强化学习微调**：在 MoE dense 层和专家层上加低秩适配器（LoRA），只更新少量参数，使 RL 微调 GPU 成本降低至全参数的约 10%。

> **重要结论**：大基座 + 小规模 LoRA 的 RL 明显优于小模型的全参数 RL，因为 **RL 的效果受限于基座模型的先验能力**——更强的 base model 产生更高质量的训练轨迹，显著提升 RL 学习效率。

---

## 5.10 MoE 与其他领域的联系

### 5.10.1 传统特征分工 vs MoE 动态稀疏分工

| 维度 | 传统卷积核 | MoE 专家 |
|------|-----------|---------|
| 分工方式 | 固定权重，隐式且固定 | 动态路由，条件激活 |
| 并行性 | 弱并行（所有核同时计算） | 稀疏并行（只激活少数专家） |
| 自适应性 | ❌ 不随输入动态改变 | ✅ 随输入 token 动态选择专家 |

### 5.10.2 MoE 的应用范围

MoE 不局限于 Transformer——作为**条件计算框架**，它被广泛应用于：

- **CNN**：动态卷积，提升视觉建模多样性
- **语音识别**：不同专家专注于不同音素或噪声条件
- **推荐系统**：解决多任务排序
- **强化学习**：多策略、多技能专家分解
- **多模态模型**：跨模态专家协作

---

## 5.11 核心概念复盘

- **MoE 解耦了什么**：参数容量 $\neq$ 每 token 计算量；$N$ 个专家但每步只激活 $k$ 个，总参数大但 FLOPs 接近稠密 $k/N$ 量级的模型。
- **路由机制的本质**：一个可学习的"分类器"，决定每个 token 该走哪条"FFN 通路"；路由决策的好坏直接影响专家分工质量。
- **负载均衡为何困难**：梯度只流向被选中的专家，天然存在马太效应；辅助损失、噪声门控、容量限制是三大主流解法。
- **DeepSeekMoE 的核心 delta**：细粒度专家（更多更小的专家 → 更灵活的组合）+ 共享专家（通用知识隔离，避免冗余）。
- **MoE 工程坑**：路由崩溃、通信开销、小 batch 推理效率差、训练不稳定。

---

## 小型自测

1. **你能说清 MoE 如何把"参数容量"和"每 token 计算量"解耦吗？**（提示：稀疏激活 + 总参数 vs 激活参数）

2. **TC 和 EC 各自的核心优劣是什么？为什么不能两全？**（提示：每 token 必处理 vs 每专家固定负载）

3. **Switch Transformer 的辅助损失为什么乘以 $N$，为什么需要 $f_i \cdot P_i$ 而不只用 $f_i$？**  
   （提示：归一化；$f_i$ 不可导，梯度只能通过 $P_i$ 传递）

4. **DeepSeekMoE 细粒度专家分割解决了什么问题，代价是什么？**  
   （提示：知识混合/重复 → 更小专家 + 更多激活；代价是通信量和调度复杂度）

5. **upcycling 为什么有时比从零训练 MoE 更慢才能赶上？**  
   （提示：旧权重的遗忘 - 重学困难 + 路由器学得太晚）

---

## 参考文献与延伸阅读

- [Switch Transformers: Scaling to Trillion Parameter Models（2021）](https://arxiv.org/abs/2101.03961)
- [DeepSeekMoE: Towards Ultimate Expert Specialization（2024）](https://arxiv.org/pdf/2401.06066)
- [DeepSeek-V3 技术报告](https://arxiv.org/pdf/2412.19437)
- [减少计算消耗的万亿参数 MoE 调优（Kimi-K2）](https://macaron.im/mindlab/research/building-trillion-parameter-reasoning-rl-with-10-gpus)
- [Loss-Free Balancing for MoE（2024）](https://www.emergentmind.com/papers/2408.15664)
- [YaRN: Efficient Context Window Extension（用于上下文外推）](https://arxiv.org/abs/2309.00071)
- Top-K TC 完整可运行代码：[Top-K TC.py](https://github.com/1iyouzhen/CS336-Chinese-co-construction/blob/main/docs/chapter5/Top-K%20TC.py)
- Top-K EC 完整可运行代码：[Top-K EC.py](https://github.com/1iyouzhen/CS336-Chinese-co-construction/blob/main/docs/chapter5/Top-K%20EC.py)
- Mini LLM + MoE 完整代码：[Mini LLM+MoE.py](https://github.com/1iyouzhen/CS336-Chinese-co-construction/blob/main/docs/chapter5/Mini%20LLM%2BMoE.py)
