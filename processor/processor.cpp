#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "facial.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        return -1;
    }

    Facial facial;
    facial.setDetectionClassifier(argv[1]);

    // open selected camera using selected API
    cv::VideoCapture cap;
    cv::Mat frame;
    cv::Scalar green(0, 255, 0);

    int deviceID = 0;        // 0 = open default camera
    int apiID = cv::CAP_ANY; // 0 = autodetect default API

    cap.open(deviceID + apiID);

    // check if we succeeded
    if (!cap.isOpened())
    {
        std::cerr << "ERROR! Unable to open camera\n";
        return -1;
    }

    //--- GRAB AND WRITE LOOP
    std::cout << "Start grabbing" << std::endl
              << "Press any key to terminate" << std::endl;

    for (;;)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);

        // check if we succeeded
        if (frame.empty())
        {
            std::cerr << "FAILURE" << std::endl;
            break;
        }

        auto faces = facial.detect(frame);

#pragma omp parallel for
        for (auto face = faces.begin(); face != faces.end(); ++face)
        {
            cv::Point start(face->x, face->y), finish(face->x + face->width, face->y + face->height);

            cv::rectangle(frame, start, finish, green, 2);
        }

        // show live and wait for a key with timeout long enough to show images
        cv::imshow("Live", frame);

        if (cv::waitKey(5) >= 0)
        {
            break;
        }
    }

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
