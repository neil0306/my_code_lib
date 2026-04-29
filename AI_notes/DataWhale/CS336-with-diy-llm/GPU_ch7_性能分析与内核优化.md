# GPU 性能分析与内核优化

---

## 1. 性能优化主线

GPU 性能优化可以按以下顺序推进：

1. **Benchmark**：确认是否慢、慢多少；
2. **Profiling**：确认慢在哪里（CPU launch、GPU kernel、sync、memory）；
3. **Kernel Optimization**：再决定是 fusion、Triton、CUDA 还是 compile。

和 roofline 分析是同一套思路：先定位瓶颈，再做针对性优化。

---

## 2. Benchmark：为什么一定要“预热 + 同步 + 多次测量”

```python
def benchmark(description: str, run: Callable, num_warmups: int = 1, num_trials: int = 3):
    # 预热：避免把首次 JIT 编译、内核加载、频率爬升计入结果
    for _ in range(num_warmups):
        run()

    # GPU 是异步执行，计时前先清空队列
    if torch.cuda.is_available():
        torch.cuda.synchronize()

    times = []
    for _ in range(num_trials):
        start_time = time.time()
        run()
        # 不同步会只测到“CPU 提交任务”的时间
        if torch.cuda.is_available():
            torch.cuda.synchronize()
        end_time = time.time()
        times.append((end_time - start_time) * 1000)
    return mean(times)
```

**常见误用 -> 正确写法**
- 误用：省略 `torch.cuda.synchronize()` 直接计时。
  正确：计时窗口前后都同步，测真实 GPU 执行时间。
- 误用：只测 1 次就下结论。
  正确：至少 warmup + 多次 trial，报告均值/方差。

### 直觉解释

- 不预热：把冷启动当成稳态性能；
- 不同步：把异步提交当成执行时间；
- 只测一次：被抖动误导。

---

## 3. Profiling：把“慢”拆成可解释的账本

通过 `add / matmul / cdist / gelu / softmax / MLP` 这类算子可观察到：

- 小算子常被 launch + sync 比例支配；
- 大 GEMM 常由 kernel 本身主导；
- 复合算子（如 `cdist`）会分解为多个子算子，瓶颈通常集中在主算子（如 matmul）。

### 常见结论

- CPU 时间高不代表 GPU 慢，可能是调度路径重；
- CUDA 时间高才说明核心算子真重；
- 看到多个小 kernel 连续触发，通常意味着融合潜力大。

### 3.1 代码层最小 profiling 模板

```python
def profile(description: str, run: Callable, num_warmups: int = 1, with_stack: bool = False):
    # 预热：让 kernel 缓存、频率状态进入稳定区间
    for _ in range(num_warmups):
        run()
    if torch.cuda.is_available():
        # 同步到统一起点，避免把旧队列任务算进本次 profiling
        torch.cuda.synchronize()

    with torch.profiler.profile(
        # 同时采集 CPU/CUDA，便于判断“调度热”还是“内核热”
        activities=[ProfilerActivity.CPU, ProfilerActivity.CUDA],
        # 栈信息可帮助追溯到 Python 调用点；代价是额外开销
        with_stack=with_stack,
    ) as prof:
        run()
        if torch.cuda.is_available():
            # 确保 run() 内所有异步 GPU 任务执行完成再出 profiler 上下文
            torch.cuda.synchronize()

    # 常见看法：先按 cuda_time_total 排，找 GPU 热点；再看 CPU 开销
    table = prof.key_averages().table(
        sort_by="cuda_time_total",
        max_name_column_width=80,
        row_limit=10,
    )
    return table
```

**常见误用 -> 正确写法**
- 误用：只开 `CPU` activity，误判瓶颈。
  正确：同时开 `CPU + CUDA`，交叉看调度与内核时间。
- 误用：profiling 期间不做同步。
  正确：退出 profiler 前同步，避免漏记异步 kernel。

这段代码的意义是把“端到端慢”拆解成“谁在耗时”。常见观察点：

- `aten::xxx` 时间高：上层算子热；
- 某个 CUDA kernel 时间高：底层内核热；
- `cudaDeviceSynchronize` 比例异常：同步点可能过重或放错位置。

### 3.2 MLP 基准块（原文代码块补全）

