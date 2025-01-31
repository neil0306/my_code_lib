var download = require('simple-git');
const downloadFn = function(url, projectName){
    try{
        download().clone(url, projectName);
        console.log('Repository cloned successfully');
    }
    catch (err){
        console.log("fail to clone repo, err msg: ", err);
    }
}
module.exports = downloadFn;