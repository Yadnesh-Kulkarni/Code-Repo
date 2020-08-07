var http = require('http');
var fs = require('./fileSys');

http.createServer(function(req,res) {
    var test = fs.myReadFile('demo.html');
    res.write(test.data);
    res.end();
} ).listen(8080);