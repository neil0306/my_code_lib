#!/usr/bin/env python3
"""
Multi-RTSP YOLO Batch Processing Stream
处理多个 RTSP 流，使用 YOLO 进行批量推理，支持流中断恢复
"""

import cv2
import numpy as np
import threading
import time
import argparse
import sys
from queue import Queue, Empty
from collections import defaultdict
from typing import List, Dict, Tuple, Optional
from ultralytics import YOLO
import logging

# 设置日志
logging.basicConfig(level=logging.INFO, 
                   format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class RTSPStreamHandler:
    """单个 RTSP 流处理器"""
    
    def __init__(self, stream_id: int, rtsp_url: str, target_size: Tuple[int, int] = (640, 480)):
        self.stream_id = stream_id
        self.rtsp_url = rtsp_url
        self.target_size = target_size  # (width, height)
        self.frame_queue = Queue(maxsize=5)  # 限制队列大小以减少延迟
        self.running = False
        self.connected = False
        self.last_frame_time = 0
        self.reconnect_attempts = 0
        self.max_reconnect_attempts = 5
        
    def start(self):
        """启动流捕获"""
        self.running = True
        self.capture_thread = threading.Thread(target=self._capture_frames, daemon=True)
        self.capture_thread.start()
        
    def stop(self):
        """停止流捕获"""
        self.running = False
        
    def get_frame(self) -> Optional[np.ndarray]:
        """获取最新帧"""
        try:
            # 获取队列中最新的帧，丢弃旧帧
            frame = None
            while not self.frame_queue.empty():
                try:
                    frame = self.frame_queue.get_nowait()
                except Empty:
                    break
            return frame
        except:
            return None
            
    def _capture_frames(self):
        """帧捕获线程"""
        cap = None
        
        while self.running:
            try:
                if cap is None or not cap.isOpened():
                    # 尝试连接 RTSP 流
                    logger.info(f"Stream {self.stream_id}: 尝试连接 {self.rtsp_url}")
                    cap = cv2.VideoCapture(self.rtsp_url)
                    cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)  # 减少缓冲延迟
                    
                    if cap.isOpened():
                        self.connected = True
                        self.reconnect_attempts = 0
                        logger.info(f"Stream {self.stream_id}: 连接成功")
                    else:
                        self.connected = False
                        self.reconnect_attempts += 1
                        if self.reconnect_attempts <= self.max_reconnect_attempts:
                            logger.warning(f"Stream {self.stream_id}: 连接失败，{self.reconnect_attempts}/{self.max_reconnect_attempts}")
                            time.sleep(2)  # 等待后重试
                            continue
                        else:
                            logger.error(f"Stream {self.stream_id}: 超过最大重连次数，跳过此流")
                            break
                
                ret, frame = cap.read()
                if not ret:
                    logger.warning(f"Stream {self.stream_id}: 读取帧失败，尝试重连")
                    self.connected = False
                    cap.release()
                    cap = None
                    time.sleep(1)
                    continue
                
                # 调整帧大小
                if frame.shape[:2] != (self.target_size[1], self.target_size[0]):
                    frame = cv2.resize(frame, self.target_size)
                
                # 更新时间戳
                self.last_frame_time = time.time()
                
                # 将帧放入队列（非阻塞）
                try:
                    if self.frame_queue.full():
                        # 如果队列满了，丢弃最旧的帧
                        try:
                            self.frame_queue.get_nowait()
                        except Empty:
                            pass
                    self.frame_queue.put(frame, block=False)
                except:
                    pass  # 队列操作失败，继续下一帧
                    
            except Exception as e:
                logger.error(f"Stream {self.stream_id}: 捕获错误 {e}")
                self.connected = False
                if cap:
                    cap.release()
                cap = None
                time.sleep(1)
                
        # 清理资源
        if cap:
            cap.release()
        logger.info(f"Stream {self.stream_id}: 捕获线程结束")


