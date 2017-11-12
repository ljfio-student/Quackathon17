#include "facial.hpp"

std::vector<cv::Rect> Facial::detect(cv::Mat frame)
{
    // Resize and color
    cv::resize(frame, frame, cv::Size(1280, 720));
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Perform classification
    detectionClassifier.detectMultiScale(gray, faces, detections, 1.2, 5, 2, cv::Size(30, 30));

    return faces;
}

void Facial::recognise(cv::Mat frame, std::vector<cv::Rect> faces)
{
    for (auto face = faces.begin(); face != faces.end(); ++face)
    {
        cv::Mat cropped = frame(*face);
        int label = model->predict(cropped);
    }
}

void Facial::setDetectionClassifier(std::string fileName)
{
    detectionClassifier.load(fileName);
}