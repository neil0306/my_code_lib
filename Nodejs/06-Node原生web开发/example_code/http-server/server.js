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





