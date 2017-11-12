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

    args::ValueFlag<std::string> classifier(parser, "classifier", "The HAAR facial classifier file", {'c', "classifier"}, args::Options::Required);

    args::Group group(parser, "Work with video file or profile image:", args::Group::Validators::Xor);
    args::ValueFlag<std::string> file(group, "file", "File you wish to process", {'f', "file"});
    args::ValueFlag<std::string> profile(group, "profile", "Produce a profile image for us to use later", {'p', "profile"});

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

    Facial facial(argv[1]);

    if (profile.Matched()) {

    } else {
        // Load the data in from stdin
        std::cin >> std::noskipws; // don't skip the whitespace while reading

        // use stream iterators to copy the stream to a string
        std::istream_iterator<char> it(std::cin);
        std::istream_iterator<char> end;
        std::string results(it, end);

        auto json = nlohmann::json::parse(results);

        // open selected camera using selected API
        cv::VideoCapture video;

        // check if we succeeded
        if (!video.isOpened())
        {
            std::cerr << "ERROR! Unable to open camera\n";
            return -1;
        }

        facial.analyse(video);
    }

    return 0;
}
