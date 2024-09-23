# 零碎知识点

1. 关于在进行 `tokenization` 之后，得到的 `Token ID`与`词表大小  (vocab_size)`之间的联系：
```txt
- Token ID 是当前 token 在词表中 的索引，一般来说，一个单词转换成 token 之后，它看起来就不再像一个完整的单词了 (长有点像背英语单词里面的"词根", "词缀"), 这是因为我们总是希望词表中每的一个 ID 都能尽可能多地表示更多同义词或者语法上的相似词 (比如一个单词的不同时态). 这样可以减少模型的训练难度，提高模型的泛化能力。
    - 训练词表的时候，采取的策略是选取出现次数最多的那些词根词缀。

- 一个词表包含的 Token ID 越少，表明这个词表训练得越好。如果联想 LLM 的评价指标的话，有一个指标叫做 perplexity, 这个指标在一定程度上反应的是模型在猜下一个单词时，候选的单词的数量，候选单词越多，模型就越"困惑". 因此，Perplexity 越小，模型的泛化能力越好。与此同时，如果我们的词表本身就小，那么模型的 perplexity 按道理就不会变得很大，因此可以推论出词表训练得越好，模型的泛化能力会更好。
```



## llama 模型代码
llama 模型文件：[`llama_model.py`](./llama_model.py)
- 这个文件里的模型是没有使用 transformer 依赖的，将一些本来被 transformers 包封装起来的东西放回来了。

## 对模型进行预训练
对应的代码文件：[`pretrain.py`](./pretrain.py)
- 着重关注代码中的 `ctx`, 这个东西是用来设置混合精度训练的。
- 关于不同数据类型的说明，可以查看[这个笔记](https://github.com/neil0306/llm.c/blob/master/learning_notes_from_video/notes_about_dtype.md).

### 关于 loss function
训练阶段：
- Llama 3 使用的 loss function 是 `CrossEntropyLoss`, 在 llama_model.py 中对应[代码](./llama_model.py#256)如下：
    ```python
    self.last_loss = F.cross_entropy(logits.view(-1, logits.size(-1)), targets.view(-1), ignore_index=-1)  # 
    ```
    - 这里使用的是 `F.cross_entropy` 函数，这个函数是用来计算两个向量之间的交叉熵的。
    - 这里使用 `view` 方法将 `logits` 和 `targets` 展平成一维的向量，这是因为 `CrossEntropyLoss` 函数要求输入的向量是一维的。
    - `ignore_index=-1` 表示忽略 `-1` 这个索引，这是因为 `-1` 在 `targets` 中表示的是填充的 token，我们不希望计算这部分的损失。

推理阶段：
- Llama 3 在推理阶段使用的是 `generate` 方法，这个方法在 llama_model.py 中对应[代码](./llama_model.py#263)如下：
    ```python
    logits = self.output(h[:, [-1], :]) # note: using list [-1] to preserve the time dim

    ```
    - 因为模型的输出本质上是在`猜`下一个单词，根据因果关系，模型的输出其实是一个`上三角矩阵`，因此这里使用 `[:, [-1], :]` 来获取最后一个 token 的输出。




# 题外话
- [ ] 如何计算一个 LLM 的参数量
  - 具体操作：计算模型中所有参数的数量，包括权重和偏置。可以使用以下代码来计算：
    ```python
    def count_parameters(model):
        return sum(p.numel() for p in model.parameters() if p.requires_grad)
    ```
    - 解释：`model.parameters()` 返回模型的所有参数，`p.numel()` 返回参数的数量，`if p.requires_grad` 过滤掉不需要梯度的参数。
    - 在这个示例中，`count_parameters` 函数会返回模型的总参数量。你可以将这个值除以 1e9（即 10^9）来转换为“B”单位。例如，如果 `total_params` 返回 7,000,000,000，那么这个模型就是一个 7B 参数量的模型。


- [ ] 如何计算一个 LLM 的计算量
  - 具体操作：计算模型中所有参数的数量，然后乘以输入序列的长度。可以使用以下代码来计算：
    ```python
    def count_flops(model, input_shape):
        flops = 0
        for name, module in model.named_modules():
            if isinstance(module, torch.nn.Linear):
                flops += module.in_features * module.out_features
    ```
    - 设模型参数量为 `N`，训练数据量（Token）为 `D`，LLM 训练中计算量（FLOPs）$C \approx 6*N*D$
    - 参考博客：https://blog.csdn.net/wxc971231/article/details/135434478

- [ ] 梯度累积技巧 (`检查点`也称为梯度累积)
  - 具体操作：将好几个 step 的梯度累积起来，然后才进行一次 backward (梯度反向传播) 和 step (更新模型参数) 操作。
  - 目的：为了减少显存压力。
    - 原因：在 PyTorch 中，每次进行 backward 操作时，都会将计算图保存下来，如果显存不够，就会导致显存溢出。而我们在进行 batch 个样本量的 forward 和 backward 操作中，如果显存不够，那么我们可以将这个 batch 个样本拆分成更细的`mini batch`, 也就是进行多次 forward 和 backward 操作，从而减少显存压力，允许这么做的理由跟“最原始的 SGD”和“Mini-batch 的 SGD”是类似的。
    - 另外，还可以使用 `torch.cuda.empty_cache()` 方法来清空缓存，从而减少显存压力。

- [ ] 梯度裁剪
  - 具体操作：在 PyTorch 中，可以使用 `torch.nn.utils.clip_grad_norm_()` 方法来裁剪梯度。
  - 目的：为了防止梯度爆炸。
    - 原因：在深度学习中，如果梯度太大，可能会导致模型在更新参数时，更新过度，从而导致模型无法收敛。


