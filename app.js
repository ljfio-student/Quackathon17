var exec = require('child_process').exec;
var fs = require('fs');
var async = require('async');

function handleErrorOrRun(callback) {
  return function(error, stdout, stderr) {
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

function recursiveList(path, callback) {
  fs.readdir(path, function(err, files) {
    async.each(files,function(file, cb) {
      fs.stat(file, function(err, stat) {
        if (err) {
          cb();
          return;
        }

        if (stat.isDirectory()) {
          console.log('recursive: ' + file);
          recursiveList(file);
        } else if (stat.isFile()) {
          console.log(file);
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
    var devDir =  '/dev/' + filename;
    var mountDir = '/mnt/usb-' + filename;

    fs.stat(devDir, function(error, stats) {
      if (error) {
        console.error('dev stat error: ' + devDir);
        return;
      }

      fs.stat(mountDir, function(error) {
        if (error) {
          console.error('mount stat error: ' + mountDir);
          return;
        }

        // Create a directory for the camera
        fs.mkdir(mountDir, function(error) {
          if (error) {
            console.error('mount create error: ' + err);
            return;
          }

          // Mount the new USB drive
          exec('mount ' + devDir + ' ' + mountDir, handleErrorOrRun(function(stdout) {

            // TODO: Get all the files
            console.log('mounted USB');
            recursiveList(mountDir, function(error) {
              if (error) {
                console.error('recursive: ' + error);
              }

              exec('umount ' + mountDir, handleErrorOrRun(function(stderr) {
                // Unmount the disk
                fs.rmdir(mountDir, function(error) {
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