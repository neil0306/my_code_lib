const ora = require('ora');
const spinner = ora().start();

spinner.text = 'loading...';
setTimeout(()=>{
    console.log('\n111');
    // spinner.succeed("process ended.");  // 运行成功提示
    // spinner.fail("process ended.");  // 运行失败提示
    spinner.info("process ended.");  // 提示一些信息，不表示成功或失败

}, 1000)