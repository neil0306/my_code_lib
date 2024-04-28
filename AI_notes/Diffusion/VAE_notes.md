# VAE 基础
VAE：Variations Auto-Encoders
- Auto-encoder：同时包含编码器和解码器的神经网络结构。
- VAE的功能是实现新的图片/文字的生成任务。

> In a nutshell(总的来说), a **VAE is an autoencoder** whose **encodings distribution is regularised during the training** in order to ensure that its latent space has good properties allowing us to **generate some new data**. 
> 
> Moreover, the term “variational” comes from the close relation there is between the regularisation and the variational inference method in statistics.


要理解 VAE，首先需要知道一些背景知识：
1. 什么是降维
2. 什么是auto-encoder
3. 什么是变分推断（Variational Inference）

## 降维
### PCA
PCA：Principle Component Analysis, 主成分分析。
- 一种基于线性变换的降维方法，通过线性投影，将原来的数据投影到n维空间中(n小于原来的数据维度)，由于投影过程中只保留n个维度（相当于信息压缩），此时会丢失一些信息，这些信息是不可恢复的。
- 降维的目的通常是为了可视化、数据存储和节省计算量等。

一些降维过程图例：
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*ayo0n2zq_gy7VERYmp4lrA@2x.png)

![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*LRPyMAwDlio7f1_YKYI2hw@2x.png)


### Autoencoder
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*UdOybs9wOe3zW8vDAfj9VA@2x.png)

autoencoder的降维发生在上图的中间 $e(x)$ 部分，这部分通常维度要比原来的x维度低得多，此时信息被压缩了。
>For a given set of possible encoders and decoders, we are looking for the pair that **keeps the maximum of information when encoding** and, so, **has the minimum of reconstruction error when decoding**. 

![](https://miro.medium.com/v2/resize:fit:1400/format:webp/1*bY_ShNK6lBCQ3D9LYIfwJg@2x.png)

在autoencoder中，我们并**不要求**原来的数据投影到一组`正交基(orthogonal basis)`上, 甚至**不要求**低维空间的基向量之间是独立的。
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*ek9ZFmimq9Sr1sG5Z0jXfQ@2x.png)

理想情况下，我们希望encoder能尽可能压缩到一个非常非常低维的空间中（比如压缩到1维，反正就是压缩率无穷大之类的），并且同时希望decoder能无损恢复出原来的数据。这当然是存在很大代价的。
>we should however keep two things in mind. 
>
>First, an important dimensionality reduction with no reconstruction loss often comes with a price: the lack of interpretable and exploitable structures in the latent space (**lack of regularity**). 
>
>Second, most of the time the final purpose of dimensionality reduction is not to only reduce the number of dimensions of the data but to reduce this number of dimensions **while keeping the major part of the data structure information in the reduced representations**. For these two reasons, the dimension of the latent space and the “depth” of autoencoders (that define degree and quality of compression) have to be carefully controlled and adjusted depending on the final purpose of the dimensionality reduction.


![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*F-3zbCL_lp7EclKowfowMA@2x.png)

总而言之，autoencoder可以简单理解为`一种可以实现尽可能无损压缩`的工具（在训练得很好的前提下）。

现在已经知道了什么是降维，也知道了auto-encoder长啥样，它们跟VAE有什么关系？
- 首先，auto-encoder由于中间的隐空间（latent space，信息压缩后存放的空间）的规律性很难捕捉，这种规律通常取决于初始空间中数据的分布、隐空间的维度和encoder的架构。这将导致我们很难确保encoder对隐空间的操作 是与我们期望的 生成过程 相匹配。

考虑一个极端例子，如果latent space只是1维空间，则：
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*iSfaVxcGi_ELkKgAG0YRlQ@2x.png)
- 在这个极端例子中，encoder将会对训练数据过拟合，此时我们从latent space采样，再经过decoder得到的数据将大概率是无意义的数据。
- 这说明latent space的规律性对于生成新图片非常重要。

