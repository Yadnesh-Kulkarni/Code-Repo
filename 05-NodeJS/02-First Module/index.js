var http = require('http');
var dt = require('./MyFirstModule');

http.createServer(function(req,res) {
    res.write('Current Date And Time is : '+ dt.myDateTime());
    res.end();
} ).listen(8080);