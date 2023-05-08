#include <iostream>
#include <cstdlib>
#include "parser.h"
#include "image.h"

const std::string INVALID_ARGUMENTS_ERROR = "invalid arguments";

void Parser::parsed(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "ok";  //справка
        return;
    }
    if (argc == 2) {
        std::cout << INVALID_ARGUMENTS_ERROR;
        return;
    }
    Image image;
    if (!image.Read(argv[1])) {
        return;
    }
    size_t i = 3;
    while (i < argc) {
        std::string filter = argv[i];
        if (filter == "-crop") {
            if (i + 2 >= argc) {
                std::cout << INVALID_ARGUMENTS_ERROR;
            }
            auto width = std::atoi(argv[i + 1]);
            auto height = std::atoi(argv[i + 2]);
            if (width <= 0 || height <= 0) {
                std::cout << INVALID_ARGUMENTS_ERROR;
                return;
            }
            image.Apply(Crop(width, height));
            i += 3;
            continue;
        }
        if (filter == "-neg") {
            image.Apply(NegativeFilter());
            ++i;
            continue;
        }
        if (filter == "-gs") {
            image.Apply(GrayScale());
            ++i;
            continue;
        }
        if (filter == "-sharp") {
            image.Apply(Sharpening());
            ++i;
            continue;
        }
        if (filter == "-edge") {
            if (i + 1 >= argc) {
                std::cout << INVALID_ARGUMENTS_ERROR;
            }
            double threshold = std::stod(argv[i + 1]);
            if (argv[i + 1][0] != '0' && threshold == 0) {
                std::cout << INVALID_ARGUMENTS_ERROR;
                return;
            }
            image.Apply(EdgeDetection(threshold));
            i += 2;
            continue;
        }
        if (filter == "-blur") {
            if (i + 1 >= argc) {
                std::cout << INVALID_ARGUMENTS_ERROR;
            }
            double sigma = std::stod(argv[i + 1]);
            if (argv[i + 1][0] != '0' && sigma == 0) {
                std::cout << INVALID_ARGUMENTS_ERROR;
                return;
            }
            image.Apply(GaussianBlur(sigma));
            i += 2;
            continue;
        }
//        if (filter == "-color") {
//            if (i + 1 >= argc) {
//                std::cout << INVALID_ARGUMENTS_ERROR;
//            }
//            std::string color = reinterpret_cast<const char *>(std::atoi(argv[i + 1]));
//            if (color == "green"  color == "red"  color == "blue") {
//                image.Apply(PurpleRedYellowFilter(color));
//            } else {
//                std::cout << "no such color";
//                return;
//            }
//            i += 2;
//            continue;
//        }
//        if (filter == "-gsbinary") {
//            if (i + 1 >= argc) {
//                std::cout << INVALID_ARGUMENTS_ERROR;
//            }
//            int adjustment = std::atoi(argv[i + 1]);
//            image.Apply(GrayScaleToBinaryFilter(adjustment));
//            i += 2;
//            continue;
//        }
//        if (filter == "-noise") {
//            image.Apply(AddSaltAndPepperNoiseFilter());
//            ++i;
//            continue;
//        }
    }

    image.Write(argv[2]);
}