```python
class MLP(nn.Module):
    def __init__(self, dim: int, num_layers: int):
        super().__init__()
        # 用等宽 MLP 形成“稳定、可控”的基准负载
        self.layers = nn.ModuleList([nn.Linear(dim, dim) for _ in range(num_layers)])

    def forward(self, x: torch.Tensor):
        for layer in self.layers:
            x = layer(x)
            x = torch.nn.functional.gelu(x)
        return x

def run_mlp(dim: int, num_layers: int, batch_size: int, num_steps: int) -> Callable:
    # 模型和输入在构造阶段固定，避免把数据准备开销混入每次 run()
    model = MLP(dim, num_layers).to(get_device())
    x = torch.randn(batch_size, dim, device=get_device())

    def run():
        for _ in range(num_steps):
            # 用 mean 形成标量损失，保证 backward 可执行
            y = model(x).mean()
            # 注意：这里没有 zero_grad，会累计梯度，适合“纯算子路径”演示
            y.backward()
    return run
```

**常见误用 -> 正确写法**
- 误用：忘记 `zero_grad` 却拿长期结果对比。
  正确：若做长循环基准，显式 `zero_grad(set_to_none=True)`。
- 误用：把数据构造放到 `run()` 里。
  正确：把固定输入放外面，只测训练路径本身。

这个代码块的意义是：把“真实训练路径（前向 + 反向）”放进 benchmark，而不是只测单个算子。

**idea**
- 用最小可复现训练环路暴露真实瓶颈（前向算子、反向算子、同步行为）。

**工程注意**
- 如果用于长期循环基准，建议显式 `zero_grad(set_to_none=True)`，否则梯度累积会抬升显存压力。
- 若要比较不同实现，保证 `dim/num_layers/batch_size/num_steps` 完全一致。

### 3.3 通用算子基准块（原文代码块补全）

```python
def run_operation2(dim: int, operation: Callable) -> Callable:
    # 固定输入，隔离“算子差异”，减少数据生成噪声
    x = torch.randn(dim, dim, device=get_device())
    y = torch.randn(dim, dim, device=get_device())
    return lambda: operation(x, y)

add_profile = profile("add", run_operation2(dim=2048, operation=lambda a, b: a + b))
matmul_profile = profile("matmul", run_operation2(dim=2048, operation=lambda a, b: a @ b))
cdist_profile = profile("cdist", run_operation2(dim=2048, operation=lambda a, b: torch.cdist(a, b)))
```

**常见误用 -> 正确写法**
- 误用：不同算子用不同输入尺寸比较。
  正确：固定 shape 和 dtype，仅改变算子类型。
- 误用：小尺寸直接外推到大模型场景。
  正确：覆盖多档尺寸，观察曲线再定优化策略。

原文这组代码强调：同一个 profiling 框架可横向比较不同算子，便于定位“谁是真热点”。

**idea**
- 把“算子本身”作为自变量，统一输入规模与 profiling 口径。

**工程注意**
- 维度太小会被 launch 开销主导，看不出算子真实差异。
- 维度太大可能触发 OOM 或热降频，建议先扫一圈维度再定基准点。

---

## 4. GeLU 案例：为什么“数学等价”不等于“性能等价”

### 4.1 公式

$$
\text{GeLU}(x)\approx 0.5x\left(1+\tanh\left(\sqrt{2/\pi}(x+0.044715x^3)\right)\right)
$$

### 4.2 性能差异来源

- `manual_gelu` 常被拆成多步逐元素操作；
- 每步都可能触发独立 kernel；
- 中间结果频繁写回 HBM，再读回。

这导致大量“非必要搬运”，即使 FLOPs 不高，也会变慢。

### 4.3 对照代码：手写 GeLU vs 内置 GeLU

```python
def pytorch_gelu(x: torch.Tensor):
    # 与手写式对齐，使用 tanh 近似版本
    return torch.nn.functional.gelu(x, approximate="tanh")

def manual_gelu(x: torch.Tensor):
    # 展开公式，便于观察 eager 下是否被拆成多个 kernel
    return 0.5 * x * (1 + torch.tanh(0.79788456 * (x + 0.044715 * x * x * x)))
```

**常见误用 -> 正确写法**
- 误用：只比输出相等，不看性能路径。
  正确：`check_equal` 后必须配合 profiler/benchmark。
- 误用：默认 `gelu` 近似方式不同。
  正确：显式设置 `approximate="tanh"` 再做公平对比。

