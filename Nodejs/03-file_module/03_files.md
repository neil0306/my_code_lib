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

如果不进行模块化，就会出现**变量作用域互相覆盖**的问题。

> 对于 nodejs 来说，一个 `.js` 文件就是一个模块，每个模块都有自己的作用域，不会互相影响。

---
## ECSMAScript Module 的使用
ECSMAScript 的模块后缀名为`.mjs`.
> 在某些环境下，如果不适用 `.mjs` 后缀名的话可能会报错，如果希望直接使用`.js`后缀名来定义模块，此时可以在`package.json`文件中添加`"type": "module"`字段来指定使用模块化。

首先创建一个模块`ex.mjs`:
```js
var val = 'ex data'

export {val as value}   
    // 将模块内的指定变量 (指定内容) 导出，此时{}里面的变量就可以被其他模块使用了
    // 使用 `as` 关键词可以只在导出的时候改变变量名 
```

然后在另一个模块`im.mjs`中引入这个模块：
```js
import {value as vv} from './ex.mjs'  
    // 导入 ex.mjs 模块中的 value 变量，注意，因为导出的时候改了名字，所以这里也需要用导出时使用的名字
    // 导入的时候同样可以使用 `as` 关键词改变变量名
console.log(vv);        // 使用导入时修改过的变量名
```
 - 然后在终端中执行`node im.mjs`，就可以看到输出了`ex data`。


导出和导入变量的使用要求：
```md
- 如果 export 的时候使用 {} 来包住变量，那么 import 的时候也要使用 {} 来包住变量

- 如果 export 的时候使用 `as` 关键词更改了变量名，那么 import 的时候也要使用 修改后的变量名
    - import 和 export 都可以使用 `as` 关键字来改名

- 如果只导出一个变量，则可以使用 `export default 变量名` 来导出，导入的时候可以不使用 {} 来包住变量
    - import 的时候可以直接用 `import 变量名 from '模块名'` 来导入
```


---

## CommonJS Module 的使用

查看 commonjs 的 module 信息：
```js
console.log(module);  // 打印 module 对象提供的所有方法

//----------------- 在 mac 的终端中得到以下输出 --------------
{
  id: '.',
  path: '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/my_code_lib/Nodejs/03-file_module/example_code/commonjs_code',
  exports: {},    // 对模块进行导出时，其实就是给这个属性赋值
  filename: '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/my_code_lib/Nodejs/03-file_module/example_code/commonjs_code/m1.js',
  loaded: false,
  children: [],
  paths: [
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/my_code_lib/Nodejs/03-file_module/example_code/commonjs_code/node_modules',
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/my_code_lib/Nodejs/03-file_module/example_code/node_modules',
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/my_code_lib/Nodejs/03-file_module/node_modules',
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/my_code_lib/Nodejs/node_modules',
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/my_code_lib/node_modules',
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/My-Git-Repo/node_modules',
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/GitHub/node_modules',
    '/Users/nullptr/Library/Mobile Documents/com~apple~CloudDocs/node_modules',
    '/Users/nullptr/Library/Mobile Documents/node_modules',
    '/Users/nullptr/Library/node_modules',
    '/Users/nullptr/node_modules',
    '/Users/node_modules',
    '/node_modules'
  ],
  [Symbol(kIsMainSymbol)]: true,
  [Symbol(kIsCachedByESMLoader)]: false,
  [Symbol(kURL)]: undefined,
  [Symbol(kFormat)]: undefined,
  [Symbol(kIsExecuting)]: true
}
```
- 可以看到，module 这个对象里有一个`exports`属性，我们在进行模块的导出的时其实就是在给这个属性赋值。

导出变量的例子：
```js
// console.log(module);  // 打印 module 这个对象的所有属性


var val = "this is commonjs";
var foo = 'foo';

// 导出变量 - 写法 1:
module.exports.val = val;
module.exports.foo = foo;

// 导出变量 - 写法 2:
module.exports = {val, foo}

// 导出变量 - 写法 3:
exports.foo = foo;  
exports.val = val;
```

导入变量的例子：
```js
var m1 = require('./m1.js');
console.log(m1);
```