> When thinking about it for a minute, this lack of structure among the encoded data into the latent space is pretty normal. Indeed, nothing in the task the autoencoder is trained for enforce to get such organisation: **the autoencoder is solely trained to encode and decode with as few loss as possible, no matter how the latent space is organised.** Thus, if we are not careful about the definition of the architecture, it is natural that, during the training, the network takes advantage of any overfitting possibilities to achieve its task as well as it can… unless we explicitly regularise it!

毕竟autoencoder的主要目标是实现近乎无损的数据压缩，然后近乎无损的还原，它不关心中间latent space长啥样也是情有可原的。

## VAE
VAE的整体结构与autoencoder差不多，都包含了一个encoder和一个decoder，但是VAE对latent space做了显示了正则化（regularized），使得这个结构可以完成较好的新图片/数据的生成。

>Instead of encoding an input as a single point, we encode it **as a distribution** over the latent space. 

VAE的训练逻辑如下：
>first, the input is encoded as distribution over the latent space.
>
>second, a point from the latent space is sampled from that distribution.
>
>third, the sampled point is decoded and the reconstruction error can be computed.
>
>finally, the reconstruction error is backpropagated through the network
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*ejNnusxYrn1NRDZf4Kg2lw@2x.png)

在实际操作中，通常将训练后的encode distribution设置为`正态分布`，此时，encoder在训练时就可以通过`均值和协方差矩阵`来描述这个分布，省事得很。
- 因为这个设定，通常就说VAE同时具备了 `local` 和 `global` 两层正则，local 通过 **协方差矩阵** 控制，global 则通过 **均值** 控制。
    - 协方差和均值的作用体现在loss function中，如下图所示
    ![](https://miro.medium.com/v2/resize:fit:1400/format:webp/1*Q5dogodt3wzKKktE0v3dMQ@2x.png)
    - loss 的第一项描述的是“重建误差”：描述decode后的数据与原数据之间的差异。
    - 对于 latent space，训练目标是让它里面的数据点服从**正态分布**，为了衡量当前分布与目标分布的差异，这里用了 Kulback-Leibler divergence（KL散度），又因为这本质上就是两个高斯分布的计算，所以最后计算的无非是均值和协方差矩阵之间的差异罢了。


### 从直觉(intuition)上理解VAE中正则项设定为正态分布
从数据生成的角度分析，我们一定是希望模型满足以下两个特性：
1. **continuity**：latent space 中靠近的两个点代表的信息应该是相近的，体现在decode之后的数据长得差不多。
2. **completeness**：对于latent space中的分布，从上面采样的点经过decode之后得到的东西应该是有意义的。

如下图所示：
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*83S0T8IEJyudR_I5rI9now@2x.png)

从这两个属性分析的话，我们在正则化的时候仅要求 encoder 将 input 映射到隐空间中的某个分布是不够的，因为从下面的loss function来看
$$
    loss = ||x - \hat{x} ||^2 + KL[N(\mu_x, \sigma_x), N(\bold{0},\bold{I})]
$$
 - 即便没有对正则项做很好的设计，模型为了使得重建误差最小（第一项）依旧可以继续训练，因为它完全可以直接忽略KL散度那一项，此时模型的表现就跟 autoencoder 没什么区别，都是对训练数据 overfitting。
   - 此时，模型要么把latent space中的数据分布整成一个 方差很小的分布（**点分布**，punctual distributions），表现为**数据点都集中到一坨** （这时候去采样，相邻的采样点可能解码出来会相差很多，比如一个是猫一个是狗）；要么整成一个 **包含不同信息的点间隔很远的一种分布**，表现为**均值很大**（这时候如果去采样，就很容易落在一些没有意义的位置上）。这两种情况都是无法同时满足上面提到的continuity和completeness属性的。


这时候，如果我们将latent distribution强制为正态分布会怎么样？
- 对于均值：在学习的过程中逐渐变成0，隐空间里的数据不会散得那么开
- 对于协方差矩阵：在学习的过程中逐渐趋近于单位矩阵，隐空间里不同类别的信息不会混到一起。