两者数值可非常接近，但执行路径不同。  
优化时要优先看 profiler，而不是只看公式形式。

### 4.4 大尺寸基准测试块（原文代码块补全）

```python
def run_operation1(dim: int, operation: Callable) -> Callable:
    # 单输入逐元素算子常见测试模板
    x = torch.randn(dim, dim, device=get_device())
    return lambda: operation(x)

manual_time = benchmark("manual_gelu", run_operation1(dim=16384, operation=manual_gelu))
pytorch_time = benchmark("pytorch_gelu", run_operation1(dim=16384, operation=pytorch_gelu))
```

**常见误用 -> 正确写法**
- 误用：只测小矩阵得出“手写一样快”。
  正确：用接近实战的大尺寸才暴露访存差异。
- 误用：不同 dtype 混测。
  正确：统一 dtype 后再比较时间。

这段对应原文“为什么手写慢”的实证部分：不是凭感觉，而是大尺寸下直接测耗时差。

**工程注意**
- 逐元素算子对访存模式敏感，建议记录 dtype（fp32/fp16/bf16）后再比较结论。
- 若对比跨后端（eager/compile/triton/cuda），请固定随机种子和设备频率策略。

---

## 5. 内核融合（Kernel Fusion）

GeLU 对比本质在复现一个常见工程事实：

- **不融合**：多次 kernel launch + 多次 global memory 往返；
- **融合后**：一次 kernel 内完成更多计算，中间值停留在寄存器/片上缓存。

所以“快”的来源更多是 IO 路径变短，而不是公式换了。

---

## 6. CUDA C++ 手写内核：关键细节与常见坑

### 6.1 典型逐元素 kernel

```cpp
__global__ void gelu_kernel(float* in, float* out, int num_elements) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < num_elements) {
        float x = in[i];
        out[i] = 0.5f * x * (1.0f + tanhf(0.79788456f * (x + 0.044715f * x * x * x)));
    }
}
```

**常见误用 -> 正确写法**
- 误用：不做 `i < num_elements` 边界判断。
  正确：始终用 mask/bounds check 防越界。
- 误用：把 host 指针当 device 指针传入。
  正确：调用前确认 tensor 在 CUDA 且 contiguous。

### 6.2 包装函数要点

- 输入必须在 CUDA 设备上；
- 输入应连续存储（contiguous）；
- 用 `empty_like` 分配输出，避免不必要初始化；
- `num_blocks = ceil_div(num_elements, block_size)`；
- 启动后做 `C10_CUDA_KERNEL_LAUNCH_CHECK()`。

### 6.4 Python 侧动态编译与绑定（原文代码块补全）

```python
from torch.utils.cpp_extension import load_inline

def create_cuda_gelu():
    # 读取 .cu 源码，便于和 Python 训练脚本一起管理
    cuda_gelu_src = open("gelu.cu").read()
    # 声明 C++ 侧函数签名，供 pybind 绑定
    cpp_gelu_src = "torch::Tensor gelu(torch::Tensor x);"

    if not torch.cuda.is_available():
        return None

    module = load_inline(
        # CUDA/C++ 源分离传入
        cuda_sources=[cuda_gelu_src],
        cpp_sources=[cpp_gelu_src],
        # 需要导出的函数名
        functions=["gelu"],
        # 编译优化等级
        extra_cflags=["-O2"],
        verbose=True,
        name="inline_gelu",
        # 固定构建目录，便于复用编译产物
        build_directory="var/cuda_gelu",
    )
    # 返回 Python 可调用句柄
    return getattr(module, "gelu")
```

**常见误用 -> 正确写法**
- 误用：每次运行都重新编译 extension。
  正确：固定 `name/build_directory` 复用编译缓存。
- 误用：忘记 CPU 回退逻辑。
  正确：`torch.cuda.is_available()` 为假时返回 `None` 或 fallback。

这个块对应“从内核代码到 Python 可调用函数”的桥接步骤，是手写 CUDA 实战里最容易缺失的一环。

### 6.3 一个常见错误

内核里直接用未定义变量 `x`，而不是 `float x = in[i]`。  
这个错误在教学代码里很常见，实际写 kernel 时要优先检查。

---

## 7. Triton：为什么很多团队把它当“首选自定义内核层”

Triton 的优势不是绝对速度，而是开发效率：

