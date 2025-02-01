const express = require('express');
const fs = require('fs');
const app = express();

// // --------- case1: 使用 "回调函数" 的方式处理 GET 请求，优点是性能较高，缺点是当出现多层回调函数嵌套时会出现回调地狱，代码难以读懂 -------
// app.get('/', function (req, res) {                 // 这里接收到的 req 和 res 都是经过 express 封装过的对象，可以使用 express 提供的方法哦！
//     fs.readFile("./db.json", "utf-8", (err, data) => {
//         if (!err) {
//             // 1. 返回读取到的所有数据
//             // res.send(data);                     // 调用 express 提供的 send 方法，它可以解析流数据并发送给客户端，然后执行 end

//             // 2. 返回 json 中指定的某一部分数据
//             var back = JSON.parse(data);            // 用 node 自带的 JSON 模块解析流数据
//             res.send(back.users);                   // 只返回 users 部分的数据
//         }
//         else {
//             res.status(500).json(err);              // 一般服务端发生错误都返回状态码 500, 在返回状态码的同时，还可以将错误信息用 json 格式包装一下丢给客户端
//         }
//     });
// })

// ------------ case2: 借助 promisify 避免使用回到函数 ---------------
const { promisify } = require('util');             // 借助 promisify 使用 async/await 来避免回调地狱
const readFile = promisify(fs.readFile);           // 使用 promisify 将 fs.readFile 变成 promise 流程

app.get('/', async function(req, res){             // 使用 async/await 异步操作，避免回调函数嵌套
    try{
        let back = await readFile('./db.json', 'utf-8');          // 这个 readFile 是 promise 的，也就是在异步处理的过程中，必须等待 readFile 执行结束才会继续往下执行
        const jsonObj = JSON.parse(back).users;
        res.send(jsonObj);
    }
    catch(err){
        res.status(500).json(err);
    }
})

// 由于 express 本质上只是对 http 这个核心模块进行了扩展，所以类似监听这种操作还是需要手动完成的
app.listen(3000, () => {
    console.log('Run http://127.0.0.1:3000');
});









