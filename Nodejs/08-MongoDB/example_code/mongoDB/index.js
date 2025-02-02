const { MongoClient } = require('mongodb');
const client = new MongoClient('mongodb://127.0.0.1:27017');

// 将数据库连接的基础操作封装一下
const clientFun = async function (c) {
    await client.connect();
    const db = client.db('mydata');
    return cc = db.collection(c)

}

const main = async () => {
    var cc = await clientFun("cc");

    // ------ 查 ------
    // var d = await cc.find();
    // console.log(await d.toArray());              // find() 返回的是多条数据，此时返回的是一个'游标'对象，我们想看具体数据的话可以转换为 array
    // var d = await cc.findOne({age:{$gt:15}})        
    // console.log(d)

    // ------ 增 ------
    // var d = await cc.insertOne({username:"neil", age:60})
    // var d = await cc.insertMany([
    //     {username:"neil1", age:10},
    //     {username:"neil2", age:20},
    //     {username:"neil3", age:30},
    //     {username:"neil4", age:40}
    // ])
    // console.log(d)

    // ------ 改 ------
    // var d = await cc.updateOne({age:{$gt:15}},{$set:{username:'lisi'}})
    // var d = await cc.updateMany({age:{$gt:15}},{$set:{username:'lisi'}})
    // console.log(d)

    // ------ 删 ------
    // var d = await cc.deleteOne({age:{$lt:{age:10}}})    // $lt 表示 "小于"
    var d = await cc.deleteMany({age:{$gt:{age:60}}})    // $lt 表示 "小于"
    console.log(d)
    

}

main().finally(()=>{
    client.close();
})


