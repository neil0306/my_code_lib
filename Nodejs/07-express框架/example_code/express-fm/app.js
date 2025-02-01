const express = require('express');
const fs = require('fs');
const app = express();

const { promisify } = require('util');             // 借助 promisify 使用 async/await 来避免回调地狱
const readFile = promisify(fs.readFile);           // 使用 promisify 将 fs.readFile 变成 promise 流程
const writeFile = promisify(fs.writeFile);

const db = require('./db');

// app.use(express.urlencoded());                       // 用 express 提供的 urlencoded() 方法来表明服务端可以处理 urlencoded 这种类型的数据
app.use(express.json());                             // 增加可处理的数据格式

app.get('/', async function (req, res) {
    try {

        //  ---- 代码重新封装前 -----
        // let back = await readFile('./db.json', 'utf-8');          // 这个 readFile 是 promise 的，也就是在异步处理的过程中，必须等待 readFile 执行结束才会继续往下执行
        // const jsonObj = JSON.parse(back).users;
        // res.send(jsonObj);

        //  ---- 重新封装后 -----
        let back = await db.getDb();
        res.send(back.users);
    }
    catch (err) {
        res.status(500).json(err);
    }
})

app.post('/', async function (req, res) {
    let body = req.body;
    if (!body){                     // 空数据 
        res.status(403).json({
            error: "缺少用户信息"
        })
    }

    //  ---- 代码重新封装前 -----
    // let back = await readFile('./db.json', 'utf-8');
    // const jsonObj = JSON.parse(back);
    
    //  ---- 重新封装后 -----
    let jsonObj = await db.getDb();
    body.id = jsonObj.users[jsonObj.users.length-1].id + 1;             // 获取最末尾用户的 id, +1 之后就是新用户的 id 
    jsonObj.users.push(body);                                           // 这里的 body 已经包含一个 user 需要的所有信息，所以直接使用 push 把数据添加到进去
    
    try{
        //  ---- 代码重新封装前 -----
        // let w =  await writeFile("./db.json", JSON.stringify(jsonObj)); // 注意写入 json 之前需要先将对象转成 json 数据
        
        //  ---- 重新封装后 -----
        let w =  await db.saveDb(jsonObj); // 注意写入 json 之前需要先将对象转成 json 数据
        
        if (!w){
            res.status(200).send({
                msg:"添加成功"
            });
        }
    }
    catch(err){
        res.status(500).json(err);
    }
})

app.listen(3000, () => {
    console.log('Run http://127.0.0.1:3000');
});









