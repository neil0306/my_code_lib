# 语言模型（LLM）架构与训练：技术细节整理（Chapter 4）

本笔记整理自 datawhale 的 `diy-llm/docs/chapter4/chapter4_第四章语言模型架构和训练的技术细节.md`，并参考 `pytorch_basic_skills.md` 的行文风格进行重构：**先抓主线直觉，再落到公式/形状/工程取舍**，最后用“提问/误区”强化记忆。

----------

## 本章应学会什么

- **Transformer 基本块**：位置编码、注意力、残差+归一化、FFN（以及这些模块最常见的现代改动）。
- **现代主流变体**：Pre-LN、RMSNorm、无 bias、GLU 家族（SwiGLU/GeGLU）、RoPE，以及推理侧 KV cache 与 MQA/GQA。
- **架构与训练的“经验法则”**：$(d_{ff})$ 与 $d_{model}$ 的比例、头数与头维度的常见约束、宽深比、词表规模、dropout/weight decay 的真实作用。
- **稳定性技巧**：输出层 softmax 的 z-loss；注意力 softmax 的稳定化（Q/K 归一化、logit 软截断）。

----------

## 4.1 标准 Transformer 快速回顾（只保留与后文相关的骨架）

Transformer Block（以解码器为主线）通常由这几块组成：

- **Token Embedding + 位置编码/位置机制**
- **(自注意力) Multi-Head Attention**
- **Residual + Norm**
- **FFN/MLP（含激活/门控）**

> 现代 LLM 多数是“decoder-only”（GPT/LLaMA/Qwen 等）：反复堆叠 Decoder Block，并带因果掩码（只能看见左侧历史）。

----------

### 4.1.1 位置编码：正余弦（Sin/Cos）绝对位置编码

原始 Transformer 的固定位置编码：

$$
\begin{aligned}
PE_{(pos,2i)} &= \sin\left(\frac{pos}{10000^{2i/d_{\text{model}}}}\right) \\
PE_{(pos,2i+1)} &= \cos\left(\frac{pos}{10000^{2i/d_{\text{model}}}}\right)
\end{aligned}
$$

- **$pos$**：token 在序列中的位置（0, 1, ...）
- **$i$**：维度索引（成对维度）
- **$d_{model}$**：embedding/hidden 维度
- **直觉**：不同维度对应不同频率；低维“转得快”负责近邻区分，高维“转得慢”负责长距离趋势。
- **用法**：$X = \text{TokenEmb} + PE(pos)$

#### 提问：为什么需要位置编码？

**答**：纯注意力对输入是“集合式”的（缺少顺序 inductive bias）。不显式注入位置信息时，模型难区分“我爱你/你爱我”这种顺序差异。

----------

### 4.1.2 多头注意力（MHA）：从形状到实现

缩放点积注意力（单头）：

$$
\text{Attention}(Q,K,V)=\text{softmax}\left(\frac{QK^\mathsf{T}}{\sqrt{d_k}}\right)V
$$

多头的关键是：把表示空间拆成 $h$ 个子空间并行做注意力，再拼回去。

- **典型形状**（不写 batch 也成立，只是多一维）：
  - 输入 $X$：`[B, S, d_model]`
  - 线性投影得到 $Q,K,V$：`[B, S, d_model]`
  - reshape 分头：`[B, h, S, d_head]`，其中 $d_{head}=d_{model}/h$

#### 形状理解补充：为什么“抽出一个头”后会少一维？

在并行实现里，注意力打分（scores）通常一次性算出所有头：

- `scores.shape == [B, h, S, S]`

其中第 2 维 `h` 表示 head 维度。当我们说“第 $i$ 个头的分数矩阵”，本质就是在 head 维上取切片：

- `scores_i = scores[:, i, :, :]`

这会把 head 这一维固定为某个索引 $i$，因此该维度**被消掉**，得到：

- `scores_i.shape == [B, S, S]`

> 若你希望保留 head 这维做广播/对齐，也可以写成 `scores[:, i:i+1, :, :]`，其形状会是 `[B, 1, S, S]`。两者信息等价，只是是否保留了长度为 1 的维度。

同理，单头输出（Head）也一样：

- 并行输出：`out.shape == [B, h, S, d_head]`
- 取某头：`out_i = out[:, i, :, :]`，得到 `out_i.shape == [B, S, d_head]`

#### 形状理解补充：为什么每个头的输出是 `[B, S, d_head]`（很多资料写成 `[B,S,d_k]`）？

这里的 $d_k$ 在很多文章里指的是**单个头的维度**（即本笔记里的 $d_{head}$），不是整个模型维度 $d_{model}$。

对单个头 $i$，典型形状是：

- $Q_i \in \mathbb{R}^{[B,S,d_{head}]}$、$K_i \in \mathbb{R}^{[B,S,d_{head}]}$、$V_i \in \mathbb{R}^{[B,S,d_{head}]}$

于是：

1) $Q_iK_i^\top:\ [B,S,d_{head}] \times [B,d_{head},S] \rightarrow [B,S,S]$  
2) `softmax` 不改变形状：仍是 `[B,S,S]`  
3) 再乘 $V_i$：$[B,S,S]\times[B,S,d_{head}]\rightarrow[B,S,d_{head}]$

所以 $\text{Head}_i$ 输出自然是 **`[B, S, d_head]`**。

#### 直觉纠偏：“多头合并相当于高秩矩阵分解”到底是什么意思？

你经常会看到一句话：**多头合并≈高秩矩阵分解**。更准确地说，这是一个“矩阵直觉”，不是严格等价定理，主要想表达两点：

1) **从输出投影看：最终输出是多个‘低维通道’的加和**  
合并多头后通常有：

$$
Y=\text{Concat}(\text{Head}_1,\dots,\text{Head}_h)W^O
$$

把 $W^O\in\mathbb{R}^{(h d_{head})\times d_{model}}$ 沿 head 切块：

$$
W^O=\begin{bmatrix}W^O_1\\ \vdots\\ W^O_h\end{bmatrix},\quad W^O_i\in\mathbb{R}^{d_{head}\times d_{model}}
$$

则：

$$
Y=\sum_{i=1}^{h}\text{Head}_i\,W^O_i
$$

这表示输出可以看成 **h 个子空间（每个维度只有 $d_{head}$）贡献的线性组合**——像是把一个复杂映射拆成多条“低维支路”再混合。

2) **从打分矩阵看：每个头的 $Q_iK_i^\top$ 天然是“低秩上限”的模式，多个头叠加更像在拼装更复杂结构**  
对单头 $i$，令 $Q_i\in\mathbb{R}^{S\times d_{head}}$、$K_i\in\mathbb{R}^{S\times d_{head}}$，则：

$$
S_i = Q_iK_i^\top \in \mathbb{R}^{S\times S},\quad \text{rank}(S_i)\le d_{head}
$$

也就是说单头打分矩阵的秩最多是 $d_{head}$（上限受头维度限制）。多头做的是在**多个不同投影子空间**里各自产生这样的“受限秩模式”，再通过后续的拼接+输出投影混合到一起。  
因此直观上可以理解为：**用多组（各自上限较小的）交互模式去组合出更复杂的整体交互**，这就是“像高秩结构分解/拼装”的含义。

下面是一段“形状正确、注释细”的 PyTorch 伪实现（便于复习张量维度流动）。