- Python 语法；
- 以 block/program 为中心；
- 避免手工管理大量线程细节；
- 对常见模式（逐元素、块级操作）表达更直接。

### Triton GeLU 的核心流程

1. 用 `program_id` 定位当前 block；
2. 构造 offsets + mask；
3. `tl.load` 向量化读入；
4. 在块内完成 GeLU 计算；
5. `tl.store` 写回。

这和 CUDA 的逻辑等价，但心智负担更低。

### 7.1 Triton GeLU 示例（核心片段）

```python
@triton.jit
def triton_gelu_kernel(x_ptr, y_ptr, num_elements, BLOCK_SIZE: tl.constexpr):
    # 当前 program（块）编号
    pid = tl.program_id(axis=0)
    # 当前块起始偏移
    block_start = pid * BLOCK_SIZE
    # 块内连续 offsets
    offsets = block_start + tl.arange(0, BLOCK_SIZE)
    # 边界保护，避免越界访问
    mask = offsets < num_elements

    # 向量化读入
    x = tl.load(x_ptr + offsets, mask=mask)
    # tanh 近似 GeLU 核心计算
    a = 0.79788456 * (x + 0.044715 * x * x * x)
    exp = tl.exp(2 * a)
    tanh = (exp - 1) / (exp + 1)
    y = 0.5 * x * (1 + tanh)
    # 向量化写回
    tl.store(y_ptr + offsets, y, mask=mask)
```

**常见误用 -> 正确写法**
- 误用：省略 `mask`，尾块越界访问。
  正确：尾块统一走 `mask` 保护读写。
- 误用：`BLOCK_SIZE` 随意设置。
  正确：结合 occupancy 与寄存器压力做 sweep。

关键点：

- `tl.arange` 本质是块内向量化索引；
- `mask` 负责边界安全；
- kernel 内一次性完成读/算/写，减少中间往返。

---

## 8. `torch.compile` 的定位

很多中等复杂度优化场景里，`torch.compile` 已经很有性价比。

建议策略：

1. 先尝试 `torch.compile`（低开发成本）；
2. profile 后若热点仍明显，再 Triton；
3. 最后 CUDA C++ 做硬件特化。

### 8.1 最小用法

```python
compiled_gelu = torch.compile(manual_gelu)
```

**常见误用 -> 正确写法**
- 误用：把首次编译时间算进稳态吞吐。
  正确：分开统计 cold-start 与 steady-state。
- 误用：动态 shape 太多还期待稳定收益。
  正确：优先在 shape 相对稳定场景验证。

常见收益来源：

- 自动融合若干逐元素算子；
- 减少 launch 次数；
- 在固定形状场景可能自动选更优执行路径。

### 8.2 原文对应的最小验证块（补全）

```python
compiled_gelu = torch.compile(manual_gelu)
check_equal(compiled_gelu, manual_gelu)
```

**常见误用 -> 正确写法**
- 误用：编译后直接替换线上路径。
  正确：先做 `check_equal`，再灰度比性能。
- 误用：只测均值。
  正确：同时记录 p95/p99，防止抖动掩盖问题。

这段是原文里的关键实践点：先验正确性，再谈性能收益。

**工程注意**
- `torch.compile` 首次调用包含图捕获/编译成本，benchmark 要分开统计“首次”与“稳态”。
- 动态 shape 过多时收益会波动，建议在稳定 shape 下评估峰值收益。

---

## 9. Nsight Systems 的意义（为什么还要学）

`torch.profiler` 能看很多，但复杂流水时仍不够直观。  
Nsight 的价值是把 CPU 线程、CUDA kernel、通信和等待放在统一时间轴上，让“谁在等谁”可视化。

适合排查：

- 内核间空泡；
- CPU launch 是否跟不上；
- 通信与计算是否重叠不足；
- 初始化阶段是否过重。

---

## 10. 可执行工作流（落地版）

1. 明确目标（吞吐/延迟）；
2. 先 benchmark 建基线；
3. profiler 定位 top hotspots；
4. 判断是 compute-bound 还是 memory/launch-bound；
5. 选手段（compile -> Triton -> CUDA）；
6. 回归 benchmark 做前后对比；
7. 保留 Nsight/Profiler 截图作为证据。

---

## 11. 一句话总结

重点不是“记住一个快公式”，而是掌握一套可靠的性能工程流程：  
**先证据，再优化；优化后必须复测。**

