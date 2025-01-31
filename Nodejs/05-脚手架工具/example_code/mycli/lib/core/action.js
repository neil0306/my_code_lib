var {default:inquirer} = require('inquirer');
var config = require('../../config');           // 借助模块化 (其实是配置文件) 的方式方便扩充框架的选项，好处是不需要改代码，只改配置即可
var downloadFn = require('./download')

// ----- 默认是 promise + then 的方式 ---------
// const myAction = (project, args)=>{
//     inquirer.prompt([
//         {
//             type:'list',                        // 用 list 来存放选项
//             name:'framework',
//             choices: config.framework,          // 用 choices 来提供具体的选项内容
//             message:'请选择你所使用的框架：',
//         }
//     ]).then((answer)=>{
//         console.log(answer);
//     })
// }

// ----- 为了方便使用，改成 async + await 的方式 ---------
const myAction = async (project, args)=>{
    const answer = await inquirer.prompt([
        {
            type:'list',                        // 用 list 来存放选项
            name:'framework',
            choices: config.framework,          // 用 choices 来提供具体的选项内容
            message:'请选择你所使用的框架：',
        }
    ]);

    // console.log(answer);  // 验证前面 promise + then 的逻辑是否一致

    // 下载代码模板
    downloadFn(config.frameworkUrl[answer.framework], project)
}

module.exports = myAction; 