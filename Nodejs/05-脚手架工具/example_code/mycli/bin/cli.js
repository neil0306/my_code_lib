#! /usr/bin/env node 

// console.log('mycli');          // 用来测试是否能在任意目录运行当前脚本

// 获取命令行参数
// console.log(process.argv[2]);  // 打印输入的参数
// console.log(process.argv);     // 打印捕获参数的所有信息


if (process.argv[2] == '--help'){
    console.log("参数捕获成功");
}


