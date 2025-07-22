# Multi-RTSP YOLO Batch Processing

这个脚本可以同时处理多个 RTSP 流，使用 YOLO 进行批量推理，自动处理流中断和 batch size 变化。

## 主要特性

- 🎯 **批量 YOLO 推理**: 将多个流的帧组成 batch 进行加速推理
- 🔄 **自动重连**: 自动处理 RTSP 流中断和重新连接
- 📊 **动态 Batch**: 根据可用流动态调整 batch size
- 🖼️ **网格显示**: 在一个窗口中显示所有流的检测结果
- 📈 **性能统计**: 实时显示 FPS、批量大小等统计信息
- 🎨 **结果映射**: 正确地将 YOLO 检测结果绘制到对应的流上

## 依赖安装

```bash
# 安装基本依赖
pip install opencv-python ultralytics numpy

# 可选：如果需要虚拟摄像头功能
pip install pyvirtualcam

# 可选：如果需要HTTP流媒体功能
pip install flask
```

## 使用方法

### 基本用法

```bash
# 使用默认RTSP URL进行测试（会自动创建4个流）
python multi_rtsp_yolo_batch.py

# 指定单个RTSP URL
python multi_rtsp_yolo_batch.py --rtsp-urls rtsp://your-camera-url

# 指定多个RTSP URLs
python multi_rtsp_yolo_batch.py --rtsp-urls \
    rtsp://camera1-url \
    rtsp://camera2-url \
    rtsp://camera3-url \
    rtsp://camera4-url
```

### 高级参数

```bash
python multi_rtsp_yolo_batch.py \
    --rtsp-urls rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast \
    --model yolov8s.pt \
    --batch-size 8 \
    --width 640 \
    --height 480 \
    --confidence 0.6 \
    --grid-cols 3 \
    --no-display
```

### 参数说明

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `--rtsp-urls` | 测试 URL | RTSP 流 URLs 列表 |
| `--model` | yolov8n.pt | YOLO 模型路径 |
| `--batch-size` | 4 | 最大 batch 大小 |
| `--width` | 640 | 帧宽度 |
| `--height` | 480 | 帧高度 |
| `--confidence` | 0.5 | 置信度阈值 |
| `--grid-cols` | 2 | 显示网格列数 |
| `--no-display` | False | 禁用显示窗口 |

## 核心功能详解

### 1. RTSPStreamHandler - 单流处理器

每个 RTSP 流都有一个独立的处理器：
- **自动重连**: 流中断时自动尝试重连（最多 5 次）
- **帧队列**: 使用队列缓存最新帧，避免延迟累积
- **状态监控**: 实时监控连接状态和帧时间戳

### 2. MultiStreamYOLOProcessor - 批量处理器

负责协调多个流并进行批量推理：
- **动态 Batch**: 收集当前可用的所有帧组成 batch
- **结果映射**: 确保每个推理结果正确对应到原始流
- **性能统计**: 跟踪 FPS、batch 大小等关键指标

### 3. MultiStreamDisplay - 网格显示

将多个流结果显示在网格布局中：
- **网格布局**: 自动计算最佳网格尺寸
- **状态显示**: 显示流状态和检测统计
- **交互控制**: 支持按键保存截图和退出

## 流中断处理

脚本具有强大的流中断恢复能力：

1. **检测中断**: 监控`cap.read()`返回状态
2. **自动重连**: 流中断时自动释放资源并重新连接
3. **动态 Batch**: batch size 会根据活跃流数量动态调整
4. **状态同步**: 所有组件都能正确处理流状态变化

## 性能优化

### 批量推理加速

```python
# 传统方式：每个流单独推理
for stream in streams:
    result = model(frame)  # 4 次推理调用

# 批量方式：一次推理处理多帧
results = model([frame1, frame2, frame3, frame4])  # 1 次推理调用
```

### 内存管理

- 队列大小限制防止内存溢出
- 及时释放 OpenCV 资源
- 使用线程安全的结果存储

### 延迟控制

- 设置`CAP_PROP_BUFFERSIZE=1`减少缓冲
- 丢弃旧帧保持实时性
- 非阻塞队列操作

## 故障排除

### 常见问题

1. **无法连接 RTSP 流**
   ```
   Stream 0: 连接失败，1/5
   ```
   - 检查网络连接
   - 验证 RTSP URL 格式
   - 确认用户名和密码

2. **显示窗口创建失败**
   ```
   显示窗口创建失败：...
   ```
   - 在有 GUI 的环境中运行
   - 或使用`--no-display`参数

3. **YOLO 模型加载失败**
   ```
   加载 YOLO 模型：yolov8n.pt
   ```
   - 确保模型文件存在
   - 检查 ultralytics 安装

### 调试模式

```bash
# 启用详细日志
export PYTHONPATH=$PYTHONPATH:.
python multi_rtsp_yolo_batch.py --rtsp-urls your-url 2>&1 | tee debug.log
```

## 扩展功能

### 添加新的输出方式

可以在`MultiStreamDisplay`类基础上添加：
- HTTP 流媒体输出
- 视频文件保存
- 虚拟摄像头输出

### 自定义检测后处理

在`process_batch`方法中添加：
- 目标跟踪
- 区域入侵检测  
- 报警机制

## 示例输出

```
2024-01-15 10:30:15,123 - __main__ - INFO - 添加流 0: rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast
2024-01-15 10:30:15,124 - __main__ - INFO - 使用单个RTSP URL创建多个测试流
2024-01-15 10:30:15,124 - __main__ - INFO - 添加流 1: rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast
2024-01-15 10:30:15,124 - __main__ - INFO - 添加流 2: rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast
2024-01-15 10:30:15,125 - __main__ - INFO - 添加流 3: rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast
2024-01-15 10:30:15,125 - __main__ - INFO - 启动所有RTSP流...
2024-01-15 10:30:17,234 - __main__ - INFO - 活跃流数量: 4/4
2024-01-15 10:30:17,235 - __main__ - INFO - 启动多流显示窗口 (按 'q' 退出, 's' 保存截图)
2024-01-15 10:30:17,456 - __main__ - INFO - 开始YOLO批量处理...
```

## 键盘控制

在显示窗口中：
- `q`: 退出程序
- `s`: 保存当前截图

## 注意事项

1. **网络带宽**: 多个高分辨率流需要足够的网络带宽
2. **计算资源**: 批量推理需要足够的 GPU/CPU 资源
3. **同步问题**: 不同流的帧率可能不同，会影响 batch 组成
4. **内存使用**: 监控内存使用，避免系统负载过高