class MultiStreamYOLOProcessor:
    """多流 YOLO 批量处理器"""
    
    def __init__(self, model_path: str = "YOLO11M_100_re.pt", max_batch_size: int = 8, 
                 target_size: Tuple[int, int] = (640, 480), confidence_threshold: float = 0.5,
                 enable_detection: bool = True):
        self.model_path = model_path
        self.max_batch_size = max_batch_size
        self.target_size = target_size
        self.confidence_threshold = confidence_threshold
        self.enable_detection = enable_detection
        
        # 只有在启用检测时才加载 YOLO 模型
        if self.enable_detection:
            logger.info(f"加载 YOLO 模型：{model_path}")
            self.model = YOLO(model_path)
            logger.info("YOLO 模型加载完成")
        else:
            self.model = None
            logger.info("YOLO 检测已禁用，将直接显示原始视频流")
        
        # 流处理器
        self.streams: Dict[int, RTSPStreamHandler] = {}
        self.running = False
        
        # 结果存储
        self.processed_frames: Dict[int, np.ndarray] = {}
        self.results_lock = threading.Lock()
        
        # 性能统计
        self.stats = {
            'total_frames': 0,
            'batch_count': 0,
            'avg_batch_size': 0,
            'fps': 0,
            'start_time': 0
        }
        
    def add_stream(self, stream_id: int, rtsp_url: str):
        """添加 RTSP 流"""
        if stream_id in self.streams:
            logger.warning(f"Stream {stream_id} 已存在，将被替换")
            
        self.streams[stream_id] = RTSPStreamHandler(stream_id, rtsp_url, self.target_size)
        logger.info(f"添加流 {stream_id}: {rtsp_url}")
        
    def start_all_streams(self):
        """启动所有流"""
        logger.info("启动所有 RTSP 流...")
        for stream in self.streams.values():
            stream.start()
            
        # 等待流连接
        time.sleep(2)
        active_streams = sum(1 for stream in self.streams.values() if stream.connected)
        logger.info(f"活跃流数量：{active_streams}/{len(self.streams)}")
        
    def stop_all_streams(self):
        """停止所有流"""
        logger.info("停止所有流...")
        for stream in self.streams.values():
            stream.stop()
            
    def collect_batch_frames(self) -> Tuple[List[np.ndarray], List[int]]:
        """收集批量帧数据"""
        batch_frames = []
        stream_ids = []
        
        for stream_id, stream in self.streams.items():
            if stream.connected:
                frame = stream.get_frame()
                if frame is not None:
                    batch_frames.append(frame)
                    stream_ids.append(stream_id)
                    
                    # 限制批量大小
                    if len(batch_frames) >= self.max_batch_size:
                        break
                        
        return batch_frames, stream_ids
        
    def process_batch(self, frames: List[np.ndarray], stream_ids: List[int]):
        """批量处理帧"""
        if not frames:
            return
            
        try:
            batch_start_time = time.time()
            
            if self.enable_detection and self.model is not None:
                # YOLO 批量推理
                results = self.model(frames, verbose=False, conf=self.confidence_threshold)
                
                inference_time = time.time() - batch_start_time
                
                # 处理结果并绘制
                total_detections = 0
                with self.results_lock:
                    for i, (frame, stream_id, result) in enumerate(zip(frames, stream_ids, results)):
                        # 绘制检测结果
                        annotated_frame = result.plot()
                        
                        # 计算检测数量
                        detection_count = len(result.boxes) if result.boxes is not None else 0
                        total_detections += detection_count
                        
                        # 添加流信息
                        cv2.putText(annotated_frame, f"Stream {stream_id}", 
                                  (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
                        
                        # 添加检测统计
                        cv2.putText(annotated_frame, f"Objects: {detection_count}", 
                                    (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
                        
                        # 添加批次信息
                        cv2.putText(annotated_frame, f"Batch: {self.stats['batch_count'] + 1}", 
                                    (10, 110), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
                        
                        # 添加推理时间
                        cv2.putText(annotated_frame, f"Inference: {inference_time*1000:.1f}ms", 
                                    (10, 150), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
                        
                        self.processed_frames[stream_id] = annotated_frame
            else:
                # 不进行检测，直接显示原始帧
                inference_time = time.time() - batch_start_time
                total_detections = 0
                
                with self.results_lock:
                    for i, (frame, stream_id) in enumerate(zip(frames, stream_ids)):
                        # 直接使用原始帧
                        display_frame = frame.copy()
                        
                        # 添加流信息
                        cv2.putText(display_frame, f"Stream {stream_id}", 
                                  (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
                        
                        # 添加无检测标识
                        cv2.putText(display_frame, "Detection: OFF", 
                                    (10, 70), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
                        
                        # 添加批次信息
                        cv2.putText(display_frame, f"Batch: {self.stats['batch_count'] + 1}", 
                                    (10, 110), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
                        
                        # 添加处理时间
                        cv2.putText(display_frame, f"Process: {inference_time*1000:.1f}ms", 
                                    (10, 150), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
                        
                        self.processed_frames[stream_id] = display_frame
                        
            # 更新统计信息
            self.stats['total_frames'] += len(frames)
            self.stats['batch_count'] += 1
            self.stats['avg_batch_size'] = self.stats['total_frames'] / self.stats['batch_count']
            
            # 第一批次或每 50 批次输出详细信息
            if self.stats['batch_count'] == 1 or self.stats['batch_count'] % 50 == 0:
                if self.enable_detection:
                    logger.info(f"批次 #{self.stats['batch_count']}: {len(frames)} 帧，{total_detections} 个检测，"
                               f"推理时间：{inference_time*1000:.1f}ms, 流：{stream_ids}")
                else:
                    logger.info(f"批次 #{self.stats['batch_count']}: {len(frames)} 帧，无检测模式，"
                               f"处理时间：{inference_time*1000:.1f}ms, 流：{stream_ids}")
            
        except Exception as e:
            logger.error(f"批量处理错误：{e}")
            import traceback
            traceback.print_exc()
            
    def start_processing(self):
        """开始处理循环"""
        self.running = True
        self.stats['start_time'] = time.time()
        
        logger.info("开始 YOLO 批量处理...")
        
        # 等待至少一个流连接
        wait_timeout = 30  # 30 秒超时
        start_wait = time.time()
        while self.running and time.time() - start_wait < wait_timeout:
            active_streams = sum(1 for stream in self.streams.values() if stream.connected)
            if active_streams > 0:
                logger.info(f"检测到 {active_streams} 个活跃流，开始处理")
                break
            time.sleep(0.5)
        else:
            logger.error("等待活跃流超时，停止处理")
            return
        
        no_frame_count = 0
        last_log_time = time.time()
        
        try:
            while self.running:
                # 收集批量帧
                batch_frames, stream_ids = self.collect_batch_frames()
                
                if batch_frames:
                    # 处理批量
                    self.process_batch(batch_frames, stream_ids)
                    no_frame_count = 0
                    
                    # 定期日志输出
                    current_time = time.time()
                    if current_time - last_log_time > 10:  # 每 10 秒输出一次
                        active_streams = sum(1 for stream in self.streams.values() if stream.connected)
                        logger.info(f"处理状态 - 活跃流：{active_streams}, 批次：{self.stats['batch_count']}, "
                                  f"批量大小：{len(batch_frames)}, FPS: {self.stats['fps']:.1f}")
                        last_log_time = current_time
                        
                else:
                    # 如果没有帧，稍作等待
                    no_frame_count += 1
                    if no_frame_count % 1000 == 0:  # 每 1000 次无帧时输出警告
                        active_streams = sum(1 for stream in self.streams.values() if stream.connected)
                        logger.warning(f"连续 {no_frame_count} 次无帧，活跃流数：{active_streams}")
                    time.sleep(0.05)  # 增加等待时间减少 CPU 占用
                    
                # 更新 FPS 统计
                elapsed = time.time() - self.stats['start_time']
                if elapsed > 0:
                    self.stats['fps'] = self.stats['total_frames'] / elapsed
                    
        except KeyboardInterrupt:
            logger.info("收到停止信号")
        except Exception as e:
            logger.error(f"处理循环错误：{e}")
            import traceback
            traceback.print_exc()
        finally:
            self.running = False
            logger.info("处理循环结束")
            
    def stop_processing(self):
        """停止处理"""
        self.running = False
        
    def get_processed_frame(self, stream_id: int) -> Optional[np.ndarray]:
        """获取处理后的帧"""
        with self.results_lock:
            return self.processed_frames.get(stream_id, None)
            
    def print_stats(self):
        """打印统计信息"""
        elapsed = time.time() - self.stats['start_time']
        logger.info(f"处理统计：")
        logger.info(f"  总帧数：{self.stats['total_frames']}")
        logger.info(f"  批次数：{self.stats['batch_count']}")
        logger.info(f"  平均批量大小：{self.stats['avg_batch_size']:.1f}")
        logger.info(f"  处理 FPS: {self.stats['fps']:.1f}")
        logger.info(f"  运行时间：{elapsed:.1f}s")


class MultiStreamDisplay:
    """多流显示器"""
    
    def __init__(self, processor: MultiStreamYOLOProcessor, grid_cols: int = 2):
        self.processor = processor
        self.grid_cols = grid_cols
        self.running = False
        
    def create_grid_display(self) -> Optional[np.ndarray]:
        """创建网格显示"""
        stream_ids = list(self.processor.streams.keys())
        if not stream_ids:
            return None
            
        # 计算网格大小
        grid_rows = (len(stream_ids) + self.grid_cols - 1) // self.grid_cols
        
        # 获取单个流的尺寸
        frame_height, frame_width = self.processor.target_size[1], self.processor.target_size[0]
        
        # 创建网格画布
        grid_height = grid_rows * frame_height
        grid_width = self.grid_cols * frame_width
        grid_frame = np.zeros((grid_height, grid_width, 3), dtype=np.uint8)
        
        # 填充网格
        for i, stream_id in enumerate(stream_ids):
            row = i // self.grid_cols
            col = i % self.grid_cols
            
            # 获取处理后的帧
            frame = self.processor.get_processed_frame(stream_id)
            
            if frame is None:
                # 创建占位符
                frame = np.zeros((frame_height, frame_width, 3), dtype=np.uint8)
                cv2.putText(frame, f"Stream {stream_id}", 
                          (50, frame_height//2), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
                cv2.putText(frame, "No Signal", 
                          (50, frame_height//2 + 40), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
            
            # 确保帧尺寸正确
            if frame.shape[:2] != (frame_height, frame_width):
                frame = cv2.resize(frame, (frame_width, frame_height))
            
            # 放置到网格中
            y1, y2 = row * frame_height, (row + 1) * frame_height
            x1, x2 = col * frame_width, (col + 1) * frame_width
            grid_frame[y1:y2, x1:x2] = frame
            
        # 添加全局信息
        info_text = f"Streams: {len(stream_ids)} | FPS: {self.processor.stats['fps']:.1f} | Batch: {self.processor.stats['avg_batch_size']:.1f}"
        cv2.putText(grid_frame, info_text, (10, grid_height - 20), 
                   cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 2)
        
        return grid_frame
        
    def start_display(self):
        """开始显示"""
        self.running = True
        window_name = "Multi-Stream YOLO Detection"
        
        # 强制在主线程中初始化 GUI
        try:
            # 先等一下，让其他初始化完成
            time.sleep(0.5)
            
            # 检查是否在主线程
            import threading
            if threading.current_thread() is not threading.main_thread():
                logger.warning("GUI 在非主线程中运行，这可能导致问题")
            
            # 尝试不同的窗口标志
            cv2.namedWindow(window_name, cv2.WINDOW_NORMAL | cv2.WINDOW_KEEPRATIO)
            cv2.resizeWindow(window_name, 800, 600)
            logger.info("✓ 显示窗口创建成功")
            
            # 测试显示功能
            test_img = np.zeros((100, 300, 3), dtype=np.uint8)
            cv2.putText(test_img, "Initializing...", (10, 50), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
            cv2.imshow(window_name, test_img)
            cv2.waitKey(100)  # 给更多时间
            logger.info("✓ 显示测试成功")
            
        except Exception as e:
            logger.error(f"✗ 无法创建显示窗口：{e}")
            logger.error("具体错误类型：", type(e).__name__)
            # 尝试使用无显示模式继续运行
            logger.info("自动切换到无显示模式...")
            self.running = False
            return
        
        # 等待处理器开始工作
        logger.info("等待处理器开始工作...")
        wait_count = 0
        while self.processor.stats['total_frames'] == 0 and wait_count < 100:
            time.sleep(0.1)
            wait_count += 1
            
        if wait_count >= 100:
            logger.warning("等待处理器超时，可能没有有效的视频流")
        
        try:
            frame_count = 0
            while self.running and self.processor.running:
                grid_frame = self.create_grid_display()
                
                if grid_frame is not None:
                    try:
                        cv2.imshow(window_name, grid_frame)
                        frame_count += 1
                        
                        # 每 100 帧打印一次状态
                        if frame_count % 100 == 0:
                            logger.info(f"显示帧数：{frame_count}, 处理 FPS: {self.processor.stats['fps']:.1f}")
                            
                    except cv2.error as e:
                        logger.error(f"显示帧失败：{e}")
                        break
                        
                key = cv2.waitKey(1) & 0xFF
                if key == ord('q'):
                    logger.info("用户退出显示")
                    self.processor.stop_processing()
                    break
                elif key == ord('s'):
                    # 保存截图
                    timestamp = time.strftime("%Y%m%d_%H%M%S")
                    filename = f"multi_stream_screenshot_{timestamp}.jpg"
                    if grid_frame is not None:
                        cv2.imwrite(filename, grid_frame)
                        logger.info(f"截图保存为：{filename}")
                    
                time.sleep(0.033)  # 约 30FPS 显示率
                
        except KeyboardInterrupt:
            logger.info("显示被用户中断")
        except Exception as e:
            logger.error(f"显示错误：{e}")
            import traceback
            traceback.print_exc()
        finally:
            try:
                cv2.destroyAllWindows()
            except:
                pass
            self.running = False
            logger.info("显示窗口已关闭")


def main():
    parser = argparse.ArgumentParser(description='Multi-RTSP YOLO Batch Processing')
    parser.add_argument('--rtsp-urls', type=str, nargs='+', 
                       default=["rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast", "rtsp://admin:Password01@192.168.1.64:554/Streaming/Channels/1/?transportmode=unicast"],
                       help='RTSP stream URLs (可以重复使用同一个 URL 进行测试)')
    parser.add_argument('--model', type=str, default='YOLO11M_100_re.pt', 
                       help='YOLO model path (default: yolov8n.pt)')
    parser.add_argument('--batch-size', type=int, default=4, 
                       help='Maximum batch size for YOLO inference (default: 4)')
    parser.add_argument('--width', type=int, default=640, help='Frame width (default: 640)')
    parser.add_argument('--height', type=int, default=480, help='Frame height (default: 480)')
    parser.add_argument('--confidence', type=float, default=0.5, 
                       help='Confidence threshold (default: 0.5)')
    parser.add_argument('--grid-cols', type=int, default=2, 
                       help='Display grid columns (default: 2)')
    parser.add_argument('--no-display', action='store_true', help='Disable display window')
    parser.add_argument('--detection', action='store_true', default=True,
                       help='Enable YOLO detection (default: True)')
    
    args = parser.parse_args()
    
    # 处理检测开关逻辑
    enable_detection = args.detection
    # enable_detection = False
    
    # 创建处理器
    processor = MultiStreamYOLOProcessor(
        model_path=args.model,
        max_batch_size=args.batch_size,
        target_size=(args.width, args.height),
        confidence_threshold=args.confidence,
        enable_detection=enable_detection
    )
    
    # 添加流（测试时可以重复使用同一个 URL）
    for i, url in enumerate(args.rtsp_urls):
        processor.add_stream(i, url)
    
    # 如果只有一个 URL 但需要多个流进行测试，复制 URL
    if len(args.rtsp_urls) == 1:
        logger.info("使用单个 RTSP URL 创建多个测试流")
        base_url = args.rtsp_urls[0]
        for i in range(1, 4):  # 创建 4 个流用于测试
            processor.add_stream(i, base_url)
    
    # 启动所有流
    processor.start_all_streams()
    
    # 检查是否有活跃的流
    active_count = sum(1 for stream in processor.streams.values() if stream.connected)
    if active_count == 0:
        logger.error("没有活跃的 RTSP 流，程序退出")
        logger.info("请检查：")
        logger.info("1. 网络连接是否正常")
        logger.info("2. RTSP URL 是否正确")
        logger.info("3. 用户名和密码是否正确")
        return
    
    # 在单独的线程中启动处理器
    processing_thread = threading.Thread(target=processor.start_processing, daemon=True)
    processing_thread.start()
    
    # 在主线程中创建并运行显示器（重要！）
    display = None
    if not args.no_display:
        try:
            display = MultiStreamDisplay(processor, args.grid_cols)
            logger.info("在主线程中启动显示器...")
            
            # 直接在主线程中运行显示器，而不是创建新线程
            display.start_display()  # 这会阻塞主线程直到窗口关闭
            
        except Exception as e:
            logger.error(f"启动显示器失败：{e}")
            logger.info("继续运行但不显示窗口...")
            
            # 如果显示失败，运行无显示模式
            try:
                logger.info("主程序运行中... (按 Ctrl+C 停止)")
                while processor.running:
                    time.sleep(1)
                    if processor.stats['batch_count'] % 50 == 0 and processor.stats['batch_count'] > 0:
                        active_streams = sum(1 for stream in processor.streams.values() if stream.connected)
                        logger.info(f"状态更新 - 活跃流：{active_streams}, 总帧数：{processor.stats['total_frames']}, FPS: {processor.stats['fps']:.1f}")
            except KeyboardInterrupt:
                logger.info("接收到中断信号")
    else:
        # 无显示模式的主循环
        try:
            logger.info("主程序运行中... (按 Ctrl+C 停止)")
            while processor.running:
                time.sleep(1)
                if processor.stats['batch_count'] % 50 == 0 and processor.stats['batch_count'] > 0:
                    active_streams = sum(1 for stream in processor.streams.values() if stream.connected)
                    logger.info(f"状态更新 - 活跃流：{active_streams}, 总帧数：{processor.stats['total_frames']}, FPS: {processor.stats['fps']:.1f}")
        except KeyboardInterrupt:
            logger.info("接收到中断信号")
    
    # 清理资源
    logger.info("正在停止处理器...")
    processor.stop_processing()
    processor.stop_all_streams()
    
    # 等待线程结束
    if processing_thread.is_alive():
        processing_thread.join(timeout=2)
    
    # 打印最终统计
    processor.print_stats()
    logger.info("程序结束")

if __name__ == "__main__":
    main() 