```python
import math
import torch
import torch.nn.functional as F

def mha_attention(
    x: torch.Tensor,            # [B, S, d_model]
    wq: torch.Tensor,           # [d_model, d_model]
    wk: torch.Tensor,           # [d_model, d_model]
    wv: torch.Tensor,           # [d_model, d_model]
    wo: torch.Tensor,           # [d_model, d_model]
    num_heads: int,
    attn_mask: torch.Tensor | None = None,  # 可选：[1 or B, 1 or h, S, S]，mask 为 True/1 表示“要屏蔽”
) -> torch.Tensor:
    """
    一个教学用的多头注意力实现：
    - 重点是把每一步的张量形状与数学公式对应起来
    - 不涉及 KV cache、FlashAttention、dropout 等工程优化
    """
    B, S, d_model = x.shape
    assert d_model % num_heads == 0, "d_model 必须能被 num_heads 整除，否则无法均分每个头的维度"
    d_head = d_model // num_heads

    # 1) 线性投影：把输入映射到 Q/K/V
    #    这里用矩阵乘法表达：x @ W 需要 x 最后一维与 W 第一维对齐
    q = x @ wq  # [B, S, d_model]
    k = x @ wk  # [B, S, d_model]
    v = x @ wv  # [B, S, d_model]

    # 2) 分头：把 d_model 拆成 [h, d_head]，并把 head 维提前，方便并行计算注意力
    #    reshape 不改变元素总数；transpose 改变维度顺序（注意 contiguous 细节）
    q = q.reshape(B, S, num_heads, d_head).transpose(1, 2)  # [B, h, S, d_head]
    k = k.reshape(B, S, num_heads, d_head).transpose(1, 2)  # [B, h, S, d_head]
    v = v.reshape(B, S, num_heads, d_head).transpose(1, 2)  # [B, h, S, d_head]

    # 3) 注意力打分：对每个头做 QK^T
    #    - q: [B, h, S, d_head]
    #    - k: [B, h, S, d_head] -> 转置最后两维后变成 [B, h, d_head, S]
    #    - scores: [B, h, S, S]，表示“每个 query 位置对所有 key 位置”的相似度
    scores = q @ k.transpose(-2, -1)  # [B, h, S, S]

    # 4) 缩放：除以 sqrt(d_head)，避免 d_head 大时点积方差随维度增长，softmax 过饱和
    scores = scores / math.sqrt(d_head)

    # 5) 掩码（因果/填充）：把不允许关注的位置打成 -inf，使 softmax 后概率为 0
    if attn_mask is not None:
        # 约定：attn_mask 为 True/1 的位置表示“禁止关注”
        scores = scores.masked_fill(attn_mask, float("-inf"))

    # 6) softmax 得到注意力权重，再对 V 做加权求和
    attn = F.softmax(scores, dim=-1)  # [B, h, S, S]
    out = attn @ v                    # [B, h, S, d_head]

    # 7) 合并多头：把 head 维挪回去，再把 [h, d_head] 合成 d_model
    out = out.transpose(1, 2).reshape(B, S, d_model)  # [B, S, d_model]

    # 8) 输出投影：与残差分支同维度，便于相加
    out = out @ wo  # [B, S, d_model]
    return out
```

#### 提问：为什么要除以 $\sqrt{d_k}$？

**答（方差视角）**：点积 $Q\cdot K=\sum_{i=1}^{d_k} q_i k_i$。若每一维近似独立同分布，点积的方差会随 $d_k$ 增大而增大；进入 softmax 后容易变得“极端尖锐”，梯度变小、训练不稳定。除以 $\sqrt{d_k}$ 近似把尺度拉回可控范围。

----------

### 4.1.3 残差连接 + 归一化（LayerNorm）

原始论文（Post-LN，后归一化）：

$$
X \leftarrow \text{LayerNorm}(X + \text{Sublayer}(X))
$$

LayerNorm（按特征维归一化）：

$$
\text{LayerNorm}(v)=\gamma\frac{v-\mu}{\sigma}+\beta
$$

- **残差的直觉**：提供“恒等路径”，让深层网络至少不会比浅层更差，同时让梯度更容易传播。
- **归一化的直觉**：让每层的激活分布更稳定，减少梯度爆炸/消失与训练抖动。

----------

### 4.1.4 FFN / MLP：原始形式与现代默认

原始 Transformer（带 bias + ReLU）：

$$
\text{FFN}(x)=\max(0, xW_1+b_1)W_2+b_2
$$

现代主流（常见做法：**去掉 bias**，并替换激活为 GeLU / GLU 家族）：

$$
\text{FFN}(x)=\text{Act}(xW_1)W_2
$$

- **经验现象**：在大模型训练里，“去掉 bias”往往更稳定、也更省一点带宽；原因并没有完全统一的理论解释，但工程上很常见。

----------

## 4.2 现代 Transformer 变体（你会在主流 LLM 里见到的那些改动）

这一节的思路：**哪些改动几乎已经“标准化”**，以及它们解决的主要矛盾是什么（稳定性/吞吐/长上下文/推理显存）。

----------

### 4.2.1 归一化：Post-LN → Pre-LN → RMSNorm（最常见）

#### 1) Pre-LN（现代主流）

$$
X \leftarrow X + \text{Sublayer}(\text{LayerNorm}(X))
$$

- **直觉**：把归一化放在子层输入处，让残差分支更“干净”，深层时梯度更稳；通常更不依赖精细的 warmup 才能训起来。

##### 为什么 Pre-Norm 往往比 Post-Norm 更好？（直觉 + 数学）

Pre-Norm 与 Post-Norm 的差异只有一个：**LayerNorm 放在残差相加的哪里**，但这会显著改变“梯度有没有一条真正的直通高速路”。

**直觉：Pre-Norm 保留一条“纯残差高速路”，Post-Norm 会在每层出口强行过一次 LN**

- **Post-Norm（原始 Transformer）**：

$$
y=\text{LN}(x + F(x))
$$

直觉上：残差分支虽然把 $x$ 直接加到了 $x+F(x)$，但下一步输出还要经过 LN。也就是说**每一层的输出都会被 LN 重新缩放/投影**，深层时更容易出现训练不稳（梯度尖峰、对 warmup 更敏感）。

- **Pre-Norm（现代主流）**：

$$
y=x + F(\text{LN}(x))
$$

直觉上：LN 只在“变换支路”里，残差分支是**纯加法直通**。哪怕 $F(\cdot)$ 一开始学得很差，模型仍能保持 $y\approx x$，并且梯度能沿着恒等路径更稳定地下传，因此更容易训练很深的网络。

**数学：看反向传播的 Jacobian 里是否“总有一个 $I$ 的直通项”**

把子层记作 $F(\cdot)$（可以是 Attention 或 MLP）。

- **Pre-Norm**：$y=x+F(\text{LN}(x))$

$$
\frac{\partial y}{\partial x}
=
I
\;+\;
\underbrace{
\frac{\partial F}{\partial \text{LN}(x)}\cdot \frac{\partial \text{LN}(x)}{\partial x}
}_{\text{变换支路}}
$$

关键是前面的 **$I$**：无论变换支路的梯度多“糟”，都至少还有一条恒等梯度路径，堆很多层时不会只靠连乘一堆不稳定的 Jacobian。

- **Post-Norm**：$y=\text{LN}(x+F(x))$，令 $u=x+F(x)$

$$
\frac{\partial y}{\partial x}
=
\underbrace{\frac{\partial \text{LN}(u)}{\partial u}}_{\text{每层都要乘的 LN Jacobian}}
\cdot
\left(I+\frac{\partial F}{\partial x}\right)
$$

这里的残差直通 $I$ 也必须先经过 $\frac{\partial \text{LN}}{\partial u}$，即**梯度每层都要被 LN 的 Jacobian“加工一次”**。由于该 Jacobian 依赖当前激活的均值/方差与相关项，层层相乘更容易导致深层训练不稳，因此工程上常更依赖 warmup、clip 等技巧。

#### 2) RMSNorm（更省、更常用）

RMSNorm 去掉了“减均值”项，只保留按均方根缩放：

$$
\text{RMSNorm}(v)=\gamma\frac{v}{\sqrt{\frac{1}{d}\sum_{i=1}^{d} v_i^2+\varepsilon}}
$$

- **为什么流行**：更少运算、更少参数（无 $\beta$），效果常与 LayerNorm 相当。

