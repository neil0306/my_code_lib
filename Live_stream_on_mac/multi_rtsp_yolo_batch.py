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
import concurrent.futures
from concurrent.futures import ThreadPoolExecutor

# 设置日志
logging.basicConfig(level=logging.INFO, 
                   format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

# 添加一个函数来动态调整日志级别
def enable_debug_logging():
    """启用调试日志"""
    logging.getLogger(__name__).setLevel(logging.DEBUG)
    logger.info("已启用调试日志模式")
    
def set_logging_level(level: str):
    """设置日志级别"""
    level_map = {
        'DEBUG': logging.DEBUG,
        'INFO': logging.INFO,
        'WARNING': logging.WARNING,
        'ERROR': logging.ERROR
    }
    if level.upper() in level_map:
        logging.getLogger(__name__).setLevel(level_map[level.upper()])
        logger.info(f"日志级别设置为：{level.upper()}")

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
        
        # 捕获帧统计（用于内部监控）
        self.captured_frame_count = 0
        self.start_time = 0
        self.capture_fps = 0.0
        
        # YOLO 处理帧统计（用于显示）
        self.processed_frame_count = 0
        self.processing_start_time = 0
        self.processing_fps = 0.0
        self.last_processing_fps_update = 0
        self.fps_update_interval = 1.0  # 每秒更新一次 FPS
        
    def start(self):
        """启动流捕获"""
        self.running = True
        self.start_time = time.time()
        self.processing_start_time = time.time()
        self.last_processing_fps_update = self.start_time
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
    
    def mark_frame_processed(self):
        """标记一帧被 YOLO 处理完成"""
        self.processed_frame_count += 1
        self._update_processing_fps()
    
    def _update_processing_fps(self):
        """更新 YOLO 处理 FPS 统计"""
        current_time = time.time()
        if current_time - self.last_processing_fps_update >= self.fps_update_interval:
            elapsed = current_time - self.processing_start_time
            if elapsed > 0:
                self.processing_fps = self.processed_frame_count / elapsed
            self.last_processing_fps_update = current_time
    
    def get_processing_fps(self) -> float:
        """获取 YOLO 处理 FPS"""
        return self.processing_fps
    
    def get_capture_fps(self) -> float:
        """获取捕获 FPS"""
        return self.capture_fps
    
    def get_stats(self) -> Dict:
        """获取流统计信息"""
        elapsed = time.time() - self.start_time if self.start_time > 0 else 0
        processing_elapsed = time.time() - self.processing_start_time if self.processing_start_time > 0 else 0
        return {
            'stream_id': self.stream_id,
            'connected': self.connected,
            'captured_frame_count': self.captured_frame_count,
            'processed_frame_count': self.processed_frame_count,
            'capture_fps': self.capture_fps,
            'processing_fps': self.processing_fps,
            'elapsed_time': elapsed,
            'processing_elapsed_time': processing_elapsed,
            'reconnect_attempts': self.reconnect_attempts
        }
            
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
                
                # 验证帧有效性
                if not self._validate_captured_frame(frame):
                    logger.warning(f"Stream {self.stream_id}: 捕获的帧无效，跳过")
                    continue
                
                # 更新捕获帧计数和 FPS 统计
                self.captured_frame_count += 1
                elapsed = time.time() - self.start_time
                if elapsed > 0:
                    self.capture_fps = self.captured_frame_count / elapsed
                
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
    
    def _validate_captured_frame(self, frame: np.ndarray) -> bool:
        """验证捕获的帧是否有效"""
        try:
            # 基本检查
            if frame is None or not isinstance(frame, np.ndarray):
                return False
            
            # 检查维度
            if len(frame.shape) != 3:
                return False
            
            # 检查尺寸
            height, width, channels = frame.shape
            if height <= 0 or width <= 0 or channels != 3:
                return False
            
            # 检查目标尺寸
            if (height, width) != (self.target_size[1], self.target_size[0]):
                return False
            
            # 检查数据类型和范围
            if frame.dtype != np.uint8:
                return False
            
            # 检查是否为全黑帧（可能的无效信号）
            if frame.max() == 0:
                return False
            
            return True
            
        except Exception:
            return False


class MultiStreamYOLOProcessor:
    """多流 YOLO 批量处理器"""
    
    def __init__(self, model_path: str = "model_weights/YOLO11M_100_re.pt", max_batch_size: int = 8, 
                 target_size: Tuple[int, int] = (640, 480), confidence_threshold: float = 0.5,
                 enable_detection: bool = True, max_workers: int = 8):
        self.model_path = model_path
        self.max_batch_size = max_batch_size
        self.target_size = target_size
        self.confidence_threshold = confidence_threshold
        self.enable_detection = enable_detection
        self.max_workers = max_workers  # 线程池大小
        
        # 只有在启用检测时才加载 YOLO 模型
        if self.enable_detection:
            logger.info(f"加载 YOLO 模型：{model_path}")
            self.model = YOLO(model_path)
            logger.info("YOLO 模型加载完成")
            
            # 创建线程池用于并行推理
            self.executor = ThreadPoolExecutor(max_workers=self.max_workers)
            logger.info(f"创建线程池，最大工作线程数：{self.max_workers}")
        else:
            self.model = None
            self.executor = None
            logger.info("YOLO 检测已禁用，将直接显示原始视频流")
        
        # 流处理器
        self.streams: Dict[int, RTSPStreamHandler] = {}
        self.running = False
        
        # 结果存储
        self.processed_frames: Dict[int, np.ndarray] = {}
        self.results_lock = threading.Lock()
        
        # 性能统计 - 移除 avg_batch_size
        self.stats = {
            'total_frames': 0,
            'batch_count': 0,
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
                # 添加帧验证
                if frame is not None and self._validate_frame(frame):
                    batch_frames.append(frame)
                    stream_ids.append(stream_id)
                    
                    # 限制批量大小
                    if len(batch_frames) >= self.max_batch_size:
                        break
                        
        return batch_frames, stream_ids
    
    def _validate_frame(self, frame: np.ndarray) -> bool:
        """验证帧是否有效"""
        try:
            # 检查是否为 None
            if frame is None:
                return False
            
            # 检查是否为有效的 numpy 数组
            if not isinstance(frame, np.ndarray):
                return False
            
            # 检查维度 (应该是 3D: height, width, channels)
            if len(frame.shape) != 3:
                return False
            
            # 检查尺寸
            height, width, channels = frame.shape
            if height <= 0 or width <= 0 or channels <= 0:
                return False
            
            # 检查通道数 (应该是 3 for BGR)
            if channels != 3:
                return False
            
            # 检查目标尺寸
            if (height, width) != (self.target_size[1], self.target_size[0]):
                logger.warning(f"帧尺寸不匹配：期望 {(self.target_size[1], self.target_size[0])}，实际 {(height, width)}")
                return False
            
            # 检查数据类型
            if frame.dtype != np.uint8:
                return False
            
            # 检查数据范围
            if frame.min() < 0 or frame.max() > 255:
                return False
            
            return True
            
        except Exception as e:
            logger.warning(f"帧验证错误：{e}")
            return False
        
    def _process_single_frame(self, frame: np.ndarray, stream_id: int) -> Tuple[int, np.ndarray, int]:
        """处理单个帧 - 用于线程池"""
        try:
            if not self._validate_frame(frame):
                raise ValueError(f"无效帧，流 ID: {stream_id}")
            
            # 单帧 YOLO 推理 (batch_size=1)
            results = self.model([frame], verbose=False, conf=self.confidence_threshold)
            
            if not results or len(results) != 1:
                raise ValueError(f"YOLO 推理结果异常，流 ID: {stream_id}")
            
            result = results[0]
            
            # 绘制检测结果
            annotated_frame = result.plot()
            
            # 计算检测数量
            detection_count = len(result.boxes) if result.boxes is not None else 0
            
            # 标记该流的一帧被处理完成
            if stream_id in self.streams:
                self.streams[stream_id].mark_frame_processed()
            
            # 获取流的 YOLO 处理 FPS
            processing_fps = self.streams[stream_id].get_processing_fps() if stream_id in self.streams else 0.0
            
            # 添加流信息和统计 - 显示 YOLO 处理 FPS
            cv2.putText(annotated_frame, f"Stream {stream_id}", 
                      (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
            cv2.putText(annotated_frame, f"Objects: {detection_count}", 
                        (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
            cv2.putText(annotated_frame, f"YOLO FPS: {processing_fps:.1f}", 
                        (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)
            
            return stream_id, annotated_frame, detection_count
            
        except Exception as e:
            logger.error(f"单帧处理失败 (流 {stream_id}): {e}")
            # 返回原始帧作为备用
            return stream_id, frame.copy(), 0

    def process_batch(self, frames: List[np.ndarray], stream_ids: List[int]):
        """批量处理帧 - 修改为并行处理单帧"""
        if not frames:
            return
        
        # 验证输入一致性
        if len(frames) != len(stream_ids):
            logger.error(f"帧数量 ({len(frames)}) 与流 ID 数量 ({len(stream_ids)}) 不匹配")
            return
            
        try:
            batch_start_time = time.time()
            
            if self.enable_detection and self.model is not None:
                # 再次验证所有帧
                valid_frames = []
                valid_stream_ids = []
                for frame, stream_id in zip(frames, stream_ids):
                    if self._validate_frame(frame):
                        valid_frames.append(frame)
                        valid_stream_ids.append(stream_id)
                    else:
                        logger.warning(f"跳过无效帧，流 ID: {stream_id}")
                
                if not valid_frames:
                    logger.warning("批次中没有有效帧，跳过处理")
                    return
                
                # 更新帧列表和流 ID 列表
                frames = valid_frames
                stream_ids = valid_stream_ids
                
                logger.debug(f"处理批次：{len(frames)} 帧，流 IDs: {stream_ids}")
                
                try:
                    # 使用线程池并行处理每个帧 (每个都是 batch_size=1)
                    logger.debug(f"开始并行 YOLO 推理，输入帧数：{len(frames)}")
                    
                    # 提交所有任务到线程池
                    futures = []
                    for frame, stream_id in zip(frames, stream_ids):
                        future = self.executor.submit(self._process_single_frame, frame, stream_id)
                        futures.append(future)
                    
                    # 收集结果
                    total_detections = 0
                    processing_results = []
                    
                    # 等待所有任务完成，设置超时
                    for future in concurrent.futures.as_completed(futures, timeout=5.0):
                        try:
                            stream_id, annotated_frame, detection_count = future.result()
                            processing_results.append((stream_id, annotated_frame, detection_count))
                            total_detections += detection_count
                        except Exception as e:
                            logger.error(f"获取处理结果失败：{e}")
                    
                    logger.debug(f"并行 YOLO 推理完成，处理结果数：{len(processing_results)}")
                    
                    inference_time = time.time() - batch_start_time
                    
                    # 更新处理后的帧
                    with self.results_lock:
                        for stream_id, annotated_frame, detection_count in processing_results:
                            # 添加推理时间信息 - 调整位置
                            cv2.putText(annotated_frame, f"Inference: {inference_time*1000:.1f}ms", 
                                        (10, 120), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)
                            
                            self.processed_frames[stream_id] = annotated_frame
                                
                except concurrent.futures.TimeoutError:
                    logger.error("并行 YOLO 推理超时")
                    # 使用原始帧而不是检测结果
                    self._process_frames_without_detection(frames, stream_ids, batch_start_time)
                    return
                except Exception as yolo_error:
                    logger.error(f"并行 YOLO 推理失败：{yolo_error}")
                    # 使用原始帧而不是检测结果
                    self._process_frames_without_detection(frames, stream_ids, batch_start_time)
                    return
                    
            else:
                # 不进行检测，直接显示原始帧
                self._process_frames_without_detection(frames, stream_ids, batch_start_time)
                return
                        
            # 更新统计信息 - 移除 avg_batch_size 计算
            self.stats['total_frames'] += len(frames)
            self.stats['batch_count'] += 1
                            
            # 第一批次或每 50 批次输出详细信息
            if self.stats['batch_count'] == 1 or self.stats['batch_count'] % 50 == 0:
                if self.enable_detection:
                    logger.info(f"批次 #{self.stats['batch_count']}: {len(frames)} 帧，{total_detections} 个检测，"
                               f"推理时间：{inference_time*1000:.1f}ms, 流：{stream_ids} (并行处理)")
                else:
                    logger.info(f"批次 #{self.stats['batch_count']}: {len(frames)} 帧，无检测模式，流：{stream_ids}")
                            
        except Exception as e:
            logger.error(f"批量处理错误：{e}")
            import traceback
            traceback.print_exc()

    def _process_frames_without_detection(self, frames: List[np.ndarray], stream_ids: List[int], start_time: float):
        """不进行检测的帧处理"""
        inference_time = time.time() - start_time
        
        with self.results_lock:
            for i, (frame, stream_id) in enumerate(zip(frames, stream_ids)):
                try:
                    # 直接使用原始帧
                    display_frame = frame.copy()
                    
                    # 标记该流的一帧被处理完成（即使没有检测）
                    if stream_id in self.streams:
                        self.streams[stream_id].mark_frame_processed()
                    
                    # 获取流的 YOLO 处理 FPS
                    processing_fps = self.streams[stream_id].get_processing_fps() if stream_id in self.streams else 0.0
                    
                    # 添加流信息 - 显示 YOLO 处理 FPS
                    cv2.putText(display_frame, f"Stream {stream_id}", 
                              (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
                    
                    # 添加无检测标识
                    detection_status = "Detection: OFF" if not self.enable_detection else "Detection: ERROR"
                    cv2.putText(display_frame, detection_status, 
                                (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)
                    
                    # 添加 YOLO 处理 FPS 信息
                    cv2.putText(display_frame, f"YOLO FPS: {processing_fps:.1f}", 
                                (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)
                    
                    # 添加处理时间
                    cv2.putText(display_frame, f"Process: {inference_time*1000:.1f}ms", 
                                (10, 120), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 0), 2)
                    
                    self.processed_frames[stream_id] = display_frame
                    
                except Exception as e:
                    logger.error(f"处理原始帧时出错 (流 {stream_id}): {e}")
                    # 最后的备用方案：使用黑色帧
                    fallback_frame = np.zeros(self.target_size[::-1] + (3,), dtype=np.uint8)
                    cv2.putText(fallback_frame, f"Stream {stream_id} ERROR", 
                              (10, self.target_size[1]//2), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                    self.processed_frames[stream_id] = fallback_frame
        
        # 更新统计信息
        self.stats['total_frames'] += len(frames)
        self.stats['batch_count'] += 1

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
                                  f"当前批量大小：{len(batch_frames)}, FPS: {self.stats['fps']:.1f}")
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
        
        # 关闭线程池
        if self.executor:
            logger.info("正在关闭线程池...")
            self.executor.shutdown(wait=True)
            logger.info("线程池已关闭")

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
        logger.info(f"  处理 FPS: {self.stats['fps']:.1f}")
        logger.info(f"  运行时间：{elapsed:.1f}s")
        
        # 打印每个流的统计信息
        logger.info(f"各流统计：")
        for stream_id, stream in self.streams.items():
            stats = stream.get_stats()
            logger.info(f"  Stream {stream_id}: 捕获 {stats['captured_frame_count']} 帧 (FPS: {stats['capture_fps']:.1f}), "
                       f"YOLO 处理 {stats['processed_frame_count']} 帧 (FPS: {stats['processing_fps']:.1f}), "
                       f"连接：{stats['connected']}, 重连次数：{stats['reconnect_attempts']}")


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
            
        # 添加全局信息 - 使用 YOLO 处理 FPS
        active_streams = sum(1 for stream in self.processor.streams.values() if stream.connected)
        avg_yolo_fps = sum(stream.get_processing_fps() for stream in self.processor.streams.values()) / len(self.processor.streams) if self.processor.streams else 0
        
        info_text = f"Streams: {active_streams}/{len(stream_ids)} | Avg YOLO FPS: {avg_yolo_fps:.1f} | Process FPS: {self.processor.stats['fps']:.1f}"
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
                       default=["rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast","rtsp://admin:Password01@192.168.1.64:554/Streaming/Channels/1/?transportmode=unicast"],
                       help='RTSP stream URLs (可以重复使用同一个 URL 进行测试)')
    parser.add_argument('--model', type=str, default='model_weights/YOLO11M_100_re.mlpackage', 
                       help='YOLO model path (default: model_weights/YOLO11M_100_re.mlpackage)')
    parser.add_argument('--batch-size', type=int, default=6, 
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
    parser.add_argument('--max-workers', type=int, default=8,
                       help='Maximum worker threads for parallel inference (default: 8)')
    
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
        enable_detection=enable_detection,
        max_workers=args.max_workers  # 新增参数
    )
    
    # 修复：正确处理多个 RTSP URL 的复制逻辑
    if len(args.rtsp_urls) == 2:
        logger.info("为两个 RTSP URL 分别创建 6 个流，总共 12 个流")
        
        # 为第一个 URL 创建 6 个流 (stream 0-5)
        url1 = args.rtsp_urls[0]
        for i in range(6):
            processor.add_stream(i, url1)
            
        # 为第二个 URL 创建 6 个流 (stream 6-11)
        url2 = args.rtsp_urls[1]
        for i in range(6, 12):
            processor.add_stream(i, url2)
            
        logger.info(f"已创建 12 个流：0-5 使用 URL1({url1})，6-11 使用 URL2({url2})")
        
    else:
        # 原有逻辑：直接添加所有提供的 URL
        for i, url in enumerate(args.rtsp_urls):
            processor.add_stream(i, url)
            
        # 如果只有一个 URL，复制它来创建更多流用于测试
        if len(args.rtsp_urls) == 1:
            logger.info("使用单个 RTSP URL 创建多个测试流")
            base_url = args.rtsp_urls[0]
            for i in range(1, args.batch_size):
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
            # 修改网格列数以更好地显示 12 个流
            grid_cols = 4 if len(processor.streams) == 12 else args.grid_cols
            display = MultiStreamDisplay(processor, grid_cols)
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