#include "facial.hpp"

std::vector<cv::Rect> Facial::detect(cv::Mat frame)
{
    cv::Mat gray;

    std::vector<cv::Rect> faces;
    std::vector<int> detections;

    // Color
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    // Perform classification
    detectionClassifier.detectMultiScale(gray, faces, detections, 1.2, 5, 2, cv::Size(30, 30));

    return faces;
}

void Facial::load(std::vector<cv::Mat> images, std::vector<int> labels)
{
    model->train(images, labels);
}

void Facial::train(std::vector<cv::Mat> images, std::vector<int> labels)
{
    model->update(images, labels);
}

void Facial::train(cv::Mat image, int label)
{
    model->update({image}, {label});
}

std::vector<int> Facial::recognise(cv::Mat frame, std::vector<cv::Rect> faces)
{
    std::vector<int> labels;

    for (auto face = faces.begin(); face != faces.end(); ++face)
    {
        cv::Mat cropped = frame(*face);
        cv::cvtColor(cropped, cropped, cv::COLOR_BGR2GRAY);
        cv::resize(cropped, cropped, cv::Size(1024, 1024));

        int label = model->predict(cropped);

        labels.push_back(label);
    }

    return labels;
}

std::multimap<int, double> Facial::analyse(cv::VideoCapture capture)
{
    cv::Mat frame;
    std::multimap<int, double> detections;

    for (;;)
    {
        capture.read(frame);

        if (frame.empty())
        {
            break;
        }

        std::vector<cv::Rect> faces = detect(frame);

        if (faces.size() > 0)
        {
            std::vector<int> labels = recognise(frame, faces);

            if (labels.size() > 0) {
                for (auto& label : labels) {
                    double current_frame = capture.get(cv::CAP_PROP_POS_FRAMES);

                    detections.insert(std::pair<int, double>(label, current_frame));
                }
            }
        }
    }

    return detections;
}

bool Facial::portrait(cv::Mat frame, cv::Mat *result)
{
    std::vector<cv::Rect> faces = detect(frame);

    if (printDebug)
    {
        std::cout << "Portrait detected " << faces.size() << " faces" << std::endl;
    }

    if (faces.size() == 1)
    {
        cv::Mat crop = frame(faces[0]);
        cv::resize(crop, *result, cv::Size(1024, 1024));

        return true;
    }
    else
    {
        return false;
    }
}