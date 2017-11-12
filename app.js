var exec = require('child_process').exec;
var spawn = require('child_process').spawn;
var fs = require('fs');
var async = require('async');
var path = require('path');
var port = 8080;
var express = require('express');
var app = express();
var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');

var faceModel = require('./face-model.json');

// Connection URL
var url = 'mongodb://localhost:27017/quackathon';

app.use(express.static(path.join(__dirname, '')));

// Use connect method to connect to the Server
app.set('view engine', 'ejs');

app.get('/home', function (req, res) {
  MongoClient.connect(url, function (err, db) {
    if (err) {
      console.log(err);
    }

    db.collection('videos').find().toArray(function (err, results) {
      res.render('index', {
        videos: results
      });
    });

    db.close();
  });
});

function handleErrorOrRun(callback) {
  return function (error, stdout, stderr) {
    if (error) {
      console.error('exec: ' + error);
    } else {
      if (stderr) {
        console.error('stderr: ' + stderr);
      }

      callback(stdout);
    }
  }
}

function recursiveList(directory, callback) {
  fs.readdir(directory, function (err, files) {
    async.each(files, function (file, cb) {
      var fullname = path.join(directory, file);

      fs.stat(fullname, function (err, stat) {
        if (err) {
          cb();
          return;
        }

        if (stat.isDirectory()) {
          console.log('recursive: ' + fullname);
          recursiveList(fullname);
        } else if (stat.isFile()) {
          console.log(fullname);

          var type = fullname.split(".");
          var thumb;

          if (type[2]) {
            thumb = type[1].split("/");
          }

          if (type[1] == "avi") {
            MongoClient.connect(url, function (err, db) {
              if (err) {
                console.log(err);
              }

              var filename = file.split(".");
              filename = filename[0];

              var upBefore = db.collection('videos').findOne({ fileName: filename }, function (err, result) {
                assert.equal(err, null)

                if (!result) {

                  db.collection('videos').insertOne({ fileName: filename }, function (err, result) {
                    assert.equal(err, null);
                    db.close();

                    var resultFileName = '/home/pi/Quackathon17/videos/' + file;

                    var readStream = fs.createReadStream(fullname);
                    var writeStream = fs.createWriteStream(resultFileName);

                    readStream.pipe(writeStream);

                    readStream.on("end", function () {
                      var classifierFile = path.join('./processor', 'haarcascade_frontalface_default.xml');

                      var analyse = spawn(path.join('./processor', 'processor'), ['-c', classifierFile, '-f', resultFileName, '-m', 'model.yml']);

                      analyse.stdout.on('data', (data) => {
                        console.log(`ps stdout: ${data}`);
                      });

                      analyse.stderr.on('data', (data) => {
                        console.error(`ps stderr: ${data}`);
                      });

                      analyse.stdin.write(JSON.stringify(faceModel));

                      analyse.on('close', (code) => {
                        if (code !== 0) {
                          console.log(`ps process exited with code ${code}`);
                        }

                        analyse.stdin.end();
                      });

                      console.log("file " + file + " moved");
                    });
                  })
                }
              });
            });
          } else if (type[2] == "jpg" && thumb[0] == "thumbnail") {
            var readStream = fs.createReadStream(fullname);
            var writeStream = fs.createWriteStream('/home/pi/Quackathon17/thumbnails/' + file);

            readStream.pipe(writeStream);

            readStream.on("end", function () {
              console.log("thumbnail " + file + " moved");
            });
          }
        }

        cb();
      })
    }, callback);
  });
}

// Wait for changes
fs.watch('/dev/', function (eventType, filename) {
  // This is likely one of the cameras
  if (eventType == 'change' && filename != null && filename.substring(0, 2) == 'sd') {
    var devDir = '/dev/' + filename;
    var mountDir = '/mnt/usb-' + filename;

    fs.stat(devDir, function (error, stats) {
      if (error) {
        console.error('dev stat error: ' + devDir);
        return;
      }

      fs.stat(mountDir, function (error) {
        if (!error) {
          console.error('mount shouldn\'t exist');
          return;
        }

        // Create a directory for the camera
        fs.mkdir(mountDir, function (error) {
          if (error) {
            console.error('mount create error: ' + error);
            return;
          }

          // Mount the new USB drive
          exec('mount ' + devDir + ' ' + mountDir, handleErrorOrRun(function (stdout) {
            // TODO: Get all the files
            console.log('mounted USB');
            recursiveList(mountDir, function (error) {
              if (error) {
                console.error('recursive: ' + error);
              }

              exec('umount ' + mountDir, handleErrorOrRun(function (stderr) {
                // Unmount the disk
                fs.rmdir(mountDir, function (error) {
                  console.error('mount remove error: ' + mountDir);
                });
              }));
            });
          }));
        });
      });
    });
  }
});

var server = app.listen(port, function () {
  console.log('Server listening on port : ' + port);
});
