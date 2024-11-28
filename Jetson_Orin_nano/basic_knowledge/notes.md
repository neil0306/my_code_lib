# 常用依赖包

- CCache
    ```txt
    Ccache 是一个编译器缓存工具，主要用于加速 C/C++ 代码的重新编译。它通过缓存之前的编译结果，并在相同的编译请求再次出现时直接使用缓存结果，从而减少编译时间。
    以下是 Ccache 的一些关键功能：
    - 加速编译：通过缓存编译结果，减少重复编译的时间。
    - 支持多种编译器：包括 GCC、Clang 和 MSVC 等。
    - 跨目录共享缓存：在不同目录或分支之间共享缓存，提高编译效率。
    - 自动缓存管理：自动管理缓存大小，确保不会占用过多磁盘空间。
    - Ccache 特别适合于频繁进行代码变更和分支切换的开发环境中使用
    ```
  - 安装：`sudo apt-get install ccache -y`


## 基础工具
- **v4l-utils**: 提供用于控制和管理视频设备的工具，特别是符合 Video4Linux (V4L) 标准的设备
- **gawk**: GNU 版本的 awk，一种用于文本处理的编程语言
- **curl**: 用于从命令行下载文件或与网络进行数据交换
- **mingetty**: 一个简单的 getty 程序，用于管理虚拟控制台登录
- **apt-utils**: 提供一些用于 apt 包管理器的实用工具
- **python3-dev**: 包含 Python 3 的头文件和静态库，通常用于编译需要 Python 支持的 C 扩展模块
- **python3-pip**: Python 的包管理工具，用于安装和管理 Python 包
- **ccache**: 编译器缓存工具，用于加速 C/C++ 代码的重新编译

## NVIDIA 和 CUDA 相关
- **nvidia-jetpack**: NVIDIA Jetson 平台的软件开发套件，包含 TensorRT、CUDA、cuDNN 等组件
- **cuda-toolkit-10-2**: NVIDIA CUDA 工具包，提供 GPU 加速计算的开发环境
- **nvidia-container-csv-cuda**: 提供 CUDA 的容器支持
- **jetson-stats**: 用于监控和控制 NVIDIA Jetson 设备的工具

## GStreamer 相关
- **libgstreamer1.0-0**: GStreamer 的核心库
- **gstreamer1.0-tools**: GStreamer 的命令行工具
- **gstreamer1.0-plugins-good/bad/ugly**: GStreamer 的插件集合，提供各种媒体格式的支持
- **gstreamer1.0-libav**: 提供基于 FFmpeg 的 GStreamer 插件
- **libgstrtspserver-1.0-0**: 用于创建 RTSP 流媒体服务器的库

## Python 相关
- **numpy**: 科学计算库，提供多维数组对象和各种数学函数
- **torch**: PyTorch 的核心库，用于深度学习
- **torchvision**: 提供 PyTorch 的计算机视觉工具
- **setuptools**: Python 的打包工具
- **Cython**: 用于将 Python 代码编译为 C 扩展模块的工具
- **pycocotools**: COCO 数据集的 Python API
- **paho-mqtt**: 用于 MQTT 协议的 Python 客户端库
- **pycurl**: libcurl 的 Python 接口
- **Jetson.GPIO**: 用于控制 Jetson 平台 GPIO 的 Python 库
- **spidev**: 用于访问 SPI 设备的 Python 库
  - 微控制器与外围设备之间的短距离通信
  - 嵌入式系统中的设备间通信
  - 常见的应用场景包括：
    - 与传感器通信（如温度传感器、加速度计）
    - 与 LCD 显示屏通信
    - 与 SD 卡通信
    - 与 EEPROM/Flash 存储器通信
- **wrapt**: 用于装饰器和代理的 Python 库
- **redis**: Redis 的 Python 客户端
- **websockets**: 用于 WebSocket 协议的 Python 库

