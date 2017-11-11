/*

code written by Christina && Rhys

*/


var port = 8080;
var moment = require('moment')
var express = require('express');
var app = express();
var path = require('path');
var formidable = require('formidable');
var fs = require('fs');
var util = require('util');
var moment = require('moment')
var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');

// Connection URL
var url = 'mongodb://localhost:27017/quackathon';
//Array to store data about connected devices
// Use connect method to connect to the Server
 app.set('view engine', 'ejs');

 app.use(express.static(path.join(__dirname, '')));

 app.get('/home', function(req, res){
   res.sendFile('index.html', {root:__dirname});
   //connect to db
   MongoClient.connect(url, function(err, db) {
     if(err){
       console.log(err);
     }

    //Get all videos
    //db.collection('videos').find().toArray(function(err, results){
      //render page with data
       //
       //res.render('index', {
            //videos:results
        //});
    //});
     //close db
     db.close();

   });


 });


 /*code required somewhere
      //Check to see if file exists
      var conBefore = db.collection('videos').findOne({fileName:fileName}, function(err, result){

         if(!result){
         //Then insert new video
         db.collection('videos').insertOne( {fileName:fileName}, function(err, result) {
            assert.equal(err, null);
            db.close();
         }
     });

*/

 app.post('/upload', function(req, res){

   // create an incoming form object
   var form = new formidable.IncomingForm();

   // specify that we want to allow the user to upload multiple files in a single request
   form.multiples = true;

   // store all uploads in the /uploads directory
   form.uploadDir = path.join(__dirname, '/videos');

   // every time a file has been uploaded successfully,
   // rename it to it's orignal name
   //Add info to db
   form.on('file', function(field, file) {
     fs.rename(file.path, path.join(form.uploadDir, file.name));
     var d = new Date();
     var device = file.name.substring(0,file.name.length-4);
     //connect to db
     MongoClient.connect(url, function(err, db) {
       if(err){
         console.log(err);
       }

      //Check to see if file has been uploaded for device before
      var upBefore = db.collection('devices').findOne({deviceName:device},function(err, result) {
         assert.equal(err, null)
         if(result){
           //if so then update device upload date
           db.collection('devices').updateOne( {deviceName:device},{$set : {uploadDate:d}}, function(err, result) {
              assert.equal(err, null);
              db.close();

            });
         }else{
           //Then insert new device
           db.collection('devices').insertOne( {deviceName:device,uploadDate:d}, function(err, result) {
              assert.equal(err, null);
              db.close();

            });
         }
       });
     });
   });

   // log any errors that occur
   form.on('error', function(err) {
     console.log('An error has occured: \n' + err);
   });

   // once all the files have been uploaded, send a response to the client
   form.on('end', function() {
     res.end('success');
   });

   // parse the incoming request containing the form data
   form.parse(req);

 });

 var server = app.listen(port, function(){
   console.log('Server listening on port : ' +port);
 });

