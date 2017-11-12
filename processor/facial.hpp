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

    bool printDebug;

  public:
    Facial(std::string fileName, bool debug = false) {
        detectionClassifier.load(fileName);
        model = cv::face::EigenFaceRecognizer::create();
        printDebug = debug;
    }

    std::vector<cv::Rect> detect(cv::Mat);
    std::vector<int> recognise(cv::Mat, std::vector<cv::Rect>);
    void load(std::vector<cv::Mat>, std::vector<int>);

    void train(std::vector<cv::Mat>, std::vector<int>);
    void train(cv::Mat, int);

    std::multimap<int, double> analyse(cv::VideoCapture);

    bool portrait(cv::Mat, cv::Mat*);
};

#endif