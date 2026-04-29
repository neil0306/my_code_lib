# 分布式训练与通信

---

## 1. 从单卡优化到集群优化

当模型规模上到数十亿/百亿参数，问题不再是“单个 kernel 快不快”，而是：

- 模型是否装得下；
- 梯度/参数如何同步；
- 通信是否压过计算。

核心问题是：  
**如何在带宽受限条件下，让多卡总体吞吐接近线性扩展。**

---

## 2. 通信原语：分布式训练的最小积木

常用 collective：

- `all-reduce`
- `reduce-scatter`
- `all-gather`
- `reduce`
- `broadcast`

工程上最关键直觉：

`all-reduce ≈ reduce-scatter + all-gather`

这个拆解决定了很多优化路径（内存、重叠、分片策略）。

### 2.1 代码示例：all-reduce / reduce-scatter / all-gather

```python
def setup(rank: int, world_size: int):
    # 控制面地址：用于进程组发现与 rendezvous
    os.environ["MASTER_ADDR"] = "localhost"
    os.environ["MASTER_PORT"] = "15623"
    if torch.cuda.is_available():
        # GPU 场景优先 NCCL，带宽和延迟表现更好
        dist.init_process_group("nccl", rank=rank, world_size=world_size)
    else:
        # CPU/调试场景走 gloo
        dist.init_process_group("gloo", rank=rank, world_size=world_size)

def cleanup():
    torch.distributed.destroy_process_group()

def collective_operations_main(rank: int, world_size: int):
    setup(rank, world_size)

    # all-reduce：先归约，再把结果回写到每个 rank
    tensor = torch.tensor([0., 1, 2, 3], device=get_device(rank)) + rank
    dist.all_reduce(tensor=tensor, op=dist.ReduceOp.SUM, async_op=False)

    # reduce-scatter：归约后，每个 rank 只留下自己负责的那一片
    input = torch.arange(world_size, dtype=torch.float32, device=get_device(rank)) + rank
    output = torch.empty(1, device=get_device(rank))
    dist.reduce_scatter_tensor(output=output, input=input, op=dist.ReduceOp.SUM, async_op=False)

    # all-gather：把各 rank 的局部片段拼回完整张量
    gathered = torch.empty(world_size, device=get_device(rank))
    dist.all_gather_into_tensor(output_tensor=gathered, input_tensor=output, async_op=False)
    cleanup()
```

这段代码直接展示了：  
`all-reduce` 的结果可以由 `reduce-scatter + all-gather` 组合复现。

**idea**
- “拆分视角”能直接映射到 ZeRO/FSDP 的参数与梯度分片通信模式。

**工程注意**
- 每个 rank 的 collective 调用序列必须一致，不一致通常直接死锁。
- 教学代码用 `async_op=False` 可读性高；生产里可用异步句柄做重叠。

---

## 3. 三类核心并行：DP / TP / PP

### 3.1 数据并行（DP）

- 每卡持有完整模型副本；
- batch 在卡间切分；
- 反向后同步梯度。

优点：简单、通用。  
限制：受全局 batch 与 all-reduce 开销约束。

### 3.2 张量并行（TP）

- 按矩阵维度切参数/计算；
- 每层前后常伴随通信同步。

优点：可扩展超大矩阵计算。  
限制：高频同步，强依赖节点内高速互联。

### 3.3 流水线并行（PP）

- 按层切分模型；
- 激活在阶段间流动；
- 需要微批调度减少 bubble。

优点：显存扩展直接。  
限制：调度复杂，利用率受气泡影响明显。

---

## 4. ZeRO / FSDP：为什么能省显存

- `ZeRO-1`：分片优化器状态；
- `ZeRO-2`：再分片梯度；
- `ZeRO-3/FSDP`：参数 + 梯度 + 优化器状态都分片。

### 直觉

把“每卡都存完整副本”改成“每卡只存自己负责分片”，在必要时通信聚合。

### 代价

通信与调度复杂度显著增加，需要重叠机制来掩蔽成本。

---

## 5. 3D 并行组合为什么常见

现实里常见组合是 DP + TP + PP（有时再加序列并行/上下文并行）。

原因：

- TP 解决节点内算力与参数切分；
- PP 解决跨节点装不下的问题；
- DP 吃剩余设备扩吞吐。

本质是把有限资源（显存、带宽、batch）按不同并行维度分配。

---

## 6. 硬件拓扑决定“什么并行放哪里”

典型链路层级：

`片上缓存/寄存器` > `HBM` > `NVLink/NVSwitch` > `跨机网络`

实践法则：

- 高通信密度（如 TP）尽量放节点内；
- 跨机更适合 DP/PP 等相对可容忍高延迟的同步模式；
- 拓扑不匹配会让理论并行收益大幅折损。

---

## 7. `torch.distributed` 代码层关键点

### 7.1 正确性

