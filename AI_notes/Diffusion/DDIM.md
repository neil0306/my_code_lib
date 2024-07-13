# DDIM (Denoising Diffusion Implicit Model)

DDIM 是 DDPM (Denoising Diffusion Probabilistic Model) 的改进版本，DDPM 其实就是原版的 Stable Diffusion. 
- 在理解 DDIM 之前，需要先打好 DDPM 的基础，可以简单看看 [diffusion 的笔记](https://github.com/neil0306/my_code_lib/blob/897f628bfbc9817876dc6442aa41a770113dc4d4/AI_notes/Diffusion/diffusion_model.md)

在 DDPM 中存在的问题：
1. 在生成图片时，需要模拟**多个步骤**的 Markov Chain (比如走 1000 次 Unet 来预测每一个 time step 的噪声，然后做减法去噪), 导致计算速度慢，计算量也大。
   - time step 一般设置比较大是因为 **只有加入的高斯噪声足够多的时候，最后一次加完噪声得到的图片才近似为一个高斯分布**.

DDIM 的主要改进&效果：
1. [改进]使用更一般化的 **non-Markov** 过程，**将随机的过程转化为确定性的过程，从而可以采用 trick 进行加速**. 
2. [效果]相比 DDPM, 生成图片的速度快 10 倍~50 倍。


DDIM 论文中的公式符号与 DDPM 公式符号的区别：
* DDPM 中用 $\bar{\alpha}$ 来表示$\alpha$连乘，但是**在 DDIM 中是直接用 $\alpha$ 来表示这个连乘**.
$$
\begin{aligned}
\text{DDPM 中的写法:} \quad &p(x_t | x_{t-1}, x_0) \sim \mathcal{N}(\sqrt{\bar{\alpha}_{t}} \cdot  \boldsymbol{x_{t-1}}, (1 - \bar{\alpha}_{t})\boldsymbol{I})  \\
\text{DDIM 中的写法:} \quad &p(x_t | x_{t-1}, x_0) \sim \mathcal{N}(\sqrt{\alpha_{t}} \cdot  \boldsymbol{x_0}, (1 - \alpha_{t})\boldsymbol{I})
\end{aligned}
$$

---

## 关于 DDIM 的目标函数
DDIM 使用的目标函与 DDPM 相同，都是噪声之间计算 L2 norm:
$$
\begin{aligned}
\mathcal{L}_{simple}(\theta) &= \mathbb{E}_{t, x_0, \epsilon} \left| \left| \epsilon - \epsilon_{\theta} (\sqrt{\alpha_t} x_0 + \sqrt{1-\alpha_t} \epsilon, t)  \right| \right|^2 
\end{aligned}
$$
- 不过，在 DDPM 中，损失函数 $\mathcal{L}(\theta)$ **只依赖于边缘分布** $q(x_t | x_0)$, 而在 DDIM 中，损失函数 $\mathcal{L}(\theta)$ 依赖于**联合分布** $q(x_{1:T}|x_0)$. 
  - 在 DDPM 里，因为每一个 time step 之间增加了一个 Markov-Chain 的假设，才使得本来的联合分布可以拆成边缘分布。
  -  作者发现，如果我们能设计出某种 non-Markov 的前向扩散过程，使得 DDIM 的 $q(x_t|x_0)$ 与 DDPM 里的 **$q(x_t|x_0)$ 相同**, 而且同时使得 **DDIM 中的联合分布 $q(x_{1:T}|x_0)$ 与 DDPM 中的联合分布不相同**(也就那些$\alpha_t$的连乘). 这时候，通过公式推导，作者发现这个 non-Markov 前向扩散过程得到的目标函数与这条目标函数只差一个与模型参数无关的常数项，于是 DDIM 可以直接用 DDPM 的目标函数来训练模型。
     - 对于 $q(x_t|x_0)$, 它对应的就是往 $x_0$ 加 t 次噪声之后的分布，我们一般假设为高斯分布，所以这个分布就被认为是已知的。所以 non-Markov 扩散过程的设计就剩下那个联合分布了。
     - 对于联合分布的部分，再回顾一下 DDPM, 它是基于 Markov Chain 的性质而获得 $q(x_t|x_{t-1},x_0) = q(x_t|x_{t-1})$, 所以在 DDIM 中其实只需要让 $q(x_t|x_{t-1},x_0)$ 具有更一般的形式就可以了。
- 这个目标函数还是`score diffusion model (score matching)`的目标函数 (**暂待考证...**).

在这个目标函数的过程中，其实可以发现：
- 前向扩散过程并没有直接关联到 **去噪过程 (生成图片的过程) 中的后验概率$p(x_{t-1}|x_t, x_0)$**, 所以，**只要保证$q(x_t|x_0)$的形式不变，甚至可以直接使用训练好的 DDPM 模型 (也就是那个 Unet) 走 DDIM 的生成过程**.  
  - 这个推导过程在论文中也有给出，作者先给出了根据 non-Markov 扩散过程的目标函数，然后用贝叶斯公式和 KL 散度进行了化简，最后证明了使用 non-Markov 扩散过程的话，目标函数与 DDPM 的目标函数其实就差一个常数项。(这里就不探究细节了)
  - 据此可知，生成图片的时候使用的噪声分布是可以另外找的，这也是作者能够把 生成图片的过程修改为 non-Markov 的理由。


---
## DDIM 里给出的 Non-Markov 前向扩散过程
DDIM 论文中，作者给出的 non-Markov 的各种分布的公式如下：
![](DDIM_images/DDIM中的non-Markov过程公式.png)
- 作者在`文章的附录B`中证明了图中的联合分布 $q_{\sigma}(x_{1:T}|x_0)$ 可以与 DDPM 里的 $q(x_t|x_0)$完全对应，也就是都能得到 $q(x_t|x_0) = \mathcal{N}(\sqrt{\alpha_t}\boldsymbol{x_0}, (1-\alpha_t) \boldsymbol{I}) $.
  - 证明的核心思路主要是：用了下面的`高斯分布定理`, 然后结合`数学归纳法(即先验证初始条件成立, 然后假设t时刻成立, 最后如果推出 t+1 时刻也成立, 就证完了)`
    ![](DDIM_images/DDIM证明过程中用到的高斯分布定理.png)

抛开证明过程，我们可以看到，作者给的这个 non-Markov 扩散过程的联合概率分布 $q_{\sigma}(x_{1:T}|x_0)$ 的均值和方差与超参数 $\sigma$ 有关，只要改变这个参数就会直接影响到图片生成过程中的每一个 time step 的去噪后的图片分布 (也就是后验概率) $q_{\sigma}(x_{t-1}|x_{t},x_0)$.

---

## DDIM 的图片生成过程 (去噪过程)
首先，在 DDPM 中给出了从边缘分布 $q(x_t|x_0) = \mathcal{N}(\sqrt{\alpha_t} \boldsymbol{x_0}, (1-\alpha_t) \boldsymbol{I}))$ 到 $x_t$ 的产生过程 (前向扩散过程) 可以用下式表达：
$$
\begin{aligned}
x_t = \sqrt{\alpha_t} x_{0} + \sqrt{1-\alpha_t} \epsilon_t
\end{aligned}
$$
- 其中 $\epsilon_t$ 是一个高斯噪声，服从 $\mathcal{N}(0, \boldsymbol{I})$.