#### 3) 如何理解“归一化让训练更稳定”？

下面从**直觉**和**数学**两条线，把“稳定”具体落到：**激活尺度、梯度尺度、softmax 数值范围**这三件事上。

**直觉：归一化在驯服“尺度漂移”，让优化别追着移动靶跑**

- **激活分布漂移（distribution shift）**：如果每层输出的均值/方差不断变，下一层看到的输入分布就一直在变，等价于你在优化过程中每一步都在追一个“移动靶”。这会让训练更抖、更依赖小心的超参。
- **梯度失控（爆炸/消失）**：深层网络里，激活过大往往会把反向梯度也放大，激活过小又会把梯度压扁，最终出现梯度范数尖峰、NaN 或者几乎不学习。
- **注意力 softmax 尤其敏感**：softmax 对 logits 的“尺度”很敏感。logits 太大时输出接近 one-hot，很多位置梯度会变得极端（数值不稳或有效梯度很少）。

归一化的效果可以概括为：**把每个 token 的隐藏向量拉回到相对可控的尺度范围**，让后续层持续工作在更稳定的输入分布上，从而训练更稳。

**数学：归一化把尺度变化变成“负反馈”，同时稳定注意力 logits**

以 LayerNorm 为例，对每个 token 的向量 $x\in\mathbb{R}^d$（在特征维上）：

$$
\mu=\frac{1}{d}\sum_{j=1}^d x_j,\quad
\sigma=\sqrt{\frac{1}{d}\sum_{j=1}^d (x_j-\mu)^2+\varepsilon}
$$

$$
\text{LN}(x)=\gamma\odot \frac{x-\mu}{\sigma}+\beta
$$

关键点：

- **前向的尺度钳制**：无论 $x$ 的整体幅度多大，都会除以同一个 $\sigma$，使得归一化后的向量尺度更可控。
- **反向的负反馈（直观版）**：精确的 Jacobian 形式较复杂，但“尺度被 $\sigma$ 控制”这件事依旧成立。你可以把它记成一个非常有用的直觉：梯度里会出现与 $1/\sigma$ 同量级的缩放因子  
  - 若 $\sigma$ 很大（激活很大），梯度被缩小  
  - 若 $\sigma$ 很小（激活很小），梯度被放大一些  
  这相当于对“尺度爆炸/塌缩”施加**负反馈**，训练会更稳。

RMSNorm 也是同样精神：虽然不减均值，但依然用 RMS（均方根）来控制尺度：

$$
\text{RMSNorm}(x)=\gamma\odot \frac{x}{\sqrt{\frac{1}{d}\sum_{j=1}^d x_j^2+\varepsilon}}
$$

**与注意力 softmax 的关系（为什么 Q/K 归一化能稳）**

注意力 logits：

$$
\text{logits}=\frac{QK^\top}{\sqrt{d_k}}
$$

如果 $Q,K$ 的尺度变大，点积的幅度与方差也会随之变大，从而把 softmax 推向饱和区。对 $Q,K$ 先做归一化（例如 LN）是在点积之前先把尺度钉住，让 logits 的范围更可控，从而 softmax 更不容易数值失控。

----------

### 4.2.2 激活函数与门控：ReLU/GeLU → GLU 家族（SwiGLU/GeGLU）

#### 1) GeLU（GPT-1/2/3 等常用）

$$
\text{GeLU}(x)=x\cdot\Phi(x)
$$

- **特点**：更平滑（相对 ReLU），但精确实现更复杂；工程上常用近似多项式/近似 erf。

#### 2) GLU 与变体（近年主流）

GLU（门控线性单元）核心形式：

$$
\text{GLU}(x)=(xW)\odot\sigma(xV)
$$

两个更常见的门控变体：

- **GeGLU**：
$$
\text{GeGLU}(x)=(xW)\odot\text{GELU}(xV)
$$
- **SwiGLU**（很多 LLaMA-like 系列使用）：
$$
\text{SwiGLU}(x)=\text{Swish}(xW)\odot(xV),\quad \text{Swish}(u)=u\cdot\sigma(u)
$$

- **直觉**：把“是否放行信息”变成输入相关的门控，而不是固定形状的单一激活；表达更强但也稍增加计算与参数组织复杂度。

#### 3) 什么是“门控（Gating）”激活？它在控制什么，为什么有效？

门控激活的核心思想是：**把一层的输出拆成两条“通道”——一条生成内容（content），一条生成门（gate），然后逐元素相乘决定每个维度“放行多少”**。

以最经典的 GLU 为例：

$$
\text{GLU}(x)=(xW)\odot \sigma(xV)
$$

- **content 分支**：$xW$ 负责生成“要传递/变换的内容”
- **gate 分支**：$\sigma(xV)$ 产生 $(0,1)$ 之间的门值（逐维度的开关强度）
- **逐元素门控**：$\odot$ 表示逐元素相乘：每个维度都可以被“缩小/关闭/部分通过”

所以“它控制了什么？”——控制的是：**每个 token、每个隐藏维度上的信息通过量**（逐元素、输入相关的动态调节）。

**为什么叫门控？** 因为 gate 分支输出的数值就像阀门开度：0 表示关、1 表示全开、中间值表示半开；而且这个开度是由输入 $x$ 决定的（动态门）。

**作用与优势（直白版）**

- **更强的表达能力**：相比 ReLU/GeLU 这种“对同一个分支做非线性”，门控相当于让网络学到“哪些特征该放大、哪些该抑制”，有点像为每个 token 做一次特征选择/调制。
- **更细粒度的非线性**：不是整层一起激活/抑制，而是逐维度控制，能更精细地表达“组合特征”。
- **经验上更稳更好（尤其在大模型里）**：大量 LLaMA-like 模型选择 SwiGLU/GeGLU 等门控 FFN，通常在同等规模下能带来更好的损失/性能（但也不是绝对必要条件）。

**为什么门控变体（GeGLU/SwiGLU）常优于原始 GLU？**

- 原始 GLU 用 sigmoid 做 gate，容易在极端输入下饱和（门值接近 0 或 1）。
- GeGLU/SwiGLU 用 GELU/Swish 这类更平滑的函数做门控/调制，常在训练动力学与效果上更占优（经验规律）。

----------

### 4.2.3 位置机制：RoPE（旋转位置编码）成为事实标准

你可以把 RoPE 记成一句话：

- **不是把位置向量加到 embedding 上**，而是**在注意力层里对 $Q/K$ 做“按维度成对的旋转”**，使得内积天然带上相对位置信息。

#### 4.2.3.1 RoPE 的“旋转”到底是什么意思？（二维直觉）

先用二维向量把“旋转”讲直白：把一个向量 $v=(x,y)$ 看成平面上的一个点（或箭头）。把它逆时针旋转 $\theta$ 度，本质是做一次线性变换：

$$
R(\theta)=\begin{bmatrix}
\cos\theta&-\sin\theta\\
\sin\theta&\cos\theta
\end{bmatrix}
$$

$$
R(\theta)\,v=
\begin{bmatrix}
\cos\theta&-\sin\theta\\
\sin\theta&\cos\theta
\end{bmatrix}
\begin{bmatrix}
x\\y
\end{bmatrix}
$$

也可以用复数理解：把 $(x,y)$ 写成 $z=x+iy$，旋转 $\theta$ 就等价于乘以 $e^{i\theta}$。这两种说法是同一个东西。

**旋转矩阵几个关键性质（后面会用到）：**

- **正交性**：$R(\theta)^{-1}=R(\theta)^\top$（旋转可逆，转置就是逆）
- **不改长度**：$\|R(\theta)v\|=\|v\|$
- **角度可加**：$R(a+b)=R(a)R(b)$

#### 4.2.3.2 为什么“旋转”会带来相对位置？（$R(m-n)$ 从哪来）