- 每个 rank 必须执行相同 collective 序列；
- 调用顺序不一致容易死锁；
- barrier 只保证同步点，不替代 CUDA 执行完成判断。

### 7.2 Benchmark 正确姿势

- 预热；
- `torch.cuda.synchronize()`；
- `dist.barrier()`；
- 再计时。

不这么做会得到“虚快”结果。

### 7.3 代码示例：通信基准（all-reduce）

```python
def all_reduce(rank: int, world_size: int, num_elements: int):
    setup(rank, world_size)
    # 每个 rank 持有同形状输入，避免输入形状差异引入变量
    tensor = torch.randn(num_elements, device=get_device(rank))

    # 预热：让通信与内核进入稳态
    dist.all_reduce(tensor=tensor, op=dist.ReduceOp.SUM, async_op=False)
    if torch.cuda.is_available():
        torch.cuda.synchronize()
    dist.barrier()

    start_time = time.time()
    dist.all_reduce(tensor=tensor, op=dist.ReduceOp.SUM, async_op=False)
    if torch.cuda.is_available():
        torch.cuda.synchronize()
    dist.barrier()
    end_time = time.time()

    duration = end_time - start_time  # 单轮 all-reduce 耗时
    size_bytes = tensor.element_size() * tensor.numel()
    # 常见粗估：每个 rank 近似“发送与接收”的双向体量
    sent_bytes = size_bytes * 2 * (world_size - 1)
    # 聚合口径写法（示例）
    total_duration = world_size * duration
    bandwidth = sent_bytes / total_duration
    return bandwidth
```

这里的核心解释：

- `2 * (world_size - 1)` 体现“发送 + 接收”；
- `barrier + synchronize` 让计时窗口在各 rank 上一致。

**工程注意**
- 带宽有“理论/有效/链路”多种口径，对比前务必统一定义。
- 小张量常 latency 主导，大张量才会体现 bandwidth 上限。

### 7.4 代码示例：通信基准（reduce-scatter）

```python
def reduce_scatter(rank: int, world_size: int, num_elements: int):
    setup(rank, world_size)
    # 输入是分片集合，归约后每个 rank 只保留一个分片
    input = torch.randn(world_size, num_elements, device=get_device(rank))
    output = torch.empty(num_elements, device=get_device(rank))

    # 预热
    dist.reduce_scatter_tensor(output=output, input=input, op=dist.ReduceOp.SUM, async_op=False)
    if torch.cuda.is_available():
        torch.cuda.synchronize()
    dist.barrier()

    start_time = time.time()
    dist.reduce_scatter_tensor(output=output, input=input, op=dist.ReduceOp.SUM, async_op=False)
    if torch.cuda.is_available():
        torch.cuda.synchronize()
    dist.barrier()
    end_time = time.time()

    duration = end_time - start_time
    data_bytes = output.element_size() * output.numel()
    # reduce-scatter 下每 rank 发送体量口径与 all-reduce 不同
    sent_bytes = data_bytes * (world_size - 1)
    total_duration = world_size * duration
    bandwidth = sent_bytes / total_duration
    cleanup()
    return bandwidth
```

这段对应原文里 all-reduce 与 reduce-scatter 的对照基准：  
前者常近似“收 + 发双向代价”，后者通信体量口径不同，不能直接按同一系数套公式。

**工程注意**
- 不同 NCCL 算法（ring/tree）会让曲线随 `world_size` 变化呈现不同拐点。
- 异常低带宽优先检查拓扑映射、NUMA 绑定、进程亲和性。

---

## 8. 并行策略代码映射（DP / TP / PP）

### 8.1 DDP 核心差异点

```python
for param in params:
    # DDP 核心：反向后做梯度全归约
    dist.all_reduce(tensor=param.grad, op=dist.ReduceOp.AVG, async_op=False)
```

相对于单机训练，这行同步梯度是数据并行最关键的行为差异。

```python
def data_parallelism_main(rank: int, world_size: int, data: torch.Tensor, num_layers: int, num_steps: int):
    setup(rank, world_size)
    batch_size, num_dim = data.size(0), data.size(1)
    # 按 batch 维切分数据
    local_batch_size = batch_size // world_size
    data = data[rank * local_batch_size : (rank + 1) * local_batch_size].to(get_device(rank))

    # 每个 rank 持有完整参数副本（DP 的本质）
    params = [get_init_params(num_dim, num_dim, rank) for _ in range(num_layers)]
    optimizer = torch.optim.AdamW(params, lr=1e-3)

    for _ in range(num_steps):
        x = data
        for param in params:
            x = F.gelu(x @ param)
        loss = x.square().mean()
        loss.backward()
        # 梯度同步：保证不同 rank 的模型副本更新一致
        for param in params:
            dist.all_reduce(tensor=param.grad, op=dist.ReduceOp.AVG, async_op=False)
        optimizer.step()
    cleanup()
```

这块补全了原文最核心的 DDP 训练骨架：本地前反向 + 梯度同步 + 参数更新。

