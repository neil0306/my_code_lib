# GAN 基础认知
- GAN: Generative Adversarial Networks, 对抗生成网络
  - 网络结构主要由两个部分组成：1. 生成器（Generator）；2. 鉴别器（Discriminator）
  - 对抗体现在生成器与鉴别器之间：
    - 生成器负责`合成`新图，鉴别器负责`判断图片是否为合成图`。生成器的训练目标是让鉴别器无法分辨出自己生成的图是合成的，鉴别器的训练目标是分辨出每一张合成图。

GAN 网络结构首次提出是在**2014 年**，由 Ian J. Goodfellow 等人提出，[原文章链接](https://arxiv.org/abs/1406.2661).


## Generator 基本原理
从结果来看，Generator 主要能力是合成一张图片，那么，怎么合成一张图？逻辑是什么？在回答之前，需要理解以下知识：
1. 计算机如何获得某个指定的随机变量
2. 怎么获得随机变量的采样值
3. 生成器能合成图片的本质

### 计算机如何获得某种指定的随机变量
首先明确一点，我们目前使用的所有计算机本质上都没有任何【随机性】，它所做的一切都是人为定死的，是固定的，计算机的随机只是一种`伪随机`，是一种`近似的随机`（只是看起来像随机的东西）。
- 怎么实现生成随机数这个事情这里不深究，总之以前的科学大牛们用概率论已经证实了可以这么用**伪随机数生成器 (pseudorandom number generator)**来生成一种近似的随机数，误差肯定是有的，但是总体可接受，大家就这么用了。
  - 伪随机数生成器需要依赖一个“种子（seed）”，当种子固定下来的时候，每次生成的序列也就是固定的了，所以在复现一些神经网络的时候，大家都会把随机种子也设定为相同的数值，以最大的可能性保证能复现结果。

### 怎么获得随机变量的采样值
随机变量可以用`某种操作(operation) 或 过程(process)`来表征，比较常见的操作有：
1. Rejection sampling（拒绝采样）
    > **Rejection sampling** expresses the random variable as the result of a process that consist in sampling not from the complex distribution but from a well known simple distribution and to accept or reject the sampled value depending on some condition. Repeating this process until the sampled value is accepted, we can show that with the right condition of acceptance the value that will be effectively sampled will follow the right distribution.

2. Metropolis-Hastings 算法 
    > the idea is to find a **Markov Chain (MC)** such that the stationary distribution of this MC corresponds to the distribution from which we would like to sample our random variable. Once this MC found, we can simulate a long enough trajectory(轨迹) over this MC to consider that we have reach a steady state and then the last value we obtain this way can be considered as having been drawn from the distribution of interest.

3. 【下面讲解】Inverse transform

从概率论的角度出发，`随机变量是根据累积分布函数定义的`，并且一个随机变量的概率密度函数（Probability Density Function，PDF）进行积分后，得到的就是累积分布函数（Cumulative Distribution Function，CDF）：
$$ CDF_X =  \int p(x)dx = \mathbb{P}(X \le x) $$

因此，对于一维的情况，我们会有：
$$ CDF_{X}(x) = \mathbb{P}(X \le x) \in [0,1] $$
而对于服从均匀分布的随机变量 U，有
$$ CDF_{U}(u) = \mathbb{P}(U \le u)   \space \space \forall u \in [0,1]  $$

假设随便变量 x 存在一种`CDF的逆变换`（inverse transform），使得下面的 (随机变量 y 的 CDF 的定义) 式子成立 
$$ Y =  CDF^{-1}_X(U)$$

这个式子说的是“将某个均匀分布的采样值经过 某种 CDF 逆变换 可以得到任意 CDF 的采样值 Y” ，那么，此时可以推导出下面式子：

$$ 
\begin{split}
    CDF_Y(y) &= \mathbb{P}(Y \le y) \\
     &= \mathbb{P}(CDF^{-1}_X(U) \le Y) \\
     &= \mathbb{P}(U \le CDF_X(y)) \\
     &= CDF_X(y)
\end{split}
$$

这个递推式说的是“经过中间的 CDF 逆变换之后，随机变量 x 和随机变量 y 具有相同的 CDF”。根据最开始提到的`随机变量可以由它的CDF确定`说明，只要我们能很好地构造出那个 CDF 的反变换函数 ( $Y =  CDF^{-1}_X(U)$ ), 那么理论上就能实现`我们想要的随机变量x 可以通过 将服从均匀分布的随机变量丢到这个逆变换函数 来定义`。

用图来表示这个过程就是：
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*Xoz06MKgbw7CZ8aNbMCt6A.jpeg)
- 现在有一个均匀分布（图片里蓝色的部分）
- 在经过某种映射（前面定义中讲的 $CDF^{-1}_X$）后，也就是图中的灰色线
- 映射到了我们的目标随机变量的分布中，图片中橙色区域。

不难看出图片中有一个"不变量" -- 面积，将蓝色方块给铺到下面的橙色区域中。

## 生成能合成图片的本质
在计算机里，一张图片读进来是一个矩阵，以灰度图为例，读进来就是一个 (h,w) 的矩阵，矩阵里的数值就是像素值。此时我们可以将这张图“展平”，变成一个向量，这个向量包含 $h*n$ 个元素。
  - 那么，我们想要从一个向量“恢复”到一张图，只需要进行一次 reshape 操作。

所以，如果我们要生成一张图片，首先就需要生成一个向量，然后想办法让这个向量 reshape 之后看起来像是包含了某些正常内容的图片即可。

如果从概率论的角度来看待，一张包含小狗的图片在展平为向量之后，这个向量里的数值其实可以看作是从某个概率分布中采样得到的。

于是，生成器其实就是`生成从某个概率分布中的采样值`，并且这个概率分布隐含着某种图片中要显示的场景的信息。大致流程如下图所示：
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*CkMMefLPqcEKPuuPLZY2_A.png)

结合前面提到的 inverse transform 可以知道，生成器的训练本质上就是让神经网络学习到一种 CDF 的逆变换，这个逆变换的输入是从均匀分布中采样得到的向量。操作起来就是：
- 用伪随机生成器，得到一堆看起来像是服从均匀分布的数，这些数字构成一个向量。
- 把这个向量丢到神经网络 (feed into generator)。
- 神经网络输出一个向量 (output vector), 然后 reshape 成图片的样子。


# 训练 GAN
在 GAN 里面，generator 和 discriminator 是分开训练的：
- 先用带标签的数据训练出一个还不错的鉴别器，它能划分出哪些是合成的图。
- 固定住鉴别器的权重，训练生成器，目标是让鉴别器分辨不出当前生成的图是合成的。

细节省略。。。不同文章可以有很多改进方法。

# reference
- [ ] https://towardsdatascience.com/understanding-generative-adversarial-networks-gans-cd6e4651a29

