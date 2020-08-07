var MongoClient = require('mongodb').MongoClient
var url = "mongodb://localhost:27017/";

MongoClient.connect(url,{ useNewUrlParser : true },function(err,db){
    if(err) throw err;
    var dbo = db.db("myDb");
    var myObj = { name: "Test", address: "Karve Road"};
    dbo.collection("customers").insertOne(myObj,function(err,res) {
        if(err) throw err;
        console.log("1 Document Inserted");
        db.close();
    })
})