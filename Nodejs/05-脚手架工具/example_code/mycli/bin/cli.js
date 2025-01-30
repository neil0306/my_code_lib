#! /usr/bin/env node 

// ----------------- 用 commander 处理命令行参数 --------------
const {program} = require('commander');
program.option('-f, --framework <framework>', '设置框架');  // 修改 --help 里的提示信息

program
    .command('create <project> [other...]')   // 添加一个叫做 create 的执行指令，这个指令要接收超过 1 个参数的话，就要用 [other...] 来占位，注意是 3 个点
    .alias('crt')                             // 给 create 增加一个别名 crt
    .description('创建项目')                   // 在 help 信息中，为 create 指令增加说明信息
    .action(                                  // action 中写的是 create 指令真正要做的事情，这是一个回调函数
        (project, args)=>{
            // 这个代码块才是命令行的执行逻辑代码
            console.log(project);
            console.log(args);
    });

program.parse(process.argv);





