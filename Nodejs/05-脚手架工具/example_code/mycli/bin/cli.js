#! /usr/bin/env node 

// ----------------- 用 commander 处理命令行参数 --------------
const {program} = require('commander');
const myHelp = require('../lib/core/help');  // 在 lib/core/help.js 中对 help 信息的管理进行了模块化封装
const myCommander = require('../lib/core/mycommander');

myHelp(program);
myCommander(program);

program.parse(process.argv);





