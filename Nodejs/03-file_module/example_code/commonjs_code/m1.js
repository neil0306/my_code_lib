// console.log(module);  // 打印 module 这个对象的所有属性


var val = "this is commonjs";
var foo = 'foo';

// 导出变量 - 写法 1:
module.exports.val = val;
module.exports.foo = foo;

// 导出变量 - 写法 2:
module.exports = {val, foo}

// 导出变量 - 写法 3:
exports.foo = foo;  
exports.val = val;
