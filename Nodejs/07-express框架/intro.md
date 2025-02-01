# express 框架
官方网站：https://expressjs.com/

express 是一个基于 Node.js 平台的 web 应用开发框架，它提供了一系列强大的特性帮助你创建各种 Web 应用。它构建于 Node.js 的核心模块 http 之上，并通过 Connect 中间件进行扩展，简化了 Web 应用程序的开发过程。

Express 的核心特性：
- 路由：Express 提供了灵活的路由机制，可以根据 HTTP 方法（GET、POST、PUT、DELETE 等）和 URL 路径定义不同的处理程序。
- 中间件 (`middleware`)：中间件函数可以访问请求对象 (req)、响应对象 (res) 以及应用程序的 "请求 - 响应" 周期中的下一个中间件函数。这使得可以执行各种任务，例如日志记录、身份验证、数据处理等。 
- 模板引擎：Express 支持多种模板引擎，例如 Pug、EJS 等，可以动态生成 HTML 页面。
- 静态文件服务：Express 可以轻松地提供静态文件，例如图像、CSS 和 JavaScript 文件。

express 适用于：
- 传统 web 网站
- API 接口服务器
- 服务端渲染中间层 (提供了很多中间件`middleware`)
- 开发辅助工具
- 自定义集成框架

--- 

安装 express：
```bash
npm install express --save
```

Hello World 示例：
```javascript
const express = require('express')
const app = express()
const port = 3000

app.get('/', (req, res) => {
  res.send('Hello World!')
})

app.listen(port, () => {
  console.log(`Example app listening on port ${port}`)
})
```