RoPE 的核心把戏是：对不同位置的 token，用不同角度去“旋转”它们的 $Q$、$K$（注意：通常只对 $Q/K$ 做，不对 $V$ 做）。

设位置 $m$ 的 query（或它的某个二维子块）旋转后为：

$$
Q_m' = R(m)\,Q_m
$$

位置 $n$ 的 key 旋转后为：

$$
K_n' = R(n)\,K_n
$$

注意力打分本质是点积（忽略缩放与 mask）：

$$
{Q_m'}\cdot {K_n'} = (R(m)Q_m)\cdot(R(n)K_n)
$$

把点积写成矩阵形式（利用 $R(\theta)^\top=R(\theta)^{-1}=R(-\theta)$）：

$$
\begin{aligned}
{Q_m'}^\top K_n'
&= Q_m^\top R(m)^\top R(n) K_n\\
&= Q_m^\top R(-m) R(n) K_n\\
&= Q_m^\top R(n-m) K_n
\end{aligned}
$$

也就是说打分里出现了 $R(n-m)$（等价写成 $R(m-n)$ 只是记号习惯不同），**只依赖两者的相对位移**。这就是原文里说的“出现 $R(m-n)$，代表相对位置信息”的本质来源。

#### 4.2.3.3 高维向量怎么旋转？（分成很多个二维子空间）

真实模型的 head 维度 $d$ 往往是 64/128/…，不是 2。RoPE 的做法是把高维向量拆成很多对维度：

- 把 $d$ 维向量按 $(0,1),(2,3),\dots$ 成对分组
- 每一对维度当成一个二维平面，在各自平面里独立旋转

因此整体的旋转矩阵是一个“很多个 $2\times2$ 旋转块拼起来”的块对角矩阵（原文写成 $R(m\theta)$）：

$$
R(m\theta)=\begin{bmatrix}
\cos(m\theta_0)&\sin(m\theta_0)&0&0&\cdots&0\\
-\sin(m\theta_0)&\cos(m\theta_0)&0&0&\cdots&0\\
0&0&\cos(m\theta_1)&\sin(m\theta_1)&\cdots&0\\
0&0&-\sin(m\theta_1)&\cos(m\theta_1)&\cdots&0\\
\vdots&\vdots&\vdots&\vdots&\ddots&\vdots\\
0&0&0&0&\cdots&\cos(m\theta_{\frac{d}{2}-1})\\
0&0&0&0&\cdots&-\sin(m\theta_{\frac{d}{2}-1})
\end{bmatrix}
$$

其中频率（角速度）按类似 sin/cos 位置编码的思路分布：

$$
\theta_i = 10000^{-2i/d}
$$

直觉同样是“多尺度”：有的维度对转得快（更敏感于近距离），有的维度对转得慢（覆盖更长距离）。

#### 4.2.3.4 实现视角：对每一对维度做一次 2D 旋转（更像几行代码）

实际工程里通常不会显式构造上面的块对角大矩阵，而是预先算好每个位置的 $\cos,\sin$，然后对 $Q/K$ 做逐元素组合：

```python
def rope_rotate(x, cos, sin):
    """
    x:  [..., d]，d 必须为偶数
    cos/sin: [..., d/2] 或可广播到对应形状（每个位置、每个维度对一组 cos/sin）
    """
    x_even = x[..., 0::2]            # 取偶数维：[..., d/2]
    x_odd  = x[..., 1::2]            # 取奇数维：[..., d/2]
    # 对每一对 (x_even, x_odd) 做二维旋转
    y_even = x_even * cos - x_odd * sin
    y_odd  = x_even * sin + x_odd * cos
    # 再交错拼回 [..., d]
    y = torch.empty_like(x)
    y[..., 0::2] = y_even
    y[..., 1::2] = y_odd
    return y
```

你可以把它理解为：**RoPE 不是给 token “加一个位置向量”，而是把 $Q/K$ 的坐标系随位置转了一点点**；不同 token 的坐标系转的角度不同，于是点积自然就携带了相对位移信息。

#### 4.2.3.5 为什么大家都喜欢 RoPE？（对齐原文结论）

- **显式相对位置**：注意力打分更直接地依赖相对距离（从上面的 $R(n-m)$ 可以看到）。
- **无参、高效**：不引入额外可训练参数，计算只是一点点逐元素三角函数与乘加（通常还能缓存 cos/sin）。
- **外推友好**：由于是正交旋转（不改长度）且位置以角度形式进入，配合一系列工程技巧更容易扩展到更长上下文。

#### 4.2.3.6 常见疑问：成对旋转、维度奇偶、以及“转得快/转得慢”

**Q1：如果 head_dim（或被旋转的维度）不是偶数怎么办？**

- RoPE 的“成对旋转”要求**被旋转的那部分维度是偶数**（才能组成 $(0,1),(2,3),\dots$ 的二维平面）。
- 工程里常见做法是：
  - **直接保证 `head_dim`/`rotary_dim` 为偶数**（大模型配置几乎都会这么设计，例如 64/128）。
  - **只对前 `rotary_dim` 做 RoPE，其余维度不旋转**（partial RoPE）。只要 `rotary_dim` 是偶数即可，剩余维度原样拼回去。
  - 很少会用“把最后 1 维丢掉/补 0 再旋转”，因为会引入不必要的边界怪异。

所以结论是：**通常不是 RoPE 去适配奇数维，而是模型设计时就让旋转维度保持偶数**。

**Q2：相邻两个维度对的角速度 $\theta_i$ 是连续的吗？不连续怎么理解“近转得快、远转得慢”？**

这里要分清两个“方向”：

- **沿位置 $pos$ 的变化**：对固定维度对 $i$，角度通常是 $\phi_{pos,i}=pos\cdot \theta_i$。当 $pos$ 每 +1，角度增加 $\theta_i$，因此对每个 $i$ 来说，角度随位置是“线性递增”的（在离散步上稳定增加）。
- **沿维度对索引 $i$ 的变化**：$i$ 本来就是离散的，所以严格说“连续”不适用。RoPE 选的是一组按指数铺开的角速度：

$$
\theta_i = 10000^{-2i/d}
$$

相邻两项的比例为常数：

$$
\frac{\theta_{i+1}}{\theta_i}=10000^{-2/d}
$$

这意味着角速度在对数尺度上均匀铺开，不会“突然跳一大截”。于是“转得快/转得慢”的直觉就很清楚：

- 小 $i$：$\theta_i$ 大 → 每走 1 个 token 转得多 → 更敏感于近距离差异（高频）
- 大 $i$：$\theta_i$ 小 → 每走 1 个 token 转得少 → 覆盖更长距离（低频）

> 这句话不需要 $\theta_i$ 对 $i$ 连续，只要 $\theta_i$ 随 $i$ 单调下降并覆盖多尺度即可成立；指数分布只是一个很合理的多尺度铺法（避免冗余、覆盖更均匀）。

> RoPE 的工程价值之一：在不引入可学习位置表的前提下，有不错的外推能力；同时衍生出很多“扩展上下文长度”的技巧（后续章节一般会展开）。

----------

### 4.2.4 注意力机制的推理侧优化：KV cache、MQA、GQA

#### 1) KV cache（自回归生成的必需品）

自回归生成时，t 时刻只新增 1 个 token，但如果每步都对所有历史重新算 $K,V$，代价是重复的。KV cache 的做法是：

- 参考图（原文 KV cache 示意）：

![](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter4/images/4-14-KV_cache.png)

- **缓存历史 token 的 $K,V$**（每层、每头/每组）
- 新 token 只需要算一次当前步的 $Q$（以及当前步的 $K,V$ 并追加到 cache）
- 注意力时用“当前 $Q$” 去和 “缓存的 $K$” 做打分，再对缓存的 $V$ 加权

##### KV cache 量化（KV quantization）：在量化什么？为什么做？有什么代价？

近年的推理框架（例如 SGLang 等）常会对 **KV cache** 做量化/低精度存储。直观上它在做的是：把注意力里“占显存大头”的历史 $K,V$ 从 FP16/BF16 改成更省的表示（如 INT8/FP8），以换取更长上下文或更大吞吐。

**量化的对象是什么？**

- 不是量化模型权重（weight quantization），而是量化 **推理过程中不断增长的缓存张量**：每层的 $K$ 与 $V$。  
- 对 decoder-only 自回归生成而言，KV cache 的体积大致随序列长度 $S$ 线性增长，且会被每一步注意力反复读取，因此它既是**显存开销**也是**带宽开销**。

**常见做法（实现视角，概念化）**

1) **按组/按通道缩放（scale）**  
把张量分成若干组（例如按 head_dim 分组，或按固定 group size），每组用一个（或少数几个）缩放因子 $s$ 表示该组的动态范围。

