#!/bin/bash
# 安装依赖脚本 (通用版本)

echo "安装基础 Python 依赖包..."

# 必需依赖
pip install opencv-python numpy

echo "安装可选依赖包..."

# 虚拟摄像头支持 (可选)
echo "尝试安装虚拟摄像头支持..."
pip install pyvirtualcam || echo "虚拟摄像头支持安装失败，将跳过此功能"

# HTTP 流媒体支持 (可选)
echo "尝试安装 HTTP 流媒体支持..."
pip install flask || echo "HTTP 流媒体支持安装失败，将跳过此功能"

# YOLO 检测支持 (可选)
echo "尝试安装 YOLO 检测支持..."
pip install ultralytics || echo "YOLO 检测支持安装失败，将跳过此功能"

# 安装 ffmpeg (如果没有安装的话)
if ! command -v ffmpeg &> /dev/null; then
    echo "安装 ffmpeg..."
    if command -v brew &> /dev/null; then
        brew install ffmpeg
    else
        echo "请先安装 Homebrew，然后运行: brew install ffmpeg"
        echo "或者从 https://ffmpeg.org/ 手动安装 ffmpeg"
    fi
else
    echo "ffmpeg 已安装"
fi

echo ""
echo "依赖安装完成！"
echo ""
echo "使用说明："
echo "1. 如果所有依赖都安装成功，您可以使用所有功能"
echo "2. 如果某些依赖安装失败，程序会自动跳过相关功能"
echo "3. 至少会有显示窗口功能可用" 