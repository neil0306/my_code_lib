var fs = require('fs');

// console.log("fs:", fs);   // 打印 fs 对象提供的所有方法

fs.readFile("./a.txt", 'utf-8', function(err, data){   // readFile 有三个参数：filePath, 字符编码格式，回调函数
    // 回调函数一定有两个参数：1. 读取发生错误时的 err 信息; 2. 传入函数的数据 
    console.log("err:" ,err);
    console.log("data:", data)
}); 













