
var download = require('simple-git');
const ora = require('ora');
const chalk = require('chalk');

const downloadFn = function(url, projectName){
    const spinner = ora().start();
    spinner.text = 'cloning repo...';

    // 使用回调函数方式来实现等待交互 (用异步的方式会立即返回，从而跳过这个回显)
    download().clone(url, projectName, (err, result) => {
        if (err) {
            spinner.fail(`fail to clone repo, err msg: ${err}`);
            return;
        }
        spinner.succeed('Repository cloned successfully');
        console.log(chalk.green("Done! \nYou can run: "));
        console.log(chalk.blue.bold('\tcd ' + projectName));
        console.log(chalk.yellow.bold('\tnpm install '));
        console.log(chalk.rgb(123,234,66).bold('\tnpm run dev '));
        
    });
}
module.exports = downloadFn;