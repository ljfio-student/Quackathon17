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

  public:
    Facial(std::string fileName) {
        detectionClassifier.load(fileName);
        model = cv::face::EigenFaceRecognizer::create();
    }

    std::vector<cv::Rect> detect(cv::Mat);
    std::vector<int> recognise(cv::Mat, std::vector<cv::Rect>);
    void train(cv::Mat, int);
    void analyse(cv::VideoCapture);

    bool portrait(cv::Mat, cv::Mat);
};

#endif