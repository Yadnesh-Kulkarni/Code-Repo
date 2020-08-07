var http = require('http');
var url = require('url');

http.createServer(function(req,res) {
    res.write(req.url);
    res.end();
} ).listen(8080);