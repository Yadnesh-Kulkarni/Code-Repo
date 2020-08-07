var fs = require('fs');

exports.myReadFile = function(fileName){
    fs.readFile(fileName,function(err,data){
        return data;
    });
}