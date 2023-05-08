#pragma once
#include <bit>
#include <iostream>
#include <fstream>
#include <array>
#include <bit>
#include <cmath>
#include <vector>

struct Color {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

using ImageData = std::vector<std::vector<Color>>;

class Filter {
public:
    virtual void Apply(ImageData &data, std::string &info) const = 0;
};

class Matrix {
public:
    Matrix(const std::array<int, 9> &data);

    void Apply(int x, int y, ImageData &data) const;

private:
    std::array<std::array<int, 3>, 3> data_{};
};

class BigMatrix {
public:
    explicit BigMatrix(long double sigma);

    void Apply(int x, int y, ImageData &data) const;

private:
    static long double created_;
    static std::vector<std::vector<long double>> data_;
};

class NegativeFilter : public Filter {
public:
    void Apply(ImageData &data, std::string &info) const override;
};

class GrayScale : public Filter {
public:
    void Apply(ImageData &data, std::string &info) const override;
};

class Sharpening : public Filter {
private:
    Matrix matrix_;

public:
    Sharpening();

    void Apply(ImageData &data, std::string &info) const override;
};

class EdgeDetection : public Filter {
public:
    explicit EdgeDetection(int threshold);

    void Apply(ImageData &data, std::string &info) const override;

private:
    Matrix matrix_;
    int threshold_;
};

class Crop : public Filter {
public:
    Crop(int32_t width, int32_t height);

    void Apply(ImageData &data, std::string &info) const override;

private:
    int32_t width_;
    int32_t height_;
};

class GaussianBlur : public Filter {
public:
    explicit GaussianBlur(long double sigma);

    void Apply(ImageData &data, std::string &info) const override;

private:
    long double sigma_;
};

class PurpleRedYellowFilter : public Filter {
public:
    PurpleRedYellowFilter(std::string color);
    void Apply(ImageData &data, std::string &info) const override;

private:
    std::string color_;
};

class GrayScaleToBinaryFilter : public Filter {
public:
    GrayScaleToBinaryFilter(int adjustment);
    void Apply(ImageData &data, std::string &info) const override;

private:
    int adjustment_;
};

class AddSaltAndPepperNoiseFilter : public Filter {
public:
    void Apply(ImageData &data, std::string &info) const override;
};
class Image {
public:
    bool Read(const char *path);

    bool Write(const char *path);

    void Apply(const Filter &filter);

    const size_t INFO = 54;
    const size_t WIDTH = 18;
    const size_t HEIGHT = 22;
    const size_t FILE_OFFSET = 2;

private:
    ImageData data_;
    std::string info_;
};
