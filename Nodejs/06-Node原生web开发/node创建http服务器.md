# 创建 HTTP 服务
首先去 Nodejs 官方网站的找到自己当前使用的 nodejs 版本的文档：
- 比如 23.7.0 的文档地址：https://nodejs.org/docs/latest/api/http.html

基础 http 服务器的代码，创建`http-server/server.js`：
```js
// 1. 导入 node 原生的 http 模块
var http = require('http');

// 2. 获取 http server 实力对象
var server = http.createServer();
server.listen(8080, function(){    // 用 listen 方法监听本机系统端口，只要不与其他服务冲突的端口均可
    console.log('http://127.0.0.1:8080')
})

server.on('request', function(req, res){   // 用 on 方法监听客户端的请求事件
    console.log('666');                    // 测试 http 服务器是否接收到客户端请求
    res.write('Hello');                     // 测试返回给客户端的响应信息
    res.end();                             // 断开与客户端的当前连接，如果不断开的话，客户端会认为服务器仍然需要继续处理某些事情，从而卡在加载的页面
})   
```
- `server.listen(port, callback)`：监听本机系统端口，只要不与其他服务冲突的端口均可

- `server.on('request', callback)`：监听客户端的请求事件
  - `req`：请求对象，包含了客户端的所有请求的信息
  - `res`：响应对象，用来给客户端发送响应信息

- 为了避免每次修改服务器都手动重新启动 http 服务，我们可以用 `nodemon` 这个工具来自动检测服务器代码的变化，然后自动重启服务。
  - 安装 `nodemon`：`npm install -g nodemon`.  
    - `-g` 表示全局安装
    - 安装好之后，启动服务就不再使用 `node server.js`, 而是`nodemon server.js`命令。
