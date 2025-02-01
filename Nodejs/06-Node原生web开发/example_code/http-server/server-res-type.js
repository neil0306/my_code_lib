// 1. 导入 node 原生的 http 模块
var http = require('http');

// 2. 获取 http server 实力对象
var server = http.createServer();
server.listen(8080, function(){    // 用 listen 方法监听本机系统端口，只要不与其他服务冲突的端口均可
    console.log('http://127.0.0.1:8080')
})


// ----- 响应纯文本 并 指定响应内容的类型 ---------
// server.on('request', function(req, res){   // 用 on 方法监听客户端的请求事件
//     console.log('666');                    // 测试 http 服务器是否接收到客户端请求

//     res.setHeader('Content-type', 'text/plain;charset=utf-8');   // Content-type 用来说明我要设置的是内容的类型，text/plain;charset=utf-8 表示具体返回的是纯文本信息并且使用 utf-8 字符编码，
    
//     res.write('你好');                      // 测试返回给客户端的响应信息
//     res.end();                             // 断开与客户端的当前连接，如果不断开的话，客户端会认为服务器仍然需要继续处理某些事情，从而卡在加载的页面
// })   


// --------- 响应 html 文件
var fs = require('fs');    // 借助 node 提供的 fs 模块去读取本地文件

server.on('request', function(req, res){
    if (req.url == '/'){
        fs.readFile('./index.html', 'utf-8', function(err, data){
            res.write(data);  // 注意，我们是在 html 文件的内容中定义了响应类型以及字符编码，所以这里不用设置
            res.end();
        })
    }
    else{
        fs.readFile('./neil.jpeg', function(err, data){  // 由于读取的是图片，我们可以不指定读取的字符编码，直接写回调函数即可
            res.end(data);          // 可以直接将数据丢给 end 方法，它的作用其实跟上面先 writer 再 end 是一样的
        })
    }
})

