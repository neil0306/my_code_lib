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

app.use(express.urlencoded());                       // 用 express 提供的 urlencoded() 方法来表明服务端可以处理 urlencoded 这种类型的数据
app.use(express.json());                             // 增加可处理的数据格式

app.get('/', async function (req, res) {             // 使用 async/await 异步操作，避免回调函数嵌套
    try {
        let back = await readFile('./db.json', 'utf-8');          // 这个 readFile 是 promise 的，也就是在异步处理的过程中，必须等待 readFile 执行结束才会继续往下执行
        const jsonObj = JSON.parse(back).users;
        res.send(jsonObj);
    }
    catch (err) {
        res.status(500).json(err);
    }
})


app.post('/', async function (req, res) {
    console.log(req.headers);   // 打印客户端请求的所有 head 信息，从中可以知道客户端发送过来的数据是什么格式类型
                                // 如果我们测试的时候用的是 "x-www-form-urlencoded", 那么打印信息中我们会看到 'content-type': 'application/x-www-form-urlencoded',
                                //  根据这个信息，我们需要 app 初始化之后添加 app.use(express.urlencoded()) 来表明服务器需要处理这个格式的数据
                                //  如果需要处理更多格式，则继续增加其他格式的声明，比如 json 格式 app.use(express.json())

    console.log(req.body);      // express 提供的 body 方法可以直接拿到客户端发过来的所有请求数据
})

// 由于 express 本质上只是对 http 这个核心模块进行了扩展，所以类似监听这种操作还是需要手动完成的
app.listen(3000, () => {
    console.log('Run http://127.0.0.1:3000');
});









