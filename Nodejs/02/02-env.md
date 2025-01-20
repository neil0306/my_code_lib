# 安装 nodejs
MacOS & Windows:
 - 去[官网](https://nodejs.org/en/download)下载安装包进行安装即可，推荐安装 LTS 版本。

Ubuntu:
```shell
sudo apt update
sudo apt install node.js
```

检测安装
```shell
node -v
```

---

# 测试一下能否运行 js 代码
新建一个 `index.js` 文件，内容如下
```js
var a = 1;
console.log(a);
```
- 运行方法：在终端中运行`node index.js`，如果输出了 `1`，则说明安装成功。
