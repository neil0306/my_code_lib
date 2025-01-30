var {default:inquirer} = require('inquirer');  
    // 似乎在新版本的 inquirer 中，
    //  const inquirer = require('inquirer') 是无法使用的，原因可能是模块导出的 [不是] 默认导出（default export）


// 用 prompt 方法进行提问
inquirer.prompt([
    {
        // type 用来指定问题的类型
        type: 'input',
        name:'username',      // 用户回答的内容将存储到 "username" 这个属性中
        message:'你的名字',
    }
]).then((answer)=>{           // 用 answer 暂存用户的回答
    console.log(answer);
})
