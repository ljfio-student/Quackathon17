var exec = require('child_process').exec;
var fs = require('fs');

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

// Wait for changes
fs.watch('/dev/', function (eventType, filename) {
  // This is likely one of the cameras
  if (eventType == 'change' && filename != null && filename.substring(0, 2) == 'sd') {
    var devDir =  '/dev/' + filename;
    var mountDir = '/mnt/usb-' + filename;

    fs.stat(devDir, function(error, stats) {
      if (error) {
        console.error('error: ' + devDir);
        return;
      }

      // Create a directory for the camera
      fs.mkdir(mountDir, function(error) {
        // Mount the new USB drive
        exec('mount ' + devDir + ' ' + mountDir, handleErrorOrRun(function(stdout) {

          // TODO: Get all the files
          console.log('mounted USB');

          exec('umount ' + mountDir, handleErrorOrRun(function(stderr) {
            // Unmount the disk
            fs.rmdir(mountDir, function(error) {
              console.error('error: ' + mountDir);
            })
          }));
        }));
      });
    })
  }
});