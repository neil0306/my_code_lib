


# Mac mini 上使用 docker-compose 搭建 rustdesk-server[待测试]

```dockerfile
version: '3'

services:
  hbbs:
    container_name: hbbs
    image: rustdesk/rustdesk-server:latest
    command: hbbs
    volumes:
      - ./data:/root
    ports:
      - "21115:21115"
      - "21116:21116"
      - "21118:21118"
      - "21116:21116/udp"
    depends_on:
      - hbbr
    restart: unless-stopped

  hbbr:
    container_name: hbbr
    image: rustdesk/rustdesk-server:latest
    command: hbbr
    volumes:
      - ./data:/root
    ports:
      - "21117:21117"
      - "21119:21119"
    restart: unless-stopped
```
- 来源：https://linux.do/t/topic/256684



# 查看监听的端口
使用`lsof`, 会列出哪个程序监听哪些端口
```shell
sudo lsof -i -P -n 
```

使用`netstat`, 下面这个命令只会列出端口和协议，不会告诉你是哪个程序
```shell
sudo netstat -tuln 
```


