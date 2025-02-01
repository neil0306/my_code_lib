var http = require('http');
var fs = require('fs'); 
var url = require('url')     // 导入处理 url 的包
var server = http.createServer();

server.listen(8080, function(){
    console.log('http://127.0.0.1:8080')
})

server.on('request', function(req, res){
    // console.log(req.method)   // 查看客户端的请求方法

    if (req.method == 'GET'){
        // ---- 以访问 127.0.0.1:8080/user?id=123 为例
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

    }
})

