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
    var mountDir = '~/Quackathon17/mounts/usb-' + filename;

    // Create a directory for the camera
    exec('mkdir ' + mountDir, handleErrorOrRun(function(stdout) {
      // Mount the new USB drive
      exec('mount ' + devDir + ' ' + mountDir, handleErrorOrRun(function(stdout) {
        // TODO: Get all the files
        console.log('mounted USB');

        exec('umount' + mountDir, handleErrorOrRun(function(stderr) {
          // Unmount the disk
        }));
      }));
    }));
  }
});