如下图所示：
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*9ouOKh2w-b3NNOVx4Mw9bg@2x.png)


理想状态下，我们希望训练后的latent space为下图的样子：
![](https://miro.medium.com/v2/resize:fit:1400/format:webp/1*79AzftDm7WcQ9OfRH5Y-6g@2x.png)
- 不同类别的数据在隐空间中分布较为集中
- 在隐空间中如果采样的点落在不同分布的重叠位置时，该采样点解码后的数据应该包含有意义的信息（比如图中，从红色和蓝色区域重叠的地方采样的点，解码后会呈现出圆与三角形的一些特征）


### 从数学角度理解VAE
这里会用到概率论的知识作为推导框架。

首先，这里用$x$表示输入数据，用$z$表示隐空间（latent space）里的随机变量，z服从我们上面说的某种概率分布，在**生成数据时**，本质上完成了以下两个步骤：
1. 从某种`先验分布`$p(z)$中采样，得到隐空间中的采样值 $z$
   > 既然要生成数据了，那就说明模型已经训练好了，因此隐空间的分布 p(z) 是已知量是合理的。生成新数据的时候，先从隐空间中采样出一个表征新数据信息的采样值，也合理。
2. 接着从`条件似然`$p(x|z)$中采样，得到输入 $x$  （注意这里的条件似表达式的z从第一步中已经获得）
   > 有了隐空间的采样值，就可以丢到decoder中进行解码，解码这个步骤从概率论角度来看，其实就是从分布 p(x|z) 中采样一个数据出来而已。

   > 这过程说明，针对**生成过程**而言，如果用贝叶斯理论来分析：
    > encoder 本质上等价于概率分布 $p(z|x)$  ====> 后验分布 posterior
    > 隐空间的分布 $p(z)$                 ======> 先验分布 prior
    > decoder 本质上等价于概率分布 $p(x|z)$  ====> 似然 likelihood

![](https://miro.medium.com/v2/resize:fit:1128/format:webp/1*dxlZr07dXNYiTFWL62D7Ag@2x.png)

进一步地，我们用贝叶斯公式来表述这个过程：

$$
% \begin{split}
    p(z|x) = \frac{p(x|z)p(z)}{p(x)} = \frac{p(x|z)p(z)}{\int p(x|u)p(u)du}
% \end{split}
$$
式子里的$p(u)$指的是一个服从均匀分布的随机变量。

为了使计算方便，同时也不失一般性，我们可以`假设p(z)是一个标准高斯分布`，此时，经过推导之后，`p(x|z) 也将具有高斯分布的形式`（有理论证明的，这里不展开了），也就是：
$$
\begin{split}
p(z) &\overset{def}{\equiv} \mathcal{N}(0,I)\\
p(x|z) &\equiv \mathcal{N}(f(z),cI)  \space \space f \in F \space \space c > 0
\end{split}
$$
式中$f()$表示某种变换，该变换属于某个函数族F，它负责将隐变量z进行变换，变换后得到的随机变量会服从高斯分布，并且具有`协方差矩阵为常数c乘以一个单位矩阵`的特性。
- 如果我们能确定$f()$是什么形式的变换，那么理论上来说，我们就知道了$p(z)$和$p(x|z)$，用贝叶斯公式就直接能得到$p(z|x)$.

但是，由于上面的贝叶斯公式中，分母存在积分项，在实际应用中我们需要使用近似计算的办法才能得到结果。
- 这种近似的方法就是`variational inference`, 变分推断。


这里小结一下：
1. 我们可以将p(z)和p(x|z)通过 “假设+推导” 的办法得出一个高斯分布的形式。
2. 如果我们进一步有 `E(x|z) = f(z) = z`, 也就是 $p(x|z) \equiv \mathcal{N}(z,cI)$的话，理论上来说，这将意味着后验概率 $p(z|x)$也服从高斯分布，并且它的均值和协方差矩阵可以写成 “与 p(x) 和 p(x|z) 的均值和协方差矩阵相关” 的某种表达式。
3. 由于贝叶斯公式中，分布有积分项，第2点情况并不现实，此时需要`借助变分推断的方法`，对前面的假设进行一定的修正，然后得到一个具有通用型且鲁棒的解。

### variational inference
核心思想：
- 设置一个参数化的分布族（例如高斯族，其参数是均值和协方差），并在该族中寻找我们目标分布的最佳近似值。
- 这些分布族中的最佳匹配分布应满足：最小化给定近似误差。如果用 Kullback-Leibler 散度来衡量目标分布与当前分布的误差，那我们的优化目标就是将KL散度最小化，这个过程就是神经网络的训练过程。

具体来说，**假如我们已经知道了 $f()$ 是什么样的变换，并借此知道了后验概率是一个高斯分布**，接下来我们需要做的事情是**用一个高斯分布 $q_x(z)$ 来近似 $p(z|x)$**, 其中，$q_x(z)$的均值和协方差都是x的函数, 并且这两个函数都是可参数化的：
$$
q_x(z) \equiv \mathcal{N}(g(x),h(x)), \quad g \in G \quad h \in H
$$
- 此时，我们做的事情变成“调整 $g()$ 和 $h()$ 里的参数，使得 $q_x(z)$ 与 目标分布 $p(z|x)$ 的KL散度最小”。
- 对比前面分母包含积分的贝叶斯推断公式，用变分推断方法的巧妙之处就是**避开了计算积分等复杂数学计算，直接从求解目标入手，将中间的计算给模糊化**，我们只需要确保最后得到的结果能近似为理论最优值即可。

写成数学表达式：
$$
\begin{split}
(g^*,h^*) &= \underset{(g,h) \in G \times H} {\arg \min} KL(q_x(z), p(z|x)) \\
    &= \underset{(g,h) \in G \times H} {\arg \min} (\mathbb{E}_{z \sim q_x}(\log q_x(z)) - \mathbb{E}_{z \sim q_x}(\log \frac{p(x|z)p(z)}{p(x)})) \\ 
    &= \underset{(g,h) \in G \times H} {\arg\min} (\mathbb{E}_{z \sim q_x} (\log q_x(z)) - \mathbb{E}_{z \sim q_x}(\log p(z)) - \mathbb{E}_{z \sim q_x}(\log p(x|z)) + \mathbb{E}_{z \sim q_x}(\log p(x)) ) \\ 
    &= \underset{(g,h) \in G \times H} {\arg\max} (  \mathbb{E}_{z \sim q_x}(\log p(x|z)) - KL(q_x(z),p(z))  ) \\ 
    &=  \underset{(g,h) \in G \times H} {\arg\max} ( \mathbb{E}_{z \sim q_x} (- \frac{||x-f(z)||^2}{2c}) - KL(q_x(z),p(z)))
\end{split}
$$

从公式的倒数第二行可以发现，要得到后验概率 p(z|x)，则需要将第一项那个 log-likelihood 最大化，同时使得KL散度最小（即 $q_x(z)$ 和 $p(z)$ 这两个分布尽可能接近） 
- 这意味着，我们在使用“观测值预测输入的分布”的同时，还需要确保 $q_x(z)$ 与先验分布非常接近。这是一个trade-off.
>This tradeoff is natural for Bayesian inference problem and express the balance that needs to be found between **the confidence we have in the data** and **the confidence we have in the prior**.

总的来说，变分推断本质是一种迭代优化的思想，它通过假设一个我们熟悉的、可参数化的分布族 来构建出 未知分布与目标分布之间的关系式，之后的任务就是用观测数据迭代地从分布族中找出最优的参数来支撑构建出来的关系式，说人话就是搞出了一个loss function，然后迭代，获得能让loss最小的模型参数。


详细的讲解可以去看看博客：
- https://towardsdatascience.com/bayesian-inference-problem-mcmc-and-variational-inference-25a8aa9bce29


### 再看VAE
回到VAE最开始的问题，VAE想要的是实现`一种 encoder-decoder 的结构`，它不仅`能实现较好的信息压缩和信息解码`，还`能满足生成新数据的要求`（**从隐空间采样的点能解码成有意义的东西，并且隐空间中的数据点满足某种概率分布规**律）

我们的分析逻辑是：

先根据贝叶斯定义，得到式子
$$
% \begin{split}
    p(z|x) = \frac{p(x|z)p(z)}{p(x)} = \frac{p(x|z)p(z)}{\int p(x|u)p(u)du}
% \end{split}
$$

然后**假设了隐空间里的数据分布 $p(z)$ 是一个标准正态分布** （纯粹是为了计算方便 同时还能 用上现成的数学工具），此时 likelihood 也可以通过某种变换$f()$ 来获得 高斯分布 的形式：
$$
\begin{split}
p(z) &\overset{def}{\equiv} \mathcal{N}(0,I)\\
p(x|z) &\equiv \mathcal{N}(f(z),cI)  \space \space f \in F \space \space c > 0
\end{split}
$$

考虑到贝叶斯公式中**分母有积分，实际中很难求解**，于是考虑**用变分推断来求出近似的后验概率分布 p(z|x)**，也就是用了KL散度作为度量手段的 loss function term，然后最小化loss，但是使用变分推断之前，`需要知道 f() 具体是什么变换`.

至此，我们不难看出：**如果隐空间的规律性主要由先验分布p(z)来决定**，VAE的**整体性能将很大程度取决于 $f()$ 函数的选择**。因此，在整个VAE中，比较难搞的就是后验概率分布的 协方差矩阵的C 和 均值里的$f()$。
- 注意上面式子里的 likelihood 其实就是 decoder，所以 $f()$ 也会直接影响到 decoder。 

假设我们通过$f()$的最优解$f^*()$找到了最优的后验概率，记为$q^*_x(z)$，从这个分布中我们可以得到的采样值$z$；从 likelihood（也就是decoder）中，我们可以从中得到解码后的输出 $\hat{x}$ (即$\hat{x} = f(z)$)，由于我们总是希望重建误差无穷小，因此，如果输入是给定的，即$x$，则我们希望VAE能实现 $x = \hat{x}$。根据这些定义，我们可以写出：
$$
\begin{split}
f^* &= \underset{f \in F}{\arg\max} \mathbb{E}_{z \sim q^*_x} (\log p(x|z)) \\ 
    &= \underset{f \in F}{\arg\max} \mathbb{E}_{z \sim q^*_x}(-\frac{||x-f(z)||^2}{2c})
\end{split}
$$
注意式子中的 $q^*_x(z)$ 与 $f$ 有关。$x$与$f(z)$ 计算重建误差，因为z是从$q^*_x(z)$中采样，所以这里重建x时使用的采样值是经过了f进行变换的。

将变分推断的过程整合进来，可以得到：
$$
\begin{split}
    (f^*, g^*, h^*) = \underset{(f,g,h) \in F \times G \times H}{\arg\max} \left( \mathbb{E}_{z \sim q_x} \left( -\frac{||x-f(z)||^2}{2c}\right)  - KL(q_x(z),p(z)) \right) 
\end{split}
$$
从式子中，如果结合前面提到的 $p(x|z) \equiv \mathcal{N}(f(z),cI)$，还可以发现：
  - 当c越大时，协方差越大，这意味着decoder重建的数据与均值$f(z)$之间有很高的方差（重建误差较大），意味着模型偏向于对隐空间进行正则化；
  - 反过来，如果c越小，就说明当前模型偏向于优化重建误差。

至此，对于VAE，我们需要求解的目标有三个 $f, g, h$，因为直接求解很难，现在都用神经网络去替代直接的数学建模。

实际场景中，$g, h$的网络结构通常属于encoder部分，并且存在部分共享权重，于是有：
$$
g(x) = g_2(g_1(x)) \quad h(x) = h_2(h_1(x)) \quad g_1(x) = h_1(x)
$$
![](https://miro.medium.com/v2/resize:fit:1400/format:webp/1*XYyWimolMhPDMg8qCNlcwg@2x.png)
<center>Encoder part of the VAE</center>


进一步地，从式子 $q_x(z) \equiv \mathcal{N}(g(x),h(x))$ 可知 $h(x)$决定了协方差矩阵，因此$h(x)$一般是个方阵，**为了降低计算难度**，通常也还会进一步**假设p(z|x)是一个多元高斯分布，并且这个协方差矩阵是个对角阵**（当然这肯定会损失p(z|x)精度，但性价比高啊），此时连带着$g(x)$也会是一个方阵，size与$h(x)$相同。

对于$f()$，通常设计在decoder部分：
![](https://miro.medium.com/v2/resize:fit:1400/format:webp/1*1n6HwjwUWbmE9PvCzOVcbw@2x.png )
<center>Decoder part of the VAE</center>

除了这两个网络结构，我们还要确保这个网络能**基于梯度下降算法**训练起来，也就是一定`要确保整个流程是可导的`，此时在decoder部分就能发现 $z$ 是经过`采样`得到的嘞，**采样这个操作本身并不可导**！！为了使这个过程可导，研究者们提出了一个叫作`reparametrisation trick`（重参数化）的操作，**由于 $f(z)$ 服从高斯分布**，均值为$g(x)$，协方差矩阵为$H(x)=h(x) \cdot h^T(x)$，所以有：
$$
z = h(x)\zeta + g(x)   \quad \zeta \in \mathcal{N}(0,1)
$$

![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*S8CoO3TGtFBpzv8GvmgKeg@2x.png)
<center>Illustration of the reparametrisation trick</center>

最后，在实际情况中，一个分布的均值我们通常还是用 Monte-Carlo 方法来逼近的，说人话就是直接拿大量的采样点计算的均值来近似理论均值，所以，对于下面的式子：
$$
\begin{split}
    (f^*, g^*, h^*) = \underset{(f,g,h) \in F \times G \times H}{\arg\max} \left( \mathbb{E}_{z \sim q_x} \left( -\frac{||x-f(z)||^2}{2c}\right)  - KL(q_x(z),p(z)) \right) 
\end{split}
$$
我们会将均值符号干掉，然后用一个大写的$C$替代$-\frac{1}{2c}$，然后得到下图给的 loss function 定义式：
![](https://miro.medium.com/v2/resize:fit:2000/format:webp/1*eRcdr8gczweQHk--1pZF9A@2x.png)
<center>Variational Autoencoders representation.</center>


# 总结
VAE就是一个同时具备 “encoder-decoder” 的网络结构，即一个autoencoder，它能具有以下特性：
1. encoder 在理想情况下可以将input很好地、接近无损地压缩到一个低维的latent space中；
2. decoder 在理想情况下可以将latent space中的每一个数据解码出来，如果input有对应数据的话，理想情况下能解压出一模一样的数据(属于一种无损的信息解压)
3. latent space里的数据具有一定的规律性，从概率论角度来说就是具有某种概率分布的特征，通过这个规律性进行采样，得到的采样点经过解码之后可以得到有意义的新数据。 -- 能生成有意义的新数据。
   - 在 latent space 中，相邻数据点经过decoder解码出来的数据应该具有相似性 -- **continuity property**
   - latent space 中根据它的分布随便采样出来的数据点，经过decoder后得到的数据是有意义的  -- **completeness property**

VAE的loss function一共包含两项：
1. reconstruction term --> 控制数据的重建误差
2. regularisation term --> 控制 latent space

由于在求解过程中使用了统计学中的 variational inference, 故名字中包含 variational，又因为本身就是一个autoencoder，故全名 variational autoencoder, VAE.

# Reference
- [ ] https://towardsdatascience.com/understanding-variational-autoencoders-vaes-f70510919f73



