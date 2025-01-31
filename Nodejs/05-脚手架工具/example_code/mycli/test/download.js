const download = require('simple-git');  // require the library, 注意 download 是一个函数
try{
    download().clone('https://github.com/xiedajian/egg-template.git', './xxx');  // 将仓库克隆到本地的 xxx 文件夹里
    console.log("repo clone successully!");
}
catch(err){
    console.log("fail to clone repo: ", err);
}



