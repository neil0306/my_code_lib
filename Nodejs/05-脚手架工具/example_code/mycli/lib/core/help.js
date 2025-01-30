const myhelp = function(program){
    program.option('-f, --framework <framework>', '设置框架');  // 修改 --help 里的提示信息
}

module.exports = myhelp;   // 注意 exports 不要漏了 s