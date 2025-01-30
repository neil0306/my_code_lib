var fs = require("fs");

// writeFile 进行 "覆盖写"
fs.writeFile("./a.txt", '6666', function(err){  // writeFile 有三个参数：文件路径，写入的字符串，回调函数
    // 此时的回调函数只接受一个参数
    console.log(err);
})

