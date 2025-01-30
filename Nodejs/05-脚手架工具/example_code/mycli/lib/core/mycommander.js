const myAction = require("./action")

const myCommander = function(program){
    program
        .command('create <project> [other...]')   // 添加一个叫做 create 的执行指令，这个指令要接收超过 1 个参数的话，就要用 [other...] 来占位，注意是 3 个点
        .alias('crt')                             // 给 create 增加一个别名 crt
        .description('创建项目')                   // 在 help 信息中，为 create 指令增加说明信息
        .action(                                  // action 中写的是 create 指令真正要做的事情，这是一个回调函数
            myAction
        );
}

module.exports = myCommander;   // 注意 exports 不要漏了 s