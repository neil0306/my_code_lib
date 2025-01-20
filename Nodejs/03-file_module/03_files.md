# 使用 nodejs 操作文件
nodejs 是一个`系统级`的 runtime 环境，因此它可以调用**浏览器**提供的一些系统级的 API, 比如文件操作，网络操作等等。在本地环境中，它也有相应的 api 可以操作文件。
- API 官方文档：https://nodejs.org/docs/latest/api/fs.html

> 在 JavaScript 中，我们可以通过 `require()` 关键字引入一个`nodejs内置的模块`，然后通过模块提供的方法来操作文件。

--- 

## 如何读取文件中的内容
新建一个`read.js`文件：
```js
var fs = require('fs');  // 导入 fs 模块

// console.log("fs:", fs);   // 打印 fs 对象提供的所有方法

fs.readFile("./a.txt", 'utf-8', function(err, data){   // readFile 有三个参数：filePath, 字符编码格式，回调函数
    // 回调函数一定有两个参数：1. 读取发生错误时的 err 信息; 2. 传入函数的数据 
    console.log("err:" ,err);
    console.log("data:", data)
}); 
```

---

## 如何写入文件内容 (覆盖写)
新建一个`write.js`文件：
```js
var fs = require("fs");

// writeFile 进行 "覆盖写"
fs.writeFile("./a.txt", '6666', function(err){  // writeFile 有三个参数：文件路径，写入的字符串，回调函数
    // 此时的回调函数只接受一个参数
    console.log(err);
});
```

## 如何向文件中追加内容
新建一个`file.js`文件：
```js
var fs = require("fs");

fs.readFile("./a.txt", 'utf8', function(err, data){
    if (!err){
        var newData = data + '8888';
        fs.writeFile('./a.txt', newData, function(err){
            if (!err){
                console.log('wirte success!');
            }
        });
    }
})
```
  - 逻辑：先读取文件内容，然后在内容后面追加内容，最后写入文件。


---

# nodejs 的模块化

模块化的概念：
- 拆分代码
- 相互独立
- 导入导出

## 模块化设计规范
CommenJS
- 一般在服务端使用

AMD, UMD  (几乎没有在用的项目了)
- 一般在浏览器中使用

ECMAScript Module
- 能同时在浏览器和服务端使用

如果不进行模块化，就会出现变量作用互相覆盖的问题。