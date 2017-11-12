#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <vector>

#ifndef _FACIAL_H
#define _FACIAL_H

class Facial
{
  private:
    cv::CascadeClassifier detectionClassifier;
    cv::Ptr<cv::face::FaceRecognizer> model;

    cv::Mat gray;

    std::vector<cv::Rect> faces;
    std::vector<int> detections;

  public:
    std::vector<cv::Rect> detect(cv::Mat);
    void recognise(cv::Mat, std::vector<cv::Rect>);

    void setDetectionClassifier(std::string);
};

#endif