2) **把 FP16/BF16 的 KV 映射到低精度整数/FP8**  
最常见的直觉形式是：

$$
x \approx s \cdot q,\quad q\in\{-127,\dots,127\}\ \text{(INT8)}
$$

存储时保存 $q$（更小的字节数）以及缩放因子 $s$（很少的额外开销）。

3) **注意力计算时按需反量化，或直接在低精度上算**  
- 有的实现会在 attention kernel 内部把所需块反量化到 FP16/BF16 再做点积；
- 也有实现可以在更低精度路径上完成部分计算（取决于硬件与 kernel）。

**为什么要做 KV cache 量化？（最核心动机）**

- **省显存**：KV cache 通常是长上下文推理时的显存瓶颈。把 KV 从 16-bit 降到 8-bit，理论上可把 cache 显存近似减半（再叠加分组 scale 的少量额外开销）。
- **省带宽、提升吞吐**：注意力在长上下文时往往受“从显存读出大量 KV”限制（memory-bandwidth bound）。KV 更小意味着每步读的数据更少，可能带来更高吞吐。
- **更长上下文**：同一张卡上能容纳更长的 $S$（或同样 $S$ 下能放更多并发请求）。

**会不会增加计算量？速度一定更快吗？**

- **会增加一些计算**：需要量化/反量化（以及乘 scale）的额外算子，属于额外开销。
- 但很多场景下 attention 是带宽瓶颈：**少搬数据**带来的收益可能大于多做一点算术，因此整体反而更快。
- 实际效果依赖：上下文长度、batch/并发、kernel 是否融合（fused）、硬件对 INT8/FP8 的支持等。

**对显存的影响怎么直观理解？**

把 KV cache 想成“每层都要存一份长度为 $S$ 的历史表”，大致规模可记成：

- cache 大小 $\propto$ 层数 $L$ × 序列长度 $S$ × KV heads 数 × head_dim × dtype 字节数

因此你能从三个方向省显存：

- **降 dtype**：FP16(2B) → INT8/FP8(1B)（KV quantization）
- **降 KV heads**：MHA → GQA/MQA（结构改动）
- **压缩表示**：例如 MLA（结构改动）

#### 2) MQA（Multi-Query Attention）

- **做法**：多个头共享同一组 $K,V$，但各自仍有独立的 $Q$。
- **动机**：推理时 KV cache 往往是显存瓶颈；共享 $K,V$ 直接减少 cache 体积。
- **代价**：表达能力可能下降（从 MHA 直接到 MQA 比较激进）。

#### 3) GQA（Grouped-Query Attention）

- **做法**：把 query heads 分组；每组共享一套 $K,V$（介于 MHA 与 MQA 之间）。
- **动机**：在“更小 KV cache”与“更强表达能力”之间折中，因此在不少模型里非常常见。

----------

### 4.2.5 更长序列的注意力：稀疏/滑动窗口/混合模式

- **核心矛盾**：全注意力对序列长度 $S$ 的计算与显存通常是 $O(S^2)$ 级别（至少在“物化注意力矩阵”的实现上）。
- **早期思路**：稀疏/结构化注意力（块稀疏、对角线连接、滑动窗口等）用可控模式近似全局依赖。
- **新趋势**：混合策略（例如“偶尔全注意力 + 多数层局部窗口 + RoPE 只用于局部”）来兼顾效率与外推。

----------

### 4.2.6 DeepSeek 的 MLA / DSA（了解其在“推理显存/长上下文成本”上的定位）

#### 1) MLA（Multi-head Latent Attention）

- **定位**：对 KV cache 做“有损压缩”，把多头的 KV 联合投到低秩潜在空间缓存，推理时再上投影恢复近似的 $K,V$。
- **权衡**：显存更省，但每次注意力需要额外的（解压/投影）计算。

形式化记忆（只记“下投影缓存 + 上投影还原”）：

$$
c_K = xW_K^{down}\in\mathbb{R}^{S\times r},\quad c_V=xW_V^{down}\in\mathbb{R}^{S\times r}
$$

$$
K=c_KW_K^{up},\quad V=c_VW_V^{up}
$$

其中 $r\ll d$。

#### 2) DSA（DeepSeek Sparse Attention）

- **定位**：先用轻量模块给历史 token 打“重要性代理分数”，再对每个 query 选 Top-k 进入精算注意力，把复杂度从 $O(S^2)$ 拉到接近 $O(S\cdot k)$。
- **直觉**：长文本里“真正影响下一步预测”的 token 往往只是一小部分。

----------

### 4.2.7 稠密模型（Dense） vs 稀疏模型 / MoE：怎么选、坑在哪

这一节的目标是把两个容易混淆的概念讲清楚：**稠密（dense）**和**稀疏（sparse）**到底“稀疏在哪里”，以及它们为什么分别适合不同的训练/部署场景。

#### 1) 定义：稠密 vs 稀疏（MoE 是最常见的稀疏形态）

- **稠密模型（Dense）**：对每个 token，网络中的主要参数（例如每层的 FFN/注意力投影）都会参与计算。直观就是“每一步都跑完整模型”。
- **稀疏模型（Sparse）**：对每个 token，只激活/使用一部分参数。
  - **MoE（Mixture-of-Experts）**：把某些层（通常是 FFN/MLP 部分）替换为“多个专家 + 路由器（router）”。对每个 token，router 只选择少数 $k$ 个专家（常见 top-1 / top-2）参与计算。

#### 2) 最重要的直觉：参数量与每-token 计算量的关系

- **Dense**：总参数量 $P$ 变大时，通常每 token 的计算也随之变大（你确实在“用全部参数算一遍”）。
- **MoE**：总参数量可以非常大（专家很多），但每 token 只走 $k$ 个专家，因此每 token 的计算更接近“一个较小模型 + 路由与通信开销”。
  **一句话**：MoE 能把“模型容量（参数数）”和“每 token 计算量”在一定程度上解耦。

#### 3) Dense 的优势与劣势

- **优势**
  - **训练更稳定、工程更简单**：没有路由，不存在专家负载不均、token 扎堆导致的通信热点等问题。
  - **推理部署更直接**：计算图规则、延迟更可控；并行策略（张量并行/流水线并行）更成熟。
- **劣势**
  - **规模变强更“硬”**：想提升容量通常就要直接增加计算（更多层/更宽维度 → FLOPs 上升）。
  - **同等 FLOPs 下的参数容量不如 MoE**（因为 dense 必须让每个 token 经过所有参数）。

#### 4) MoE/稀疏的优势与劣势（重点：工程/通信/稳定性）

- **优势**
  - **更高的参数容量**：在相近的每-token FLOPs 下放入更多参数，常能提升训练性价比（同算力下更强）。
  - **可扩展性强**：增加专家数是一条“扩容量”的路径（前提：并行和路由做对）。
