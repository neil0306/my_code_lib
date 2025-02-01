var http = require('http');
var fs = require('fs'); 
var url = require('url');     // 导入处理 url 的包
const { log } = require('console');
var server = http.createServer();

server.listen(8080, function(){
    console.log('http://127.0.0.1:8080')
})

server.on('request', function(req, res){
    // console.log(req.method)   // 查看客户端的请求方法

    if (req.method == 'GET'){
        // ---- 以访问 127.0.0.1:8080/user?id=123 为例，介绍 [请求头的传参方法]
        // console.log(req.url)    // 如果客户端访问的时候带参数的话，这里会打印出 /user?id=123
        // console.log(url.parse(req.url, true))  // 借助 url 模块可以解析客户端请求时附带的参数，true 表示处理参数字符串，此时会返回一个对象
        console.log(url.parse(req.url,true).query.id)  // 这里会打印客户端请求参数中的具体参数，打印出 123
        
        if (req.url == '/'){
            fs.readFile('./index.html', 'utf-8', function(err, data){
                res.write(data);
                res.end();
            })
        }
        else{
            fs.readFile('./neil.jpeg', function(err, data){  
                res.end(data);         
            })
        }
    }
    else if (req.method == 'POST'){
        // ----- 请求体中的传参，需要使用 net 这个比 http 更底层的模块里定义好的 [事件] 进行特定的处理，在官方文档中需要去 net 章节才能找到相关信息 ---
        var data = ''                   // 用一个变量作为缓存，接收所有的数据，var 类型的作用域是"函数作用域或全局作用域", const 的作用域是"代码块级的作用域"
        req.on('data', function(d){     // data 事件：每当有数据传过来的时候就会触发这里的回调函数
            // console.log(d)           // 打印出 16 进制的流数据，但是这里并不是所有的数据，可能是一段段的数据包

            data += d;                  // 拼接每一次传过来的数据
        })
        
        req.on('end', function(){       // 当数据传送完毕时，就会触发 end 事件的回调函数
            console.log(require('querystring').parse(data));  // 打印出一个对象的信息
                                                              // 借助 querystring 这个模块解析流数据
        })
    }
})