由此，对应的去噪过程就是：
$$
\begin{aligned}
f_{\theta}^{(t)}(x_t) = \tilde{x}_{0} = \frac{1}{\sqrt{\alpha_t}} (x_t - \sqrt{1-\alpha_t} \epsilon_{\theta}^{(t)})
\end{aligned}
$$
- 也就是已知 $x_t$ 和 t 时刻预测的噪声$\epsilon_{\theta}^{(t)}$ 就能得到此时此刻去噪后的估计值 $f_{\theta}^{(t)}(x_t)$, 显然，**这个结果也可以理解为给定$x_t$的条件下，对$x_0$的观测**.
  - $\theta$表示模型参数，$t$表示当前的 time step.

如果我们将每个时刻的$f_{\theta}^{(t)}(x_t)$都视为对 $x_0$ 的观测，此时，结合一个固定先验 $p_{\theta}(\boldsymbol{x_T}) = \mathcal{N}(\boldsymbol{0},\boldsymbol{I})$, 也就是图片加完噪声之后服从正态分布。那么，我们就可以定义出每一个时刻的后验概率 $p_{\theta}(\boldsymbol{x_{t-1}}|\boldsymbol{x_t})$:

$$
\begin{equation}
p_{\theta}(\boldsymbol{x_{t-1}}|\boldsymbol{x_t}) = 
\left\{
    \begin{aligned}
    &\mathcal{N}(f_{\theta}^{(t)}(\boldsymbol{x_t}), \sigma_1^2\boldsymbol{I}) \quad &\text{if} \quad t = 1\\
    &q_{\sigma}(\boldsymbol{x_{t-1}}|\boldsymbol{x_t}, f_{\theta}^{(t)}(\boldsymbol{x_t})) \quad &\text{otherwise} 
    \end{aligned}
    \right.
\end{equation}
$$
- 公式里的$q_{\sigma}(\boldsymbol{x_{t-1}}|\boldsymbol{x_t}, f_{\theta}^{(t)}(\boldsymbol{x_t}))$ 就是论文提出的 non-Markov 扩散过程中的后验概率分布，即前面贴的`论文公式 (7)`.


整理之后，每个 time step 从这个后验概率分布采样的样本可以表示为：
$$
\begin{equation}
x_{t-1} = \underbrace{\sqrt{\alpha_{t-1}} \left( \frac{x_t - \sqrt{1 - \alpha_t} \epsilon_{\theta}^{(t)}(x_t)}{\sqrt{\alpha_t}} \right)}_{\text{``predicted } x_0\text{''}} + \underbrace{\sqrt{1 - \alpha_{t-1} - \sigma_t^2} \cdot \epsilon_{\theta}^{(t)}(x_t)}_{\text{``direction pointing to } x_t\text{''}} + \underbrace{\sigma_t \epsilon_t}_{\text{random noise}}
\end{equation}
$$
- 这其实是按照`reparameterization trick`公式写出来的表达式，前面两项其实是后验概率$p_{\theta}(\boldsymbol{x_{t-1}}|\boldsymbol{x_t})$的`均值`, 最后一项是`标准差 x 一个高斯噪声的采样值`.
- 可以发现，"均值" 和 "方差" 这两个部分都与超参数 $\sigma_t$ 相关，所以可以通过调整$\sigma_t$来改变生成图片的效果。
  - 我们训练的模型在这个式子中对应的是 $\epsilon_{\theta}^{(t)}(x_t)$, 这说明，**如果我们训练好了一个模型，那么我们在调整超参数 $\sigma_t$ 时是不需要重新训练的!!!** ($\sigma_t$只影响采样的结果!)
  - 当 $\sigma_t = \sqrt{(1-\alpha_{t-1})/(1-\alpha_t)} \sqrt{1-\alpha_t/\alpha_{t-1}}$ 时，这个采样公式就等价于 DDPM 中的采样公式。
    - 所以论文里提到，DDIM 是 DDPM 更一般化的形式。
  - **当 $\sigma_t = 0$ 时，整个表达式将没有任何随机项，一切都是确定性的，这个模型被作者成为 DDIM (Denoising Diffusion Implicit Model)**.
    - 因为这个模型在训练的时候采用的是 DDPM 的目标函数，训练得到一个概率模型$\epsilon_{\theta}$, 而它在生成图片的过程中是`隐藏式`地使用的，因此得名 `implicit model`. 

---

## DDIM 中加速图片生成的 Respacing 技巧
出发点：
- 从去噪过程的目标函数中可以发现，它并没有限制每个 time step 的噪声必须满足马尔可夫链的性质，只要我们能让 $q_{\sigma}(x_t|x_0)$ 在每个 time step 都是高斯分布即可。这表明，我们可以在生成图片的时候，**不必非要经历完前向扩散过程那么多个 time step**, 而是经历了$[1,T]$范围内的某个子集，这个子集的长度为 S，即：
$$
\begin{aligned}
&q(x_{\tau_i}|x_0) = \mathcal{N}(\sqrt{\alpha_{\tau_i}}x_0, (1-\alpha_{\tau_i})\boldsymbol{I} ), \quad x_{\tau_i} \in \{x_{\tau_1}, \cdots, x_{\tau_S}\}, S \in [1, \cdots, T] 
\end{aligned}
$$
  - 这种感觉有点像`空洞卷积`的出发点，标准卷积操作是 kernel size 范围内的所有相邻像素点都拿来做计算，但是这样的话感受野就不够大了，于是提出空洞卷积，它不再是那卷积核中心紧挨着的像素点，而是**跳着取点**，这样感受野就变大了。对比这里的 respacing, 就是生成图片的速度变快了。

Respacing 技巧既可以用在 DDIM 上，也可以用在 DDPM 上，由于采样的过程变短了，所以整个生成图片的过程就变快了。


--- 

# 核心代码分析
Github 仓库：https://github.com/openai/improved-diffusion/tree/main

## 训练的核心函数
核心文件位于[gaussian_diffusion.py](https://github.com/openai/improved-diffusion/blob/main/improved_diffusion/gaussian_diffusion.py)文件中。



- 训练 loss 的实现：`trainning_loss`:
  ```python
  def training_losses(self, model, x_start, t, model_kwargs=None, noise=None): 
      # model 就是 Unet 模型，输入是 x_t, 输出是可以是：1. 噪声 epsilon; 2. 输出"x_0"; 3. 直接输出预测的 x_{t-1}
      # x_start: 训练集图片
      # t: 当前的 time step  (在 ViT 中，t 就是 position embedding)
  """
  Compute training losses for a single timestep.

  :param model: the model to evaluate loss on.
  :param x_start: the [N x C x ...] tensor of inputs.
  :param t: a batch of timestep indices.
  :param model_kwargs: if not None, a dict of extra keyword arguments to
      pass to the model. This can be used for conditioning.
  :param noise: if specified, the specific Gaussian noise to try to remove.
  :return: a dict with the key "loss" containing a tensor of shape [N].
            Some mean or variance settings may also have other keys.
  """
  if model_kwargs is None:
      model_kwargs = {}
  if noise is None:
      noise = th.randn_like(x_start)
  x_t = self.q_sample(x_start, t, noise=noise)   # 获取加了噪声的 x_t 样本，使用的是 q(x_t|x_0) 的分布，整个函数是基于"重参数"技巧实现的

  terms = {}

  # 根据设置的 loss 的类型走不同分支 (OpenAI 的 improve diffusion 用的第一个分支，DDIM 原始论文中用的是第二个分支，也就是用的 mse loss)
  if self.loss_type == LossType.KL or self.loss_type == LossType.RESCALED_KL:
      terms["loss"] = self._vb_terms_bpd(
          model=model,
          x_start=x_start,
          x_t=x_t,
          t=t,
          clip_denoised=False,
          model_kwargs=model_kwargs,
      )["output"]
      if self.loss_type == LossType.RESCALED_KL:
          terms["loss"] *= self.num_timesteps
  elif self.loss_type == LossType.MSE or self.loss_type == LossType.RESCALED_MSE:
      model_output = model(x_t, self._scale_timesteps(t), **model_kwargs)  # 得到模型的输出，model_outout 是一个字典，输出的东西根据使用的模型不同，可以是 "x_0" 或 "噪声" 或者 "x_{t-1}"

      # 根据 方差 sigma 是否可学习，进行分支
      if self.model_var_type in [
          ModelVarType.LEARNED,
          ModelVarType.LEARNED_RANGE,
      ]:
          B, C = x_t.shape[:2]
          assert model_output.shape == (B, C * 2, *x_t.shape[2:])
          model_output, model_var_values = th.split(model_output, C, dim=1)
          # Learn the variance using the variational bound, but don't let
          # it affect our mean prediction.
          frozen_out = th.cat([model_output.detach(), model_var_values], dim=1)
          terms["vb"] = self._vb_terms_bpd(
              model=lambda *args, r=frozen_out: r,
              x_start=x_start,
              x_t=x_t,
              t=t,
              clip_denoised=False,
          )["output"]
          if self.loss_type == LossType.RESCALED_MSE:
              # Divide by 1000 for equivalence with initial implementation.
              # Without a factor of 1/1000, the VB term hurts the MSE term.
              terms["vb"] *= self.num_timesteps / 1000.0

      # 这段代码就是区分 model_output 输出的类型了。
      ## PREVIOUS_X: 就是直接输出的 x_{t-1}, 这里本质计算的是 "均值"
      ## START_X: 就是 x_start (也就是 "x_0")
      ## EPSILON: 就是噪声
      target = {
          ModelMeanType.PREVIOUS_X: self.q_posterior_mean_variance(  # 计算均值
              x_start=x_start, x_t=x_t, t=t
          )[0],
          ModelMeanType.START_X: x_start,
          ModelMeanType.EPSILON: noise,
      }[self.model_mean_type]

      assert model_output.shape == target.shape == x_start.shape
      terms["mse"] = mean_flat((target - model_output) ** 2)   # 计算 mse, 也就是 L_{simple}
      if "vb" in terms:
          terms["loss"] = terms["mse"] + terms["vb"]
      else:
          terms["loss"] = terms["mse"]
  else:
      raise NotImplementedError(self.loss_type)

  return terms
  ```


## 采样的核心函数
核心文件位于[gaussian_diffusion.py](https://github.com/openai/improved-diffusion/blob/main/improved_diffusion/gaussian_diffusion.py)文件中。

- ddim_sample: 预测 `一个时刻` 的样本
  ```python
  def ddim_sample(
    self,
    model,
    x,
    t,
    clip_denoised=True,
    denoised_fn=None,
    model_kwargs=None,
    eta=0.0,
  ):
    """
    Sample x_{t-1} from the model using DDIM.

    Same usage as p_sample().
    """
    out = self.p_mean_variance(    # 预测 t-1 时刻的均值和方差
        model,      # Unet
        x,          # x_t
        t,          # time step
        clip_denoised=clip_denoised,
        denoised_fn=denoised_fn,
        model_kwargs=model_kwargs,
    )

    # --------- 下面这块代码就是论文里的公式 (12) 的实现 ---------
    # Usually our model outputs epsilon, but we re-derive it
    # in case we used x_start or x_prev prediction.
    eps = self._predict_eps_from_xstart(x, t, out["pred_xstart"])  # 用重参数化参数计算出 epsilon (哪怕p_mean_variance函数返回了一个, 这里还是算了一遍)
    alpha_bar = _extract_into_tensor(self.alphas_cumprod, t, x.shape)
    alpha_bar_prev = _extract_into_tensor(self.alphas_cumprod_prev, t, x.shape)
    
    # sigma_t 的式子
    sigma = (
        eta                                                       # eta = 1 时对应 DDPM, eta = 0 时对应 DDIM
        * th.sqrt((1 - alpha_bar_prev) / (1 - alpha_bar))
        * th.sqrt(1 - alpha_bar / alpha_bar_prev)
    )

    # Equation 12.
    noise = th.randn_like(x)

    mean_pred = (   # 公式(12)左半部分, 对应重参数化里的 "均值" 部分
        out["pred_xstart"] * th.sqrt(alpha_bar_prev)
        + th.sqrt(1 - alpha_bar_prev - sigma ** 2) * eps
    )

    nonzero_mask = (   # 用来控制 t == 0 时不加噪声, 因为最后一次输出是不加噪声的
        (t != 0).float().view(-1, *([1] * (len(x.shape) - 1)))
    )  # no noise when t == 0

    sample = mean_pred + nonzero_mask * sigma * noise    # 重采样的样本 = 均值 + 方差*高斯噪声
    return {"sample": sample, "pred_xstart": out["pred_xstart"]}

  # 预测 t-1 时刻的均值和方差的函数
  def p_mean_variance(
      self, model, x, t, clip_denoised=True, denoised_fn=None, model_kwargs=None
  ):
      """
      Apply the model to get p(x_{t-1} | x_t), as well as a prediction of
      the initial x, x_0.

      :param model: the model, which takes a signal and a batch of timesteps
                    as input.
      :param x: the [N x C x ...] tensor at time t.
      :param t: a 1-D Tensor of timesteps.
      :param clip_denoised: if True, clip the denoised signal into [-1, 1].
      :param denoised_fn: if not None, a function which applies to the
          x_start prediction before it is used to sample. Applies before
          clip_denoised.
      :param model_kwargs: if not None, a dict of extra keyword arguments to
          pass to the model. This can be used for conditioning.
      :return: a dict with the following keys:
                - 'mean': the model mean output.
                - 'variance': the model variance output.
                - 'log_variance': the log of 'variance'.
                - 'pred_xstart': the prediction for x_0.
      """
      if model_kwargs is None:
          model_kwargs = {}

      B, C = x.shape[:2]
      assert t.shape == (B,)
      model_output = model(x, self._scale_timesteps(t), **model_kwargs)

      if self.model_var_type in [ModelVarType.LEARNED, ModelVarType.LEARNED_RANGE]:
          assert model_output.shape == (B, C * 2, *x.shape[2:])
          model_output, model_var_values = th.split(model_output, C, dim=1)
          if self.model_var_type == ModelVarType.LEARNED:
              model_log_variance = model_var_values
              model_variance = th.exp(model_log_variance)
          else:
              min_log = _extract_into_tensor(
                  self.posterior_log_variance_clipped, t, x.shape
              )
              max_log = _extract_into_tensor(np.log(self.betas), t, x.shape)
              # The model_var_values is [-1, 1] for [min_var, max_var].
              frac = (model_var_values + 1) / 2
              model_log_variance = frac * max_log + (1 - frac) * min_log
              model_variance = th.exp(model_log_variance)
      else:
          model_variance, model_log_variance = {
              # for fixedlarge, we set the initial (log-)variance like so
              # to get a better decoder log likelihood.
              ModelVarType.FIXED_LARGE: (
                  np.append(self.posterior_variance[1], self.betas[1:]),
                  np.log(np.append(self.posterior_variance[1], self.betas[1:])),
              ),
              ModelVarType.FIXED_SMALL: (
                  self.posterior_variance,
                  self.posterior_log_variance_clipped,
              ),
          }[self.model_var_type]
          model_variance = _extract_into_tensor(model_variance, t, x.shape)
          model_log_variance = _extract_into_tensor(model_log_variance, t, x.shape)

      def process_xstart(x):
          if denoised_fn is not None:
              x = denoised_fn(x)
          if clip_denoised:
              return x.clamp(-1, 1)
          return x

      if self.model_mean_type == ModelMeanType.PREVIOUS_X:
          pred_xstart = process_xstart(
              self._predict_xstart_from_xprev(x_t=x, t=t, xprev=model_output)
          )
          model_mean = model_output
      elif self.model_mean_type in [ModelMeanType.START_X, ModelMeanType.EPSILON]:
          if self.model_mean_type == ModelMeanType.START_X:
              pred_xstart = process_xstart(model_output)
          else:
              pred_xstart = process_xstart(
                  self._predict_xstart_from_eps(x_t=x, t=t, eps=model_output)
              )
          model_mean, _, _ = self.q_posterior_mean_variance(
              x_start=pred_xstart, x_t=x, t=t
          )
      else:
          raise NotImplementedError(self.model_mean_type)

      assert (
          model_mean.shape == model_log_variance.shape == pred_xstart.shape == x.shape
      )
      return {
          "mean": model_mean,
          "variance": model_variance,
          "log_variance": model_log_variance,
          "pred_xstart": pred_xstart,
      }
  ```


- ddim_sample_loop_progressive: 用循环，预测`多个时刻的样本`
  ```python
  def ddim_sample_loop_progressive(
    self,
    model,
    shape,
    noise=None,
    clip_denoised=True,
    denoised_fn=None,
    model_kwargs=None,
    device=None,
    progress=False,
    eta=0.0,
  ):
    """
    Use DDIM to sample from the model and yield intermediate samples from
    each timestep of DDIM.

    Same usage as p_sample_loop_progressive().
    """
    if device is None:
        device = next(model.parameters()).device
    assert isinstance(shape, (tuple, list))
    if noise is not None:
        img = noise
    else:
        img = th.randn(*shape, device=device)
    indices = list(range(self.num_timesteps))[::-1]

    if progress:
        # Lazy import so that we don't depend on tqdm.
        from tqdm.auto import tqdm

        indices = tqdm(indices)

    for i in indices:
        t = th.tensor([i] * shape[0], device=device)
        with th.no_grad():
            out = self.ddim_sample(   # 用循环完成采样的整个过程
                model,
                img,
                t,
                clip_denoised=clip_denoised,
                denoised_fn=denoised_fn,
                model_kwargs=model_kwargs,
                eta=eta,
            )
            yield out
            img = out["sample"]
  ```


## Respacing 加速
文件位于：https://github.com/openai/improved-diffusion/blob/main/improved_diffusion/respace.py

- `space_timesteps()`
  ```python
  def space_timesteps(num_timesteps, section_counts):    # 从完整的时间序列中生成 子序列，子序列的生成根据 section_counts 来完成; 生成的子序列可以是"先稀疏再密集"或者"先密集再稀疏", 甚至可以"均匀取某一段子序列"
    # num_timesteps 是原来训练的时候的总步数;  section_counts 指的是采样的时候分成几部分进行采样 (列表或者字符串的形式), 字符串的话比如 "ddim50" 表示子序列一共有 50 个 time step
      """
      Create a list of timesteps to use from an original diffusion process,
      given the number of timesteps we want to take from equally-sized portions
      of the original process.

      For example, if there's 300 timesteps and the section counts are [10,15,20]
      then the first 100 timesteps are strided to be 10 timesteps, the second 100
      are strided to be 15 timesteps, and the final 100 are strided to be 20.

      If the stride is a string starting with "ddim", then the fixed striding
      from the DDIM paper is used, and only one section is allowed.

      :param num_timesteps: the number of diffusion steps in the original
                            process to divide up.
      :param section_counts: either a list of numbers, or a string containing
                            comma-separated numbers, indicating the step count
                            per section. As a special case, use "ddimN" where N
                            is a number of steps to use the striding from the
                            DDIM paper.
      :return: a set of diffusion steps from the original process to use.
      """
      if isinstance(section_counts, str):
          if section_counts.startswith("ddim"):
              desired_count = int(section_counts[len("ddim") :])
              for i in range(1, num_timesteps):
                  if len(range(0, num_timesteps, i)) == desired_count:
                      return set(range(0, num_timesteps, i))   # 根据 section counts 取得新的采样时间序列 (生成子序列)
              raise ValueError(
                  f"cannot create exactly {num_timesteps} steps with an integer stride"
              )
          section_counts = [int(x) for x in section_counts.split(",")]   # 如果 section_counts 是嵌套列表，就按照逗号分隔，转成列表
      size_per = num_timesteps // len(section_counts)
      extra = num_timesteps % len(section_counts)
      start_idx = 0
      all_steps = []
      for i, section_count in enumerate(section_counts):
          size = size_per + (1 if i < extra else 0)
          if size < section_count:
              raise ValueError(
                  f"cannot divide section of {size} steps into {section_count}"
              )
          if section_count <= 1:
              frac_stride = 1
          else:
              frac_stride = (size - 1) / (section_count - 1)
          cur_idx = 0.0
          taken_steps = []
          for _ in range(section_count):
              taken_steps.append(start_idx + round(cur_idx))
              cur_idx += frac_stride
          all_steps += taken_steps
          start_idx += size
      return set(all_steps)
  ```

- `SpacedDiffusion` 子类 (集成了父类 GaussianDiffusion, 重写了里面的几个方法) 和 `_WrappedModel` 类
  ```python
  class SpacedDiffusion(GaussianDiffusion):
      """
      A diffusion process which can skip steps in a base diffusion process.

      :param use_timesteps: a collection (sequence or set) of timesteps from the
                            original diffusion process to retain.
      :param kwargs: the kwargs to create the base diffusion process.
      """

      def __init__(self, use_timesteps, **kwargs):
          self.use_timesteps = set(use_timesteps)  # 指的是"可以使用的time step", 值可能是1, 也可能大于1 (respacing)
          self.timestep_map = []   # 基本等同于 use_timesteps, 不过这里是列表
          self.original_num_steps = len(kwargs["betas"])  # 原始步长, 在 GaussianDiffusion 中生成时间序列的时候会用到这个参数

          base_diffusion = GaussianDiffusion(**kwargs)  # pylint: disable=missing-kwoa
          last_alpha_cumprod = 1.0

          # 重新定义beta序列
          new_betas = []
          for i, alpha_cumprod in enumerate(base_diffusion.alphas_cumprod):
              if i in self.use_timesteps:
                  new_betas.append(1 - alpha_cumprod / last_alpha_cumprod)
                  last_alpha_cumprod = alpha_cumprod
                  self.timestep_map.append(i)
          
          # 更新self.betas 成员变量
          kwargs["betas"] = np.array(new_betas)  # 更新后, 父类的时间步长就变短了, 实现 respacing
          super().__init__(**kwargs)

      def p_mean_variance(  # 预测t-1时刻的方差, 均值, 以及预测的 x_0
          self, model, *args, **kwargs
      ):  # pylint: disable=signature-differs
          return super().p_mean_variance(self._wrap_model(model), *args, **kwargs)

      def training_losses(
          self, model, *args, **kwargs
      ):  # pylint: disable=signature-differs
          return super().training_losses(self._wrap_model(model), *args, **kwargs)

      def _wrap_model(self, model):   # 重点是看看封装的函数做了什么
          if isinstance(model, _WrappedModel):
              return model
          return _WrappedModel(
              model, self.timestep_map, self.rescale_timesteps, self.original_num_steps
          )

      def _scale_timesteps(self, t):
          # Scaling is done by the wrapped model.
          return t
  

  class _WrappedModel:
    def __init__(self, model, timestep_map, rescale_timesteps, original_num_steps):   # rescale_timesteps 将时间步长固定到 1000 以内
        self.model = model
        self.timestep_map = timestep_map
        self.rescale_timesteps = rescale_timesteps
        self.original_num_steps = original_num_steps

    def __call__(self, x, ts, **kwargs):
      # ts (time step) 是一个 "连续" 的索引!! map_tensor 中包含的时 respacing 之后的索引!
      # __call__ 函数的作用是将 ts 映射到真正的respacing后的 time step 上 (因为在前面那个 ddim_sample_loop 函数里, indices变量是走了一个 range 来生成 time step 的, 如果我们想要实现 [1000, 999, 996, 990, 888,..., 10, 5, 3, 1] 这种跳步的采样, 就需要先用map_tensor 进行映射!)

        map_tensor = th.tensor(self.timestep_map, device=ts.device, dtype=ts.dtype)  # 引入新变量时一定要记得放在同一个设备上
        new_ts = map_tensor[ts]
        if self.rescale_timesteps:  # 控制 new_ts 始终在 1000 以内!
            new_ts = new_ts.float() * (1000.0 / self.original_num_steps)
        return self.model(x, new_ts, **kwargs)
  ```



---

## 一些超参数和终端指令
### 训练阶段
训练模型的代码位于：
- `scripts/image_train.py`

一些核心参数：
```shell
# 模型超参数
MODEL_FLAGS="--image_size 32 --num_channels 128 --num_res_blocks 3 --learn_sigma False --class_cond False"   # 输入feature map的大小, 通道数, residual block 数量, 超参数sigma是否可学习, 是否有类别条件

# 扩散过程的超参数
DIFFUSION_FLAGS="--diffusion_steps 2000 --noise_schedule linear"  # 扩散步数, 加噪方案(线性增长 或 cosine的增长方式)

# 模型训练的超参数
TRAIN_FLAGS="--lr 1e-4 --batch_size 32 --use_kl False, --save_interval 20000" # 学习率, batch size, 是否使用 KL 散度(设置为false 表示使用上面提到的 $L_{simple}(\theta)$ 目标函数), 保存模型的间隔
```

终端命令 (上面的核心参数要先在终端里设置好)：
```shell
python scripts/image_train.py --data_dir path/to/images $MODEL_FLAGS $DIFFUSION_FLAGS $TRAIN_FLAGS
```

### 采样阶段 (图片生成)
终端指令：
```shell
python scripts/image_sample.py --model_path 模型路径 $MODEL_FLAGS $DIFFUSION_FLAGS --use_ddim True --timestep_respacing ddim50 --sample_name="图片名字"

# --use_ddim 参数表示是否使用 DDIM 的采样方式
# --timestep_respacing DDIM 中提到的加速采样步骤, 本质上就是减少采样的步数
```








--- 

# 参考资料
- [ ] 哔哩哔哩上的讲解视频
  - [DDPM 公式讲解](https://www.bilibili.com/video/BV1b541197HX/?vd_source=7cf7026bc2c23d0b0b88a3094e5ce55a)
  - [Improve Diffusion model 代码讲解](https://www.bilibili.com/video/BV1sG411s7vV/?vd_source=7cf7026bc2c23d0b0b88a3094e5ce55a)
  - [Score diffusion model 理论及代码讲解](https://www.bilibili.com/video/BV1Dd4y1A7oz/?vd_source=7cf7026bc2c23d0b0b88a3094e5ce55a)
  - [概率扩散模型 (DDPM) 与分数扩散模型 (SMLD) 的联系与区别](https://www.bilibili.com/video/BV1QG4y1674Q/?vd_source=7cf7026bc2c23d0b0b88a3094e5ce55a)