- **劣势**
  - **路由与负载均衡困难**：token 可能扎堆去少数专家，导致热点、吞吐下降甚至训练不稳；需要额外机制（负载均衡损失、capacity factor、router jitter 等）。
  - **分布式通信更复杂**：专家往往分布在多张 GPU 上，常出现 all-to-all / token 交换，通信代价和实现复杂度显著上升。
  - **小 batch / 低并发推理更吃亏**：请求少时专家并行利用率低、计算不规则，延迟更抖。
  - **“看起来参数很大”但每 token 只用一部分**：这是优势（省计算）也是限制（并非每次都动用全容量）。

#### 5) 记忆版选型建议

- **想要稳定、实现简单、推理延迟可控**：优先 **Dense**。
- **在大规模训练/大集群上追求更高性价比（同 FLOPs 更强）**：考虑 **MoE**，但要接受更高的工程复杂度与稳定性成本。

----------

### 4.2.8 对比：原始 Transformer vs LLaMA-like vs Qwen-like（分别改了什么）

这一节只对齐“**架构层面的关键差异**”（不展开数据/训练配方）。你可以把 LLaMA 与 Qwen 当成两条非常典型的 **LLaMA-like 收敛路线**：RMSNorm + RoPE + (Swi)GLU + decoder-only，并在推理效率/长上下文上继续演进。