## 其他工具和库
- **figlet**: 用于生成 ASCII 艺术文本的工具
- **libjpeg-dev**: JPEG 图像处理库的开发文件
- **libopenblas-dev**: OpenBLAS 的开发文件，用于线性代数运算
- **libomp-dev**: OpenMP 的开发文件，用于并行编程
- **libcurl4-openssl-dev**: libcurl 的开发文件，支持 OpenSSL
- **libssl-dev**: OpenSSL 的开发文件
- **udisks2**: 提供磁盘管理的 D-Bus 服务
- **autofs**: 自动挂载文件系统的工具
- **v4l2loopback-dkms**: 用于创建虚拟视频设备的内核模块
- **libjansson4**: 用于 JSON 数据处理的库
- **zlib1g-dev**: zlib 的开发文件，用于数据压缩



安装依赖包以及相关基础配置：
```shell
sudo apt-get install -y v4l-utils # 安装 v4l-utils 库, 用于视频设备管理
sudo apt-get install gawk curl -y # 安装 gawk 和 curl, 用于文本处理和网络请求
sudo apt-get install mingetty   # 管理虚拟控制台登录

sudo apt-get install nvidia-jetpack -y    # 安装 NVIDIA Jetson 平台的软件开发套件, 包含 TensorRT、CUDA、cuDNN 等组件

sudo -H pip3 install -U jetson-stats   # 监控和控制 NVIDIA Jetson 设备的工具

sudo apt dist-upgrade -y  # 智能更新软件包, 会处理依赖关系, 升级依赖, 解决冲突
sudo apt-get update && sudo apt-get install -y --no-install-recommends apt-utils  # 更新软件包列表
sudo apt-get install -y python3-dev  # 安装 Python 开发文件
sudo apt-get install -y python3-pip  # Python 的包管理工具

sudo nvpmodel -m 0  # 设置 Jetson 的功耗模式
sudo jetson_clocks  # 调整 Jetson 的时钟频率

sudo systemctl set-default multi-user.target  # 设置默认启动目标为多用户模式

# ----------- 安装 GStreamer 相关库, 用于多媒体处理 -----------
sudo apt install -y \
   libgstreamer1.0-0 \
   gstreamer1.0-tools \
   gstreamer1.0-plugins-good \
   gstreamer1.0-plugins-bad \
   gstreamer1.0-plugins-ugly \
   gstreamer1.0-libav \
   libgstrtspserver-1.0-0


sudo usermod -a -G tty 你要设置的用户名  # 添加用户到 tty 组

# ------ 配置 github ------------
git config --global user.email "git账号邮箱"
git config --global user.name "用来标识你身份的git昵称"

# ---------- GPIO ------------
sudo groupadd -f -r gpio  # 创建 gpio 组
sudo usermod -a -G gpio 你的用户名  # 将用户添加到 gpio 组
sudo cp /lib/udev/rules.d/60-jetson-gpio-common.rules  /etc/udev/rules.d/  # 复制规则文件到 udev 规则目录, 用于 GPIO 管理


# --------- pytorch ----------
# install the dependencies (if not already onboard)
sudo apt-get install python3-pip libjpeg-dev libopenblas-dev libopenmpi-dev libomp-dev -y  # 安装 Python 开发文件
sudo -H pip3 install future  # 安装 future 库 , 用于帮助 python2 和 python3 的兼容性
sudo pip3 install wheel mock pillow  # 安装 wheel、mock 和 pillow 库, 用于打包和模拟
sudo -H pip3 install testresources  # 安装 testresources 库, 用于测试资源
sudo pip3 install configparser  # 安装 configparser 库, 用于解析配置文件

# install gdown to download from Google drive
sudo -H pip3 install gdown
# download the wheel
gdown https://drive.google.com/uc?id=1TqC6_2cwqiYacjoLhLgrZoap6-sVL2sd   # download torch
sudo -H pip3 install torch-1.10.0a0+git36449ea-cp36-cp36m-linux_aarch64.whl
# clean up
rm torch-1.10.0a0+git36449ea-cp36-cp36m-linux_aarch64.whl

# download TorchVision 0.11.0
gdown https://drive.google.com/uc?id=1C7y6VSIBkmL2RQnVy8xF9cAnrrpJiJ-K  # download torchvision
# install TorchVision 0.11.0
sudo -H pip3 install torchvision-0.11.0a0+fa347eb-cp36-cp36m-linux_aarch64.whl
# clean up
rm torchvision-0.11.0a0+fa347eb-cp36-cp36m-linux_aarch64.whl

# above 58.3.0 you get version issues
sudo -H pip3 install setuptools==58.3.0
sudo -H pip3 install Cython


# --------- cuda related ----------
sudo apt install -y cuda-command-line-tools-10-2 cuda-compiler-10-2 cuda-cuobjdump-10-2 cuda-cupti-10-2 cuda-cupti-dev-10-2 \
  cuda-gdb-10-2 cuda-libraries-10-2 cuda-libraries-dev-10-2 cuda-memcheck-10-2 cuda-nvdisasm-10-2 cuda-nvgraph-10-2 cuda-nvgraph-dev-10-2 cuda-nvml-dev-10-2 \
  cuda-nvprof-10-2 cuda-nvprune-10-2 cuda-nvtx-10-2 cuda-tools-10-2
sudo apt install libcurand-10-2 libcurand-dev-10-2 libcusolver-10-2 libcusolver-dev-10-2 libcusparse-10-2 libcusparse-dev-10-2 -y

sudo pip3 install pycocotools       # COCO 数据集的 Python API

sudo apt install figlet             # 生成 ASCII 艺术文本的工具

sudo pip3 install --upgrade pip==20.2  # 升级 pip 到 20.2 版本
sudo pip3 install --ignore-installed PyYAML  # 忽略已安装的 PyYAML 库
sudo pip3 install serial  # 安装 serial 库

# --------- spidev: 用于同步设备间通信 ----------
sudo pip3 install --upgrade setuptools  # 升级 setuptools
sudo pip3 install --upgrade pip  # 升级 pip
sudo -H pip3 install spi  # 安装 spi 库, 用于 SPI 通信
sudo pip3 install  spidev  # 安装 spidev 库
sudo modprobe spidev  # 加载 spidev 内核模块
echo spidev | sudo tee -a /etc/modules  # 将 spidev 添加到 /etc/modules 文件中


# -------- solve problem For name server ----------
sudo rm  /etc/resolv.conf 
sudo ln -s /run/systemd/resolve/resolv.conf /etc/resolv.conf




# ----------- something from "requirements.sh" ---------
sudo apt install libcurl4-openssl-dev libssl-dev -y  # 安装 libcurl 和 libssl 开发文件, 用于 HTTP 请求
sudo -H pip3 install hikvisionapi  # 安装 hikvisionapi 库, 用于海康威视摄像头
sudo -H pip3 install pycurl  # 安装 pycurl 库, 用于 HTTP 请求
sudo -H pip3 install getch  # 安装 getch 库, 用于获取按键输入

sudo apt install udisks2  # 安装 udisks2 库, 用于文件系统管理
sudo apt install autofs  # 安装 autofs 库, 用于自动挂载文件系统

sudo apt-get install v4l2loopback-dkms -y  # 安装 v4l2loopback-dkms 库, 用于虚拟视频设备


sudo apt dist-upgrade -y
sudo apt-get update && sudo apt-get install -y --no-install-recommends apt-utils

sudo pip3 install rq rq-dashboard  # 安装 rq 和 rq-dashboard 库

sudo timedatectl set-timezone 'Asia/Hong_Kong'  # 设置时区为香港
sudo timedatectl set-ntp true  # 启用 NTP 时间同步 (马上更新设备的时间)

sudo apt-get install -y ccache  # 安装 ccache 库
sudo apt-get install -y libblas-dev libeigen3-dev  # 安装 libblas 和 libeigen3 开发文件

sudo echo "PollIntervalMinSec=32" | sudo tee -a /etc/systemd/timesyncd.conf  # 设置最小轮询间隔
sudo echo "PollIntervalMaxSec=2048" | sudo tee -a /etc/systemd/timesyncd.conf  # 设置最大轮询间隔

sudo systemctl restart systemd-timesyncd.service  # 重启时间同步服务
sudo systemctl status systemd-timesyncd.service  # 查看时间同步服务状态

sudo apt install -y libcurl4-openssl-dev libssl-dev  # 安装 libcurl 和 libssl 开发文件, 用于 HTTP 请求

sudo systemctl set-default multi-user.target  # 设置默认启动目标为多用户模式


sudo apt-get clean  # 清理软件包缓存
sudo apt-get autoremove -y  # 自动删除不需要的软件包

sudo -H pip3 install paho-mqtt  # 安装 paho-mqtt 库, 用于 MQTT 协议

if [[ $(uname -p | grep x86_64) != "" ]]; then
   sudo apt-get install -y dmidecode  # 安装 dmidecode 库, 用于获取系统硬件信息
fi

sudo apt install -y libjansson4=2.11-1 v4l2ucp  # 安装 libjansson4 库, 用于 JSON 数据处理


sudo apt-get install -y libjpeg-dev zlib1g-dev  # 安装 libjpeg 和 zlib 开发文件

sudo apt-get install -y python3-pip libopenblas-base libopenmpi-dev libjpeg-dev zlib1g-dev python3-matplotlib  # 安装 Python 开发文件
sudo -H pip3 install tqdm  # 安装 tqdm 库, 用于进度条显示

sudo apt install libcurl4-openssl-dev libssl-dev  # 安装 libcurl 和 libssl 开发文件, 用于 HTTP 请求

sudo -H pip3 install packaging  # 安装 packaging 库, 用于打包和分发 Python 包

sudo -H pip3 install Jetson.GPIO  # 安装 Jetson.GPIO 库, 用于控制 Jetson 平台 GPIO  
sudo -H pip3 install pyserial  # 安装 pyserial 库, 用于串口通信
sudo -H pip3 install wrapt==1.12.1  # 安装 wrapt 库, 用于装饰器和代理
sudo -H pip3 install redis  # 安装 redis 库, 用于 Redis 数据库

sudo -H pip3 install websockets

## Enable GPIO for jetson nano
sudo groupadd -f -r gpio
# sudo usermod -a -G gpio your_user_name
sudo usermod -a -G gpio 你的用户名  # 将用户添加到 gpio 组
sudo usermod -a -G dialout 你的用户名  # 将用户添加到 dialout 组
sudo cp /lib/udev/rules.d/60-serial.rules /etc/udev/rules.d/  # 复制规则文件到 udev 规则目录
sudo usermod -a -G tty 你的用户名  # 将用户添加到 tty 组

cd
git clone git@github.com:NVIDIA/jetson-gpio.git
sudo cp jetson-gpio/lib/python/Jetson/GPIO/99-gpio.rules /etc/udev/rules.d/  # 复制规则文件到 udev 规则目录
sudo udevadm control --reload-rules && sudo udevadm trigger  # 重新加载 udev 规则并触发事件

i2cdetect -y -r 1  # 检测 I2C 设备, 用来处理各种传感器的通信, 比如温湿度传感器, 加速度传感器, 陀螺仪, 压力传感器等



# ---------- 处理串口 ----------
sudo chmod 0777 /dev/ttyACM0    # 修改串口设备 /dev/ttyACM0 的权限

# nvgetty 是 NVIDIA Jetson 设备上的一个串口终端服务
sudo systemctl stop nvgetty       # 停止 nvgetty 服务
sudo systemctl disable nvgetty    # 禁止开机自启动
sudo udevadm trigger              # 触发 udev 事件 (udev 负责管理设备节点和设备权限, 此命令强制系统重新检测设备并应用新的权限规则)
# --------------------------------
```