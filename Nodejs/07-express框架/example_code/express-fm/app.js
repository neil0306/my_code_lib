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
    if (!body) {                     // 空数据 
        res.status(403).json({
            error: "缺少用户信息"
        })
    }

    //  ---- 代码重新封装前 -----
    // let back = await readFile('./db.json', 'utf-8');
    // const jsonObj = JSON.parse(back);

    //  ---- 重新封装后 -----
    let jsonObj = await db.getDb();
    body.id = jsonObj.users[jsonObj.users.length - 1].id + 1;             // 获取最末尾用户的 id, +1 之后就是新用户的 id 
    jsonObj.users.push(body);                                           // 这里的 body 已经包含一个 user 需要的所有信息，所以直接使用 push 把数据添加到进去

    try {
        //  ---- 代码重新封装前 -----
        // let w =  await writeFile("./db.json", JSON.stringify(jsonObj)); // 注意写入 json 之前需要先将对象转成 json 数据

        //  ---- 重新封装后 -----
        let w = await db.saveDb(jsonObj); // 注意写入 json 之前需要先将对象转成 json 数据

        if (!w) {
            res.status(200).send({
                msg: "添加成功"
            });
        }
    }
    catch (err) {
        res.status(500).json(err);
    }
})

app.put('/:id', async (req, res) => {       // "/:id" 中的冒号表示接受 url 中的参数，这是固定写法！然后把接收的参数赋值给一个属性，这里使用的是"id" 作为属性名
    // console.log(req.params.id);    // 用 req.params 可以获取到 url 中的参数，由于前面使用了 id 作为属性名，所以这里 req.params.id 就能拿到这个属性的值
    // console.log(req.body);         // 查看客户端请求中的具体信息
    try {
        let userInfo = await db.getDb();
        let userId = Number.parseInt(req.params.id);  // Number.parseInt() 将字符串转换为整数

        let user = userInfo.users.find(item => item.id === userId);   // 用 find() 方法查找是否存在请求信息中的 user id
        if (!user) {
            res.status(403).json({
                error: "用户不存在！"
            })
        }

        // res.send(user);  // debug: 返回找到的用户的信息

        const body = req.body;
        user.username = body.username ? body.username : user.username;  // 三元运算符，如果 body.username 不为空，则将它赋值给 user.username, 否则保持不变
        user.age = body.age ? body.age : user.age;
        
        userInfo.users[userId - 1] = user;         // 注意，这在索引用户的时候，需要确保数据库中的 user 的 index 与这个用户的 id 能正确关联起来！(代码例子中，index 从 0 开始计数，id 从 1 开始计数，所以这里减 1)
        if (!await db.saveDb(userInfo)) {          // db.saveDb() 执行成功时返回 null, 取反表示 true
            res.status(201).json({
                msg: "修改成功！"
            })
        }
    }
    catch (err) {
        res.status(500).json(err);
    }
})

app.listen(3000, () => {
    console.log('Run http://127.0.0.1:3000');
});