> 参考图（来自课程原文的对比总览图，直观看“2017 → 2025”的组件演进）：  
> ![2017-2025 架构对比总览](https://raw.githubusercontent.com/datawhalechina/diy-llm/main/docs/chapter4/images/4-5-models.png)

#### 1) 先明确“原始 Transformer（2017）”的默认设定

这里按论文《Attention Is All You Need》的经典设计来记：

- **编码器-解码器（Encoder-Decoder）**：N 层 Encoder + N 层 Decoder（而不是单纯 decoder-only）。
- **位置编码**：输入端加一次**正余弦绝对位置编码**（Sin/Cos PE）。
- **归一化**：**Post-LN**（残差相加后再 LayerNorm）。
- **FFN 激活**：ReLU（带 bias）。
- **注意力**：标准 MHA（每头独立 Q/K/V）。

#### 2) LLaMA-like：相对原始 Transformer 的核心改动（“三件套 + decoder-only”）

LLaMA 系列（以及一大批开源模型）最典型的架构改动可以概括为：

- **decoder-only + causal mask**：去掉 Encoder，只保留 Decoder 堆叠（更适合自回归语言建模）。
- **Post-LN → Pre-LN**：把归一化挪到子层输入端，训练更稳（本笔记 `4.2.1` 已解释）。
- **LayerNorm → RMSNorm**：更省算、更少参数（无 $\beta$），效果常相当。
- **ReLU/GeLU → SwiGLU（门控 FFN）**：FFN 更强、经验上更稳/更好。
- **Sin/Cos PE → RoPE**：位置不再“加在输入”，而是对 Q/K 进行旋转，天然表达相对位置。
- **（推理侧常见）GQA/MQA**：后续模型（如 LLaMA2/3 等）常把 KV 头数做得更少以减 KV cache（实现上表现为 `n_kv_heads < n_heads`）。

**参考（代码级佐证）**：官方 LLaMA 推理实现里可以直接看到 `RMSNorm`、SwiGLU 形态的 MLP（`silu(w1(x)) * w3(x)`）以及 RoPE 组件。  
链接：[`meta-llama/llama` 的 `model.py`](https://github.com/meta-llama/llama/blob/main/llama/model.py)

#### 3) Qwen-like：在 LLaMA-like 主干上，更强调“推理效率 + 长上下文工程化”

以 Qwen2 技术报告为代表，Qwen 的骨干也基本属于 LLaMA-like 收敛路线，但它在报告中更“明确写出来/工程化落地”的点主要是：

- **仍是 decoder-only**（与 LLaMA 一样）。
- **Pre-Norm + RMSNorm + RoPE + SwiGLU**：与 LLaMA-like 主干一致（报告中明确列出）。
- **GQA 作为默认注意力配置**：用更少的 KV heads 来显著减小 KV cache（提升推理吞吐/降低显存）。
- **QKV bias（特定层保留 bias）**：报告/相关讨论中强调其与 RoPE 外推、稳定性等可能有关（属于工程经验项）。
- **长上下文系统化方案**（Qwen2 重点）：  
  - 训练上下文扩到 32K；  
  - 提升 RoPE base（如 $10^4\rightarrow 10^6$）以支持更长上下文；
  - 配合 YARN、Dual Chunk Attention（DCA）等进一步做长度外推/长序列训练。
- **提供 MoE 变体**：MoE 仅替换 FFN 子层，注意力子层与稠密版本同型（见报告公式）。

**参考（权威来源）**：Qwen2 Technical Report 的 Model Architecture 小节。  
链接：[Qwen2 Technical Report（arXiv HTML）](https://arxiv.org/html/2407.10671v1)

#### 4) 一个“背诵版”对比表（只记差异点）

- **原始 Transformer**：Encoder-Decoder + Sin/Cos PE + Post-LN + ReLU FFN + 标准 MHA  
- **LLaMA-like**：decoder-only + RoPE + Pre-LN + RMSNorm + SwiGLU（以及常见 GQA/MQA）  
- **Qwen-like**：LLaMA-like 主干 + **更明确的 GQA 默认** + **QKV bias** + **长上下文工程（RoPE base/YARN/DCA）** +（可选）MoE 变体

----------

### 4.2.9 “长上下文”到底在说什么？以 Qwen2 为代表的一套工程路线

这一节回答三个问题：**上下文增加是不是 context window 变大？为什么会变大？怎么直观/数学理解？** 并以 Qwen2 Technical Report 里公开描述的训练路线为例（不同版本/型号细节以官方报告为准）。

#### 1) 术语对齐：context window / 训练上下文 / 推理上下文

- **context window（上下文窗口）**：模型一次能“看见并处理”的最大 token 数（上限）。它既可能来自**训练时见过的最大长度**，也可能来自**推理阶段的位置编码/注意力工程技巧**把可用长度进一步拉长。
- **训练上下文长度**：预训练/继续训练时，真实把序列切到多长来算注意力（决定模型“学过”多长的依赖模式）。
- **推理上下文长度**：线上服务允许用户输入多长；可能等于训练上限，也可能通过外推/插值等技术**超过**训练上限（风险是质量下降或需要专门训练）。

> 直观：把 Transformer 想成“读一条长度为 $S$ 的纸条”。**纸条变长**就是 context window 变大；但要让它真的好用，通常需要 **训练见过足够长的样本** + **位置机制在更长 $S$ 上仍然数值稳定且语义合理**。

#### 2) Qwen2 这类模型“把上下文做长”，通常做了哪几类事？

以 Qwen2 报告描述为例，它把长上下文当成一个系统工程，而不是只改一个超参：

- **把训练阶段的最大序列长度做大**（例如报告提到从较短上下文扩到更长上下文训练；具体数字以报告表格为准）。  
  直觉：模型需要在梯度更新中真实经历“远距离依赖”，否则长距离行为主要靠外推猜测。
- **RoPE 基频/频率表调整（RoPE base 变大）**：报告提到将 RoPE base 从 $10^4$ 提升到 $10^6$ 量级以优化长上下文表现。  
  直觉：RoPE 用不同频率的旋转去编码位置；**频率尺度**决定“同样走 1 个 token，相位变化多快”。调 base 等价于调整整套频率标尺，让更长序列上相位不会过快进入混乱/饱和区（工程上常与插值/缩放联用）。
- **长度外推/插值类方法：YARN**（报告明确提到用于 rescale attention weights 以改善长度外推）。  
  直觉：当你想把模型用到比训练更长的 $S$ 时，注意力 logits 的尺度与 RoPE 相位分布可能“失配”，YARN 这类方法通过**重标定注意力权重**去缓解外推恶化。
- **长序列注意力结构：Dual Chunk Attention（DCA）**（报告提到把长序列分 chunk，在 chunk 内外组织注意力以提升长上下文）。  
  直觉：全注意力在长序列上贵且难训；DCA 用“分块 + 跨块连接”的方式在**算力/显存**与**远程依赖**之间找折中。
- **推理侧配套（不训练也能影响可用长度/成本）**：更省 KV cache（GQA）、KV 量化、FlashAttention 等——它们更像“让长上下文**跑得动**”，本身不一定等价于“模型更会长依赖”，但能决定你能不能开到更大的窗口。

权威参考：[Qwen2 Technical Report（arXiv HTML）](https://arxiv.org/html/2407.10671v1)

#### 3) “上下文变大”为什么会难？（数学直觉：RoPE 相位 + 注意力尺度）

给一个不需要背公式的直觉版：

- RoPE 本质是让 $Q,K$ 随位置发生旋转；位置从 $m$ 走到 $m+\Delta$，相当于乘上一串旋转 $R(\Delta\cdot\omega)$（$\omega$ 由频率表决定）。  
  当 $\Delta$ 很大时，不同频率分量会累积到很大的相位差；如果训练只在较小 $\Delta$ 上“校准”过，那么在超大 $\Delta$ 上就容易出现**匹配失准**（外推问题）。
- 注意力里还有 $\frac{1}{\sqrt{d_k}}$ 缩放与外推时的各种尺度问题；YARN 这类方法就是在修“**长序列上注意力分布/ logits 尺度**”这件事。

一句话：**长上下文训练=让模型在更长的 $S$ 上把 RoPE+注意力这条数值链路训稳定；长上下文外推=在更长 $S$ 上重新校准频率与注意力尺度，让行为不崩。**

#### 4.2.9.1 YARN（YaRN）是什么？怎么理解？怎么实现？

**是什么：** YaRN 全称 *Yet another RoPE extensioN*（Peng 等，2023），是一套把 **RoPE 模型的可用上下文窗口**从训练长度 $L$ 扩到更长 $L'$ 的方法族，核心目标是：**既不要“全局硬拉伸 RoPE 频率”导致高频信息崩坏，又要在超长序列上让注意力 softmax 不至于变得过平/过尖**。

论文与代码（建议直接对照原文）：[YaRN: Efficient Context Window Extension of Large Language Models](https://arxiv.org/abs/2309.00071) · [实现参考仓库](https://github.com/jquesnelle/yarn)

**怎么理解（拆成两块）：**

1) **对 RoPE 频率做“分段/分维度”的缩放（NTK-by-parts 思路）**  
把 RoPE 的每个维度对看成不同“波长/频率”的位置编码通道。简单地把所有频率统一除以缩放因子 $s=L'/L$（线性插值 PI）会伤害高频通道；YaRN 的做法是对不同频率通道采用不同的插值/外推策略，并用一个 ramp 函数 $\gamma(\cdot)$ 在“低频/高频”之间平滑过渡。直觉：**低频更像外推、高频更像插值**，避免一刀切的副作用。

2) **对注意力 logits 做温度缩放（attention temperature）**  
当上下文非常长时，注意力分布可能出现“过平/过尖”等不稳定现象。YaRN 在注意力 softmax 前引入温度 $t$，把 logits 从

$$
\text{softmax}\left(\frac{q_m^\top k_n}{\sqrt{|D|}}\right)
$$

改成（概念上）

$$
\text{softmax}\left(\frac{q_m^\top k_n}{t\sqrt{|D|}}\right)
$$

论文还给出了 LLaMA 系列上拟合得到的经验关系（用于把 $t$ 与缩放因子 $s$ 联系起来），常见写法是：

$$
\sqrt{\frac{1}{t}} \approx 0.1\ln(s)+1
$$

直觉：$t$ 在调节 softmax 的“尖锐程度”，从而在长上下文下把注意力的熵/稀疏性拉回更健康的区间。

**怎么实现（工程上通常改哪里）：**

- **改 RoPE 的频率生成**：在计算 `cos/sin`（或等价的 `inv_freq`）时，对每个维度通道应用 YaRN 的分段缩放规则（而不是只做一个全局常数缩放）。
- **改注意力缩放**：在 attention logits 上引入 $t$（有时可通过等价变换吸收进 RoPE 的缩放实现里，取决于框架是否 fused）。
- **通常还需要少量长上下文继续训练（continue pretrain / finetune）**：YaRN 论文强调可以用很少比例的数据把窗口扩起来；纯推理外推也有相关动态版本，但效果与风险要单独评估。

**和 Qwen2 报告里那句 “YARN rescale attention weights” 怎么对齐？**  
本质就是上面第 2 点：通过温度/缩放改变注意力 logits 的有效尺度，从而改变 softmax 后的注意力分布（你口语里说的“rescale attention weights”，数学上通常对应 logits 侧的温度/缩放，而不是改 $V$ 的数值）。

## 4.3 超参数：行业经验法则与“不要迷信”

这一节建议背成“默认值 + 例外 + 你应该怎么理解它”。

----------

### 4.3.1 FFN 宽度：$d_{ff}$ 与 $d_{model}$ 的比例

- **非门控 MLP（ReLU/GeLU 类）常见默认**：$d_{ff}\approx 4d_{model}$
- **GLU 变体的常见经验**：为了控制参数量，常用 $d_{ff}\approx \frac{8}{3}d_{model}\approx 2.66d_{model}$

#### 重要提醒：这不是铁律

T5 曾经用过非常激进的比例（论文里出现过远大于 4 的设置），说明“只要总体参数/算力分配合理”，很多比例都可能工作；但其后续版本也回到更常规配置，表明“激进并不必然更好”。

----------

### 4.3.2 注意力头数与维度：保持 head dim 稳定是常见策略

常见共识：让

$$
\frac{(\text{num\_heads}\cdot \text{head\_dim})}{d_{model}}\approx 1
$$

- **直觉**：多数模型更倾向于“增加头数来提升并行/多视角”，而不是把每个头做得很宽。

----------

### 4.3.3 宽 vs 深：宽深比影响并行策略选择

除了纯损失曲线外，宽深比还会影响可用的并行方式：

- **更深**：更适合流水线并行（层切分）。
- **更宽**：更依赖张量并行（矩阵切分），通常对互联带宽/延迟更敏感。

经验法则之一：每层大约“128 个隐藏维度”的粒度在不少 GPT/LLaMA 变体里常见（作为直觉即可）。

----------

### 4.3.4 词表大小：从 3-5 万到 10-25 万

- **早期/单语**：常见 30k–50k
- **面向部署/多语**：10 万到 25 万越来越常见

#### 提问：大词表对单一高资源语言（如中文/英文）一定更好吗？

**答**：未必。大词表更确定的收益往往在**多语言/低资源语言**：用更少 token 表达同样文本，从而降低推理成本与上下文占用。

----------

### 4.3.5 dropout 与 weight decay：预训练里谁更常见？

- **dropout**：预训练里逐渐“降温”，不少新模型很少依赖它。
- **weight decay**：仍然很常见，但它在预训练中的作用不一定是“防过拟合”这么简单；一些观察表明它会与学习率调度产生耦合，从而影响训练末期的优化动态。

#### weight decay 是什么？（把它从“正则化口号”还原成“更新规则里的一项”）

**weight decay(权重衰减)**指：在优化器更新参数时，除了沿着梯度方向走之外，再额外把参数往 0 拉一点，典型形式是：

$$
\theta \leftarrow \theta - \eta g - \eta\lambda \theta
$$

- $\theta$：参数（权重）
- $g$：梯度（由 loss 反传得到）
- $\eta$：学习率
- $\lambda$：`weight_decay`（衰减强度）

直觉：它给权重加了一个“**持续收缩**”的力，避免权重在训练中无意义地变大；同时会改变优化轨迹（尤其在 Adam 这类自适应优化器里）。

#### AdamW 的“解耦 weight decay”到底是什么意思？

在 **Adam** 族里，如果你把 L2 正则当成“给 loss 加 $\frac{\lambda}{2}\|\theta\|^2$，再对 $\theta$ 求导”，那么 **L2 梯度项会进入 $m,v$ 的自适应统计**里，导致 **weight decay 的强度与自适应缩放耦合**，调参更难、行为更怪。

**AdamW（Decoupled Weight Decay）**的做法是：把 **weight decay 从梯度路径里拆出来**，在参数更新时**单独**施加衰减（不把它混进构造 $m,v$ 的梯度里）。一句话：

- **Adam + L2（耦合）**：decay 通过修改 $g$ 影响 $m,v$
- **AdamW（解耦）**：decay 直接作用在 $\theta$ 上，不被自适应分母“二次加工”

工程上这通常就是你在大模型训练里看到的默认：`torch.optim.AdamW(..., weight_decay=...)`。官方说明见：[AdamW — PyTorch 文档](https://pytorch.org/docs/stable/generated/torch.optim.AdamW.html)

#### 代码上怎么写？（最常见：AdamW）

```python
import torch
import torch.nn as nn
import torch.optim as optim

model = nn.Linear(128, 64)

# weight_decay 就是 λ（强度）；AdamW 会把 decay “解耦”到参数更新里
opt = optim.AdamW(model.parameters(), lr=3e-4, weight_decay=0.1)

loss = model(torch.randn(8, 128)).sum()
loss.backward()

opt.step()          # 执行参数更新（含解耦 weight decay）
opt.zero_grad(set_to_none=True)  # 清空梯度：比置零更省显存/更快（PyTorch 推荐写法）
```

#### 预训练里 weight decay “未必是在防过拟合”：怎么理解才准确？

在“海量数据 + 往往跑不满很多 epoch / 甚至接近单 epoch”的 **LLM 预训练**里，经典叙事“weight decay 防止过拟合”经常**不够贴切**，更贴近事实的说法是：

- **它更像一个训练动力学/尺度控制项**：影响权重增长、与自适应优化器的交互、以及训练末期的轨迹。
- **它会和学习率调度强耦合**：例如余弦衰减到很小时，weight decay 的相对影响会变化，从而改变最后阶段的收敛形态（有时甚至会帮助得到更低的训练损失——这不等价于“泛化更好”，也不等价于“防过拟合”）。

所以你在读技术报告时，看到 “we use AdamW with weight decay” 时，脑子里应把它翻译成：

- **“我们用 AdamW，并启用解耦权重衰减来塑造优化过程”**  
而不是自动翻译成  
- **“我们主要靠 weight decay 防过拟合”**（这在预训练语境里常常不准确）

----------


## 4.4 稳定性：softmax 是“问题儿童”，常见干预都围绕它

Transformer 里有两个典型 softmax：

- **输出层 softmax**：词表维度巨大，logits 动态范围可能失控。
- **注意力 softmax**：logits = $QK^\mathsf{T}/\sqrt{d_k}$，数值过大时会导致极端分布与梯度问题。

----------

### 4.4.1 z-loss：稳定输出层 softmax 的归一化因子

z-loss（PaLM 等提出并使用）核心正则项：

$$
\mathcal{L}_{z}=\lambda\cdot \log^2 Z,\quad Z=\sum_{i=1}^{V}\exp(z_i)
$$

总损失：

$$
\mathcal{L}=\mathcal{L}_{CE}+\lambda\log^2 Z
$$

- **直觉**：别让 softmax 的“分母规模”$Z$ 变得极端，从而让训练更稳。

----------

### 4.4.2 注意力 softmax 的稳定化：Q/K 归一化

把 $Q,K$ 在做点积前先归一化（例如 LayerNorm）：

$$
Q=\text{LN}(W_q(x)),\quad K=\text{LN}(W_k(x)),\quad \text{logits}=QK^\mathsf{T}/\sqrt{d_k}
$$

- **直觉**：不去直接约束 $Z$，而是先控制 logits 的数值范围，让 softmax 输入天然“别太离谱”。

----------

### 4.4.3 logit 软截断（soft-clipping / logit-capping）

常见形式：

- 给定 cap（比如 30）
$$
\text{logits}_{clip}=cap\cdot\tanh(\text{logits}/cap)
$$

- **效果**：把 logits 范围软限制在 $(-cap, cap)$，减少极端值导致的数值问题。
- **现实**：并非在所有设置下都提升效果（有时可能伤性能），通常作为“稳定性工具箱”之一。

----------

## 4.5 复盘：把“架构选择”当作一组可解释的权衡

- **位置机制**：Sin/Cos（简单无参） → RoPE（注意力内注入相对位置，现代主流）
- **归一化**：Post-LN（原始） → Pre-LN（更稳） → RMSNorm（更省，几乎标配）
- **FFN/激活**：ReLU/GeLU → GLU 家族（门控更强，但要控制参数与算力）
- **推理效率**：KV cache（必需） + MQA/GQA（省 KV 显存） + 更激进的压缩/稀疏（MLA/DSA 等）
- **稳定性**：围绕 softmax 做约束（z-loss / QK norm / logit capping）

----------

## 小型自测（用来检查你是不是真的会了）

- **你能不看资料写出 MHA 的核心形状流转吗？** 从 `[B,S,d_model]` 到 `[B,h,S,d_head]`，再到 `[B,h,S,S]`，最后合并回 `[B,S,d_model]`。
- **你能解释 Pre-LN 为什么更容易训练深层吗？**（提示：残差路径与梯度尺度）
- **你能用一句话解释 RoPE 的“相对位置”从哪来吗？**（提示：$R(a+b)=R(a)R(b)$）
- **你能说清 KV cache 为什么会成为推理显存瓶颈吗？**（提示：层数×序列长度×(K,V)）

## 小型自测·参考答案（对照复盘用）

- **MHA 形状流转（并行实现视角）**  
  1) 输入 $X$：`[B,S,d_model]`  
  2) 线性投影：$Q=XW_Q,\ K=XW_K,\ V=XW_V$，形状仍是 `[B,S,d_model]`  
  3) reshape 分头并交换维度：`[B,S,d_model] -> [B,S,h,d_head] -> [B,h,S,d_head]`，其中 $d_{head}=d_{model}/h$  
  4) 打分：`scores = Q @ K^T`（对最后两维做矩阵乘），得到 `[B,h,S,S]`  
  5) softmax 后乘 $V$：`attn @ V` 得 `[B,h,S,d_head]`  
  6) 合并多头：`[B,h,S,d_head] -> [B,S,h*d_head]=[B,S,d_model]`，再乘输出投影 $W_O$

- **Pre-LN 为什么更容易训深层（抓住 Jacobian 里的 $I$）**  
  Pre-Norm：$y=x+F(\text{LN}(x))$，对 $x$ 求导会出现 **$I$ + 变换支路项**：梯度至少有一条“残差直通”路径；而 Post-Norm：$y=\text{LN}(x+F(x))$ 会让梯度每层都乘上 LN 的 Jacobian，深层更容易不稳、更依赖 warmup。

- **RoPE 的相对位置一句话**  
  对 $Q,K$ 分别乘上位置相关的旋转后，利用 $R(a+b)=R(a)R(b)$ 与正交性，点积里会自然出现只依赖 $(m-n)$ 的相对旋转项（相对位移），而不是只靠绝对位置相加。

- **KV cache 为什么容易成为推理显存瓶颈**  
  自回归每步都要保存历史 token 的 $K,V$；总缓存规模大致随 **层数 $L$ × 序列长度 $S$ × KV heads × $head_{dim}$ × dtype 字节数** 增长，且随 batch/并发线性放大，因此长上下文时往往先于算力成为瓶颈（同时也吃内存带宽）。