**工程注意**
- 实际训练应在每轮显式 `zero_grad(set_to_none=True)`，避免梯度累积干扰。
- `AVG`/`SUM` 与学习率存在等效关系，切换策略时要联动调参。

### 8.2 TP 核心通信点

```python
activations = [torch.empty(batch_size, local_num_dim, device=get_device(rank)) for _ in range(world_size)]
# 从每个 rank 收集局部激活分片
dist.all_gather(tensor_list=activations, tensor=x, async_op=False)
# 沿特征维拼接回完整激活
x = torch.cat(activations, dim=1)
```

张量并行每层常需显式通信激活，这也是其带宽成本高的根源。

```python
def tensor_parallelism_main(rank: int, world_size: int, data: torch.Tensor, num_layers: int):
    setup(rank, world_size)
    data = data.to(get_device(rank))
    batch_size, num_dim = data.size(0), data.size(1)
    # 列并行：每个 rank 只负责输出特征的一部分
    local_num_dim = num_dim // world_size

    # 每层参数按列切分为 [full_dim, local_dim]
    params = [get_init_params(num_dim, local_num_dim, rank) for _ in range(num_layers)]
    x = data
    for i in range(num_layers):
        x = F.gelu(x @ params[i])  # [B, local_dim]，本 rank 局部输出
        activations = [torch.empty(batch_size, local_num_dim, device=get_device(rank)) for _ in range(world_size)]
        # 跨 rank 汇聚局部输出，恢复全维表示供下一层使用
        dist.all_gather(tensor_list=activations, tensor=x, async_op=False)
        x = torch.cat(activations, dim=1)  # [B, full_dim]
    cleanup()
```

这段补全了原文里 TP 的关键机制：每层都要把局部激活聚合回全维表示。

**idea**
- TP 用“参数切分”换“单卡显存压力下降”，代价是层间通信变频繁。

**工程注意**
- TP 非常依赖节点内高速互联（NVLink/NVSwitch）；跨机做 TP 成本通常很高。
- 切分维度需与算子实现匹配，否则会引入额外重排与拷贝。

### 8.3 PP 核心通信点

```python
if rank - 1 >= 0:
    # 从上游 stage 接收激活
    dist.recv(tensor=x, src=rank - 1)
...
if rank + 1 < world_size:
    # 发送到下游 stage
    dist.send(tensor=x, dst=rank + 1)
```

流水线并行是阶段间点对点传递激活，调度质量直接影响 bubble 大小。

```python
def pipeline_parallelism_main(rank: int, world_size: int, data: torch.Tensor, num_layers: int, num_micro_batches: int):
    setup(rank, world_size)
    data = data.to(get_device(rank))
    batch_size, num_dim = data.size(0), data.size(1)
    # 按 stage 切层：每个 rank 只持有局部层
    local_num_layers = num_layers // world_size
    local_params = [get_init_params(num_dim, num_dim, rank) for _ in range(local_num_layers)]

    # 微批是减少 pipeline bubble 的核心手段
    micro_batch_size = batch_size // num_micro_batches
    if rank == 0:
        micro_batches = data.chunk(chunks=num_micro_batches, dim=0)
    else:
        micro_batches = [torch.empty(micro_batch_size, num_dim, device=get_device(rank)) for _ in range(num_micro_batches)]

    for x in micro_batches:
        if rank - 1 >= 0:
            # 非首 stage：先接收上游激活
            dist.recv(tensor=x, src=rank - 1)
        for param in local_params:
            x = F.gelu(x @ param)
        if rank + 1 < world_size:
            # 非末 stage：把本 stage 输出送给下游
            dist.send(tensor=x, dst=rank + 1)
    cleanup()
```

这段补全了原文 PP 代码块的主干流程：分阶段、分微批、点对点传激活。

**idea**
- PP 通过“按层切图”解决模型装不下问题，再用微批把流水线填满。

**工程注意**
- 微批数太少会导致 bubble 明显；太多会抬升调度与通信开销。
- Stage 负载需尽量均衡，否则最慢 stage 决定整条流水线吞吐上限。

---

## 9. 带宽测量公式的直觉

以 all-reduce 为例，常见粗估会考虑：

- 每 rank 发送 + 接收；
- 与 `world_size` 相关；
- 总耗时要按并行参与者统一视角折算。

重点不是追求一个固定 GB/s 数字，而是用统一口径比较不同策略和不同规模下的变化趋势。

---

## 10. 性能优化顺序（实战）

1. 先确认瓶颈在算还是在传；
2. 再定并行拆分（DP/TP/PP 比例）；
3. 再做通信与计算重叠；
4. 再调微批与流水线调度；
5. 最后看系统层异常（长尾请求、故障重试、拓扑拥塞）。

---

## 11. 一句话总结

核心不是记住并行名词，而是学会资源权衡：  
**显存、带宽、算力、批大小** 四者之间的动态平衡。

