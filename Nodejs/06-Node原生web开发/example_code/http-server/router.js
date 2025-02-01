var url = require('url');
var fs = require('fs'); 
var controller = require("./controller")

module.exports = (req, res)=>{
    if (req.method == 'GET'){
        // console.log(url.parse(req.url,true).query.id)
        if (req.url == '/'){
            controller.index(res);
        }
        else{
            fs.readFile('./neil.jpeg', function(err, data){  
                res.end(data);         
            })
        }
    }
    else if (req.method == 'POST'){
        var data = ''                   
        req.on('data', function(d){    
            data += d;           
        })
        
        req.on('end', function(){       
            controller.user(require('querystring').parse(data), res);
        })
    }
}