#!/usr/bin/env python3
"""
测试多流 RTSP YOLO 批量处理脚本
"""

import sys
import time
import logging
from multi_rtsp_yolo_batch import MultiStreamYOLOProcessor, RTSPStreamHandler

# 设置日志
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

def test_stream_handler():
    """测试单个流处理器"""
    logger.info("=== 测试单个流处理器 ===")
    
    # 使用默认的 RTSP URL
    rtsp_url = "rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast"
    
    handler = RTSPStreamHandler(0, rtsp_url)
    handler.start()
    
    # 等待连接
    time.sleep(5)
    
    if handler.connected:
        logger.info("✓ 流连接成功")
        
        # 尝试获取几帧
        for i in range(5):
            frame = handler.get_frame()
            if frame is not None:
                logger.info(f"✓ 获得帧 {i+1}: {frame.shape}")
            else:
                logger.warning(f"✗ 帧 {i+1}: 无数据")
            time.sleep(1)
    else:
        logger.error("✗ 流连接失败")
    
    handler.stop()
    logger.info("单个流处理器测试完成")

def test_processor():
    """测试批量处理器"""
    logger.info("\n=== 测试批量处理器 ===")
    
    processor = MultiStreamYOLOProcessor(
        model_path="yolov8n.pt",
        max_batch_size=2,
        target_size=(320, 240),  # 较小尺寸以加快测试
        confidence_threshold=0.5
    )
    
    # 添加测试流
    rtsp_url = "rtsp://admin:Password01@192.168.1.65:554/Streaming/Channels/1/?transportmode=unicast"
    processor.add_stream(0, rtsp_url)
    processor.add_stream(1, rtsp_url)
    
    # 启动所有流
    processor.start_all_streams()
    
    # 等待连接
    time.sleep(5)
    
    active_count = sum(1 for stream in processor.streams.values() if stream.connected)
    logger.info(f"活跃流数量：{active_count}")
    
    if active_count > 0:
        logger.info("✓ 至少有一个流连接成功")
        
        # 运行处理器一小段时间
        import threading
        processing_thread = threading.Thread(target=processor.start_processing, daemon=True)
        processing_thread.start()
        
        # 等待一些批次处理
        test_duration = 10  # 10 秒
        start_time = time.time()
        
        while time.time() - start_time < test_duration:
            if processor.stats['batch_count'] > 0:
                logger.info(f"处理进度：批次={processor.stats['batch_count']}, "
                          f"帧数={processor.stats['total_frames']}, "
                          f"FPS={processor.stats['fps']:.1f}")
            time.sleep(2)
        
        processor.stop_processing()
        processor.stop_all_streams()
        
        if processor.stats['batch_count'] > 0:
            logger.info("✓ 批量处理器工作正常")
            processor.print_stats()
        else:
            logger.warning("✗ 批量处理器未产生任何批次")
    else:
        logger.error("✗ 所有流连接失败")

def test_gui_availability():
    """测试 GUI 环境可用性"""
    logger.info("\n=== 测试 GUI 环境 ===")
    
    try:
        import cv2
        # 尝试创建一个测试窗口
        window_name = "GUI Test"
        cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
        
        # 创建一个测试图像
        import numpy as np
        test_image = np.zeros((100, 200, 3), dtype=np.uint8)
        cv2.putText(test_image, "GUI Test", (10, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), 2)
        
        cv2.imshow(window_name, test_image)
        cv2.waitKey(1000)  # 显示 1 秒
        cv2.destroyAllWindows()
        
        logger.info("✓ GUI 环境可用")
        return True
        
    except Exception as e:
        logger.error(f"✗ GUI 环境不可用：{e}")
        return False

def main():
    logger.info("开始多流 RTSP YOLO 批量处理测试")
    
    # 测试 GUI 可用性
    gui_available = test_gui_availability()
    
    # 测试单个流处理器
    try:
        test_stream_handler()
    except Exception as e:
        logger.error(f"流处理器测试失败：{e}")
    
    # 测试批量处理器
    try:
        test_processor()
    except Exception as e:
        logger.error(f"批量处理器测试失败：{e}")
    
    logger.info("\n=== 测试总结 ===")
    logger.info("如果看到 '✓' 标记，说明相应功能正常")
    logger.info("如果看到 '✗' 标记，说明相应功能有问题")
    
    if gui_available:
        logger.info("建议运行：python multi_rtsp_yolo_batch.py")
    else:
        logger.info("建议运行：python multi_rtsp_yolo_batch.py --no-display")

if __name__ == "__main__":
    main() 