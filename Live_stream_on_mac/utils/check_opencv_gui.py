#!/usr/bin/env python3
"""
检查 OpenCV GUI 支持状态
"""
import cv2
import sys
import numpy as np

def check_opencv_build():
    """检查 OpenCV 编译信息"""
    print("=== OpenCV 编译信息 ===")
    print(f"OpenCV 版本：{cv2.__version__}")
    
    # 检查编译信息
    build_info = cv2.getBuildInformation()
    
    # 查找 GUI 相关信息
    gui_backends = []
    lines = build_info.split('\n')
    
    for line in lines:
        if any(keyword in line.lower() for keyword in ['gui', 'cocoa', 'qt', 'gtk', 'highgui']):
            print(f"  {line.strip()}")
            if 'yes' in line.lower() or 'on' in line.lower():
                gui_backends.append(line.strip())
    
    print(f"\n检测到的 GUI 后端：{len(gui_backends)} 个")
    return len(gui_backends) > 0

def test_window_creation():
    """测试窗口创建"""
    print("\n=== 测试窗口创建 ===")
    
    try:
        # 尝试创建窗口
        window_name = "OpenCV Test"
        cv2.namedWindow(window_name, cv2.WINDOW_AUTOSIZE)
        print("✓ 窗口创建成功")
        
        # 创建测试图像
        test_img = np.zeros((200, 400, 3), dtype=np.uint8)
        cv2.putText(test_img, "OpenCV GUI Test", (50, 100), 
                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
        cv2.putText(test_img, "Press any key to close", (50, 150), 
                    cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 255), 2)
        
        # 显示图像
        cv2.imshow(window_name, test_img)
        print("✓ 图像显示成功")
        print("请查看是否有窗口弹出，然后按任意键继续...")
        
        # 等待按键
        key = cv2.waitKey(0)
        print(f"✓ 检测到按键：{key}")
        
        cv2.destroyAllWindows()
        print("✓ 窗口关闭成功")
        return True
        
    except Exception as e:
        print(f"✗ 窗口测试失败：{e}")
        return False

def check_backend_support():
    """检查不同后端支持"""
    print("\n=== 测试不同后端 ===")
    
    backends_to_test = [
        ('CAP_AVFOUNDATION', cv2.CAP_AVFOUNDATION),
        ('CAP_QTKIT', 500),  # QTKit backend
        ('CAP_ANY', cv2.CAP_ANY),
    ]
    
    for name, backend in backends_to_test:
        try:
            cap = cv2.VideoCapture(0, backend)  # 测试摄像头
            if cap.isOpened():
                print(f"✓ {name} 后端可用")
                cap.release()
            else:
                print(f"✗ {name} 后端不可用")
        except:
            print(f"✗ {name} 后端测试失败")

def main():
    print("OpenCV GUI 支持检查工具")
    print("=" * 50)
    
    # 基本信息
    print(f"Python 版本：{sys.version}")
    print(f"运行平台：{sys.platform}")
    
    # 检查编译信息
    has_gui = check_opencv_build()
    
    # 检查后端支持
    check_backend_support()
    
    # 测试窗口
    if has_gui:
        window_works = test_window_creation()
        
        if not window_works:
            print("\n=== 可能的解决方案 ===")
            print("1. 安装 XQuartz:")
            print("   brew install --cask xquartz")
            print("   然后重启终端")
            print()
            print("2. 重新安装 OpenCV:")
            print("   pip uninstall opencv-python")
            print("   pip install opencv-python-headless  # 无 GUI 版本")
            print("   pip install opencv-contrib-python   # 完整版本")
            print()
            print("3. 使用 conda 安装：")
            print("   conda install opencv")
            print()
            print("4. 检查系统权限：")
            print("   系统偏好设置 > 安全性与隐私 > 隐私 > 辅助功能")
            print("   添加终端应用的权限")
    else:
        print("\n✗ OpenCV 未编译 GUI 支持")
        print("建议重新安装支持 GUI 的 OpenCV 版本")

if __name__ == "__main__":
    main() 