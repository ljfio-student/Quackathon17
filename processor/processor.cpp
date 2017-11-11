#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2) {
        return -1;
    }

    Mat frame;
    Mat gray;

    vector<Rect> faces;
    vector<int> detections;

    CascadeClassifier faceCascade(argv[1]);

    //--- INITIALIZE VIDEOCAPTURE
    VideoCapture cap;

    // open the default camera using default API
    cap.open(0);

    // OR advance usage: select any API backend
    int deviceID = 0;             // 0 = open default camera
    int apiID = cv::CAP_ANY;      // 0 = autodetect default API

    // open selected camera using selected API
    cap.open(deviceID + apiID);

    // check if we succeeded
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    //--- GRAB AND WRITE LOOP
    cout << "Start grabbing" << endl << "Press any key to terminate" << endl;

    for (;;) {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        cv::resize(frame, frame, Size(1280, 720));

        // check if we succeeded
        if (frame.empty()) {
            cerr << "ERROR! blank frame grabbed\n";
            break;
        }

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        faceCascade.detectMultiScale(gray, faces, detections, 1.2, 5, 2, Size(30, 30));

        #pragma omp parallel for
        for (vector<Rect>::iterator face = faces.begin(); face != faces.end(); ++face) {
            cv::rectangle(frame, Point(face->x, face->y), Point(face->x + face->width, face->y + face->height), Scalar(0, 255, 0), 2);
        }

        // show live and wait for a key with timeout long enough to show images
        imshow("Live", frame);

        if (waitKey(5) >= 0) {
            break;
        }
    }

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
