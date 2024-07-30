# About
这里主要想记录一些零碎的，常用的命令。


# 直接打包实体机的 ubuntu 系统环境成 docker 镜像
参考资料：
- https://blog.csdn.net/qq_29935433/article/details/120778646

背景：
- 想要复刻出一个与`实体机`一样的 docker 环境。
- 这里给出的方案可能不是最优的，这里只是记录一些可行方法。

## 方案一：直接打包整个系统

1. 切换到`root`用户
    ```shell
    sudo -i   # 之后会要求输入当前用户的密码

    # 或者用下面的方式
    sudo su   # 之后会要求输入 root 用户的密码，如果不知道这个密码，可以用上面的方式
    ```

2. 执行打包命令：
    ```shell
    # 要确保已经在 root 用户下
    cd /

    # 执行打包命令，不想打包的目录可以继续添加用--exclude 参数进行排除;  打包好的文件会放在 /tmp 目录下，名字是 ubuntu-arm64.tar
    tar -cvpf /tmp/ubuntu-arm64.tar --directory=/ --exclude=proc --exclude=sys --exclude=dev --exclude=run --exclude=boot .
    ```
      - 打包后的文件会很大，估计有 10 几甚至 20 多个 G, 所以要确保有足够的空间。
        - 进一步压缩这个包为`tar.gz`格式，可以用下面的命令：
          ```shell
          gzip /tmp/ubuntu-arm64.tar   # 测试了一下，从 21G 压缩到了 13G, 用时大概 40 分钟
          ```


3. 将压缩包导出到新机器后，用 docker 导入：
    ```shell
    # 如果没有用 gzip 进行压缩，使用下面的命令。
    cat ubuntu-arm64.tar | docker import - [REPOSITORY]:[TAG]

    # 如果使用了 gzip 进行压缩，则可以使用 zcat, 再导入
    zcat ubuntu-arm64.tar.gz | docker import - [REPOSITORY]:[TAG]
    ```
   - `[REPOSITORY]:[TAG]`表示将导入的系统镜像进行标记，这个是方便以后区分用的，比如`ubuntu:arm64`
   - 在第二种情况里使用 `zcat` 是因为**它可以直接读取压缩的 `.tar.gz` 文件，无需先手动解压缩。** 这样可以更加方便快捷地处理压缩的 Docker 镜像文件。

4. 运行导入的 docker 镜像
    ```shell
    docker run -it [REPOSITORY]:[TAG] /bin/bash   # 如果报错就运行 docker run -it [REPOSITORY]:[TAG] bash 
    ```

---

## 方案二：在实体机上先分析已安装依赖，然后在 docker 内在线安装这些依赖

1. 在实体机上分析并导出已安装的`deb`依赖包列表
    ```shell
    dpkg -l > ./system_deps.txt
    ```
2. 分析并导出`pip`安装的依赖包列表
    ```shell
    pip freeze > ./system_pip_deps.txt
    ```


3. 在需要运行 docker 的机器上拉取一个基础镜像，比如`ubuntu:20.04`
    ```shell
    docker pull ubuntu:20.04
    ```
4. 用拉取的镜像创建容器
    ```shell
    docker run -it --name mycontainer ubuntu:20.04 /bin/bash
    ```
    - `mycontainer`可以改成自己喜欢的名字

5. 在要运行的 docker 机器上先编写一下 shell 脚本，用于逐行安装这些 deb 依赖 (不推荐直接把整个依赖包列表丢给 dpkg 进行安装)
    ```bash
    #!/bin/bash

    # 从 dpkg -l 的输出中提取已安装的包名
    grep '^ii' ./system_deps.txt | awk '{print $2}' | while read pkgname
    do
        echo -e "\033[1;32mStart to install [$pkgname]\033[0m"
        sudo apt install $pkgname -y
        if [ $? -ne 0 ]; then
            echo -e "\033[1;31mFailed to install [$pkgname]\033[0m"
        fi
    done
    ```
    - grep `^ii`：筛选出已安装的包（dpkg -l 输出中，已安装的包的行以 "ii" 开头）。
    - awk `{print $2}`：提取每行的第二列，即包的名称。
    - 错误处理：通过检查 apt install 命令的退出状态（$?），如果不是 0（表示成功），则打印安装失败的消息。

6. 在要运行的 docker 机器上先编写一下 shell 脚本，用于安装 pip 的依赖包
    ```bash
    #!/bin/bash

    # 读取 pip 依赖列表并安装每个包
    cat ./system_pip_deps.txt | while read pkgname
    do
        echo -e "\033[1;32mStart to install [$pkgname]\033[0m"
        pip install "$pkgname"
        if [ $? -ne 0 ]; then
            echo -e "\033[1;31mFailed to install [$pkgname]\033[0m"
        fi
    done
    ```

7. 将脚本和依赖包列表的文件都传到 docker 容器内
    ```shell
    # 从宿主机复制文件到容器
    docker cp ./system_deps.txt mycontainer:/root/system_deps.txt
    docker cp ./system_pip_deps.txt mycontainer:/root/system_pip_deps.txt
    docker cp ./install_deps.sh mycontainer:/root/install_deps.sh
    docker cp ./install_pip.sh mycontainer:/root/install_pip.sh

    # 进入容器
    docker exec -it mycontainer /bin/bash

    # 在容器内执行脚本
    cd /root
    bash install_deps.sh
    bash install_pip.sh
    ```
    - 复制文件的时候，容器的名字如果不是`mycontainer`则需要修改成对应的容器名字
    - 这里假设安装脚本的名字为`install_deps.sh`和`install_pip.sh`
