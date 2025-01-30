var {default:inquirer} = require('inquirer');
const config = require('../../config');           // 借助模块化 (其实是配置文件) 的方式方便扩充框架的选项，好处是不需要改代码，只改配置即可

const myAction = (project, args)=>{
    inquirer.prompt([
        {
            type:'list',                        // 用 list 来存放选项
            name:'framework',
            choices: config.framework,          // 用 choices 来提供具体的选项内容
            message:'请选择你所使用的框架：',
        }
    ]).then((answer)=>{
        console.log(answer);
    })
}

module.exports = myAction; 