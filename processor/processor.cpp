#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "json.hpp"
#include "args.hxx"
#include "facial.hpp"

int main(int argc, char **argv)
{
    args::ArgumentParser parser("Processor", "Words.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});

    args::ValueFlag<std::string> classifierFlag(parser, "classifier", "The HAAR facial classifier file", {'c', "classifier"}, args::Options::Required);
    args::ValueFlag<std::string> fileFlag(parser, "file", "File you wish to process", {'f', "file"}, args::Options::Required);

    args::Group orGroup(parser, "Video file specific:", args::Group::Validators::Xor);
    args::ValueFlag<std::string> modelFlag(orGroup, "model", "Model", {'m', "model"});
    args::ValueFlag<std::string> profileFlag(orGroup, "portrait", "Produce a portrait image for us to use later", {'p', "portrait"});

    args::Flag debugFlag(parser, "debug", "Print debug information", {'d', "debug"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    Facial facial(args::get(classifierFlag), args::get(debugFlag));

    if (fileFlag && profileFlag)
    {
        cv::Mat input = cv::imread(args::get(fileFlag));
        cv::Mat output;

        if (facial.portrait(input, &output))
        {
            cv::imwrite(args::get(profileFlag), output);
        }
        else
        {
            std::cerr << "Couldn't process profile, too many or none faces found" << std::endl;
        }
    }
    else if (fileFlag && modelFlag)
    {
        // Load the data in from stdin
        std::cin >> std::noskipws; // don't skip the whitespace while reading

        // use stream iterators to copy the stream to a string
        std::istream_iterator<char> it(std::cin);
        std::istream_iterator<char> end;
        std::string results(it, end);

        auto json = nlohmann::json::parse(results);

        // open selected camera using selected API
        cv::VideoCapture video;
        video.open(args::get(fileFlag));

        // Model data
        std::vector<cv::Mat> files;
        std::vector<int> labels;

        // Iterate through JSON and get the
        for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
            int label = atoi(it.key().c_str());
            auto value = it.value();

            // Check that it is an array
            if (value.is_array()) {
                // Place into the array
                for (auto& file : value) {
                    if (file.is_string()) {
                        // Load the file
                        std::string fileName = file;
                        cv::Mat image = cv::imread(fileName);

                        cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

                        // Input into vector
                        files.push_back(image);
                        labels.push_back(label);
                    }
                }
            }
        }

        // Input into Model
        facial.load(files, labels);

        // check if we succeeded
        if (!video.isOpened())
        {
            std::cerr << "ERROR! Unable to open video file\n";
            return -1;
        }

        auto detections = facial.analyse(video);

        nlohmann::json result;

        for (auto iter = detections.begin(); iter != detections.end(); ++iter) {
            std::string key = std::to_string(iter->first);

            if (result[key].is_array()) {
                result[key] += iter->second;
            } else {
                result[key] = nlohmann::json::array({iter->second});
            }
        }

        std::cout << result.dump() << std::endl;
    }

    return 0;
}
