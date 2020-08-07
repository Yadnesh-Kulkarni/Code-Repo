var MongoClient = require('mongodb').MongoClient
var url = "mongodb://127.0.0.1:27017/";

MongoClient.connect(url,{ useNewUrlParser : true },function(err,db){
    if(err) throw err;
    var dbo = db.db("myDb");
    var query = { address : "Karve Road" };
    dbo.collection("customers").find().toArray(function(err,result){
        if (err) throw err;
        console.log(result);
        db.close();
    });
});