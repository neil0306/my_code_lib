const myAction = (project, args)=>{
    // 这个代码块才是命令行的执行逻辑代码
    console.log(project);
    console.log(args);
}

module.exports = myAction; 