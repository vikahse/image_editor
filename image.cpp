#include "image.h"
#include "parser.h"
long double BigMatrix::created_ = -1;
std::vector<std::vector<long double>> BigMatrix::data_ =
        std::vector<std::vector<long double>>(5, std::vector<long double>(5));

BigMatrix::BigMatrix(long double sigma) {
    if (created_ != sigma) {
        created_ = sigma;
        long double tss = 2 * std::pow(sigma, 2);
        long double rp_tss = std::sqrt(tss * M_PI);
        long double summ = 0;
        for (int i = -2; i <= 2; ++i) {
            for (int j = -2; j <= 2; ++j) {
                long double distance = std::pow(i, 2) + std::pow(j, 2);
                data_[2 + i][2 + j] = 1 / rp_tss * std::exp(-1 * distance / tss);
                summ += data_[2 + i][2 + j];
            }
        }
        for (int i = -2; i <= 2; ++i) {
            for (int j = -2; j <= 2; ++j) {
                data_[2 + i][2 + j] /= summ;
            }
        }
    }
}

void BigMatrix::Apply(int x, int y, ImageData &data) const {
    Color color = {0, 0, 0};
    long double blue = 0;
    long double green = 0;
    long double red = 0;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            int current_x = std::min(std::max(0, x + i), static_cast<int>(data[0].size() - 1));
            int current_y = std::min(std::max(0, y + j), static_cast<int>(data.size() - 1));
            blue += static_cast<long double>(data[current_y][current_x].blue) * data_[2 + j][2 + i];
            green += static_cast<long double>(data[current_y][current_x].green) * data_[2 + j][2 + i];
            red += static_cast<long double>(data[current_y][current_x].red) * data_[2 + j][2 + i];
        }
    }
    color.blue = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(blue))));
    color.green = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(green))));
    color.red = static_cast<unsigned char>(std::min(255, std::max(0, static_cast<int>(red))));
    data[y][x] = color;
}

GaussianBlur::GaussianBlur(long double sigma) : sigma_(sigma) {
}

void GaussianBlur::Apply(ImageData &data, std::string &info) const {
    BigMatrix matrix(sigma_);
    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < data[0].size(); ++j) {
            matrix.Apply(i, j, data);
        }
    }
}

void NegativeFilter::Apply(ImageData &data, std::string &info) const {
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[0].size(); ++j) {
            data[i][j].blue = 1 - data[i][j].blue;
            data[i][j].green = 1 - data[i][j].green;
            data[i][j].red = 1 - data[i][j].red;
        }
    }
}

void GrayScale::Apply(ImageData &data, std::string &info) const {
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[0].size(); ++j) {
            data[i][j].blue = 0.299 * data[i][j].red + 0.587 * data[i][j].green + 0.114 * data[i][j].blue;
            data[i][j].green = 0.299 * data[i][j].red + 0.587 * data[i][j].green + 0.114 * data[i][j].blue;
            data[i][j].red = 0.299 * data[i][j].red + 0.587 * data[i][j].green + 0.114 * data[i][j].blue;
        }
    }
}

void Matrix::Apply(int x, int y, ImageData &data) const {
    Color color = {0, 0, 0};
    int blue = 0;
    int green = 0;
    int red = 0;
    for (int i = -1; i < 2; ++i) {
        for (int j = -1; j < 2; ++j) {
            int current_x = std::min(std::max(0, x + i), static_cast<int>(data[0].size() - 1));
            int current_y = std::min(std::max(0, y + j), static_cast<int>(data.size() - 1));
            blue += static_cast<int>(data[current_y][current_x].blue) * data_[1 + j][1 + i];
            green += static_cast<int>(data[current_y][current_x].green) * data_[1 + j][1 + i];
            red += static_cast<int>(data[current_y][current_x].red) * data_[1 + j][1 + i];
        }
    }
    color.blue = static_cast<unsigned char>(std::min(255, std::max(0, blue)));
    color.green = static_cast<unsigned char>(std::min(255, std::max(0, green)));
    color.red = static_cast<unsigned char>(std::min(255, std::max(0, red)));
    data[y][x] = color;
}
Matrix::Matrix(const std::array<int, 9> &data) {
    int index = 0;
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            data_[i][j] = data[index];
            ++index;
        }
    }
}

void Sharpening::Apply(ImageData &data, std::string &info) const {
    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < data[0].size(); ++j) {
            matrix_.Apply(j, i, data);
        }
    }
}

Sharpening::Sharpening() : matrix_({0, -1, 0, -1, 5, -1, 0, -1, 0}) {
}

void EdgeDetection::Apply(ImageData &data, std::string &info) const {
    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < data[0].size(); ++j) {
            matrix_.Apply(j, i, data);
            if (static_cast<int>(data[i][j].blue) + static_cast<int>(data[i][j].green) +
                static_cast<int>(data[i][j].red) <
                threshold_ * 255) {
                data[i][j] = {0, 0, 0};
            } else {
                data[i][j] = {255, 255, 255};
            }
        }
    }
}

EdgeDetection::EdgeDetection(int threshold) : matrix_({0, -1, 0, -1, 4, -1, 0, -1, 0}), threshold_(threshold) {
}

Crop::Crop(int32_t width, int32_t height) : width_(width), height_(height) {
}

void Crop::Apply(std::vector<std::vector<Color>> &data, std::string &info) const {
    if (width_ < data[0].size()) {
        for (auto &col : data) {
            while (col.size() > width_) {
                col.pop_back();
            }
        }
        *(reinterpret_cast<int *>(info.data() + Image().WIDTH)) = width_;
    }
    if (height_ < data.size()) {
        data.resize(height_);
        *(reinterpret_cast<int *>(info.data() + Image().HEIGHT)) = height_;
    }
    *(reinterpret_cast<int64_t *>(info.data() + Image().FILE_OFFSET)) = data.size() * data[0].size() * 3 + Image().INFO;
}

void PurpleRedYellowFilter::Apply(ImageData &data, std::string &info) const {
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[0].size(); ++j) {
            if (color_ == "blue") {
                if (data[i][j].blue + 50 >= 255) {
                    data[i][j].blue = 255;
                } else {
                    data[i][j].blue += 50;
                }
            } else if (color_ == "green") {
                if (data[i][j].green + 50 >= 255) {
                    data[i][j].green = 255;
                } else {
                    data[i][j].green += 50;
                }
            } else if (color_ == "red") {
                if (data[i][j].red + 50 >= 255) {
                    data[i][j].red = 255;
                } else {
                    data[i][j].red += 50;
                }
            }
        }
    }
}

PurpleRedYellowFilter::PurpleRedYellowFilter(std::string color) : color_(color) {
}

void GrayScaleToBinaryFilter::Apply(ImageData &data, std::string &info) const {
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[0].size(); ++j) {
            if (data[i][j].blue + data[i][j].green + data[i][j].red > adjustment_) {
                data[i][j].blue = 255;
                data[i][j].green = 255;
                data[i][j].red = 255;
            } else {
                data[i][j].blue = 0;
                data[i][j].green = 0;
                data[i][j].red = 0;
            }
        }
    }
}
GrayScaleToBinaryFilter::GrayScaleToBinaryFilter(int adjustment) : adjustment_(adjustment) {
}

void AddSaltAndPepperNoiseFilter::Apply(ImageData &data, std::string &info) const {
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = 0; j < data[0].size(); ++j) {
            int randnum01 = rand() % 100;
            if (randnum01 < 5) {
                int randnum02 = rand() % 2;
                if (randnum02 == 0) {
                    data[i][j].blue = 0;
                    data[i][j].green = 0;
                    data[i][j].red = 0;
                } else {
                    data[i][j].blue = 255;
                    data[i][j].green = 255;
                    data[i][j].red = 255;
                }
            }
        }
    }
}


bool Image::Read(const char *path) {
    std::ifstream input_stream(path, std::ios::binary);

    std::reverse(data_.begin(), data_.end());

    if (!input_stream) {
        std::cout << "file did not open";
        return false;
    }
    info_.resize(INFO);

    input_stream.read(info_.data(), INFO);

    if (input_stream.gcount() != INFO) {
        input_stream.close();
        return false;
    }

    uint32_t width = *(reinterpret_cast<uint32_t *>(info_.data() + WIDTH));
    uint32_t height = *(reinterpret_cast<uint32_t *>(info_.data() + HEIGHT));

    int padding = (4 - (width * 3) % 4) % 4;

    data_.resize(height);

    for (size_t i = 0; i < height; ++i) {
        std::vector<Color> ans(width);
        for (size_t j = 0; j < width; ++j) {
            Color obj{};
            uint8_t a1;
            uint8_t b1;
            uint8_t c1;
            input_stream.read(reinterpret_cast<char *>(&a1), sizeof(a1));
            if (input_stream.gcount() != 1) {
                input_stream.close();
                return false;
            }
            input_stream.read(reinterpret_cast<char *>(&b1), sizeof(b1));
            if (input_stream.gcount() != 1) {
                input_stream.close();
                return false;
            }
            input_stream.read(reinterpret_cast<char *>(&c1), sizeof(c1));
            if (input_stream.gcount() != 1) {
                input_stream.close();
                return false;
            }
            obj.blue = a1;
            obj.green = b1;
            obj.red = c1;
            ans[j] = obj;
        }
        data_[i] = ans;

        input_stream.ignore(padding);
    }

    std::reverse(data_.begin(), data_.end());

    input_stream.close();

    return true;
}

bool Image::Write(const char *path) {

    std::ofstream output_stream;

    output_stream.open(path, std::ios::binary);

    std::reverse(data_.begin(), data_.end());

    uint32_t width = *(reinterpret_cast<uint32_t *>(info_.data() + WIDTH));
    uint32_t height = *(reinterpret_cast<uint32_t *>(info_.data() + HEIGHT));

    if (!output_stream) {
        std::cout << "file did not open while writing";
        return false;
    }

    int padding = ((4 - (width * 3) % 4)) % 4;

    for (size_t i = 0; i < INFO; ++i) {
        output_stream.write(info_.data() + i, sizeof(char));
    }

    char bmpPad[3] = {0, 0, 0};

    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < width; ++j) {
            char blue = static_cast<char>(data_[i][j].blue);
            char green = static_cast<char>(data_[i][j].green);
            char red = static_cast<char>(data_[i][j].red);

            char color[] = {blue, green, red};

            output_stream.write(reinterpret_cast<char *>(color), sizeof(Color));
        }
        output_stream.write(reinterpret_cast<char *>(bmpPad), padding);
    }
    std::reverse(data_.begin(), data_.end());

    output_stream.close();

    return true;
}

void Image::Apply(const Filter &filter) {
    filter.Apply(data_, info_);
}

int main(int argc, char *argv[]) {
    Parser::parsed(argc, argv);
////    Parser::parsed(argc, argv);
        Image image1;
        image1.Read("../example.bmp");
////
//        Image image2;
//        image2.Read("../projects/image_processor/examples/igru.bmp");
//
//
        image1.Apply(GrayScale());
//    //    image2.Apply(NegativeFilter());
//    //    image.Apply(Sharpening());
//    //    image.Apply(EdgeDetection(3));
        image1.Apply(Crop(2014, 2014));
        image1.Apply(GaussianBlur(0.5));
//    //    image2.Apply(Crop(1090, 2346));
//    //    image.Apply(GaussianBlur(1000));
//
//    //доп фильтр №1: параметр один из цветов (red/blue/green)
//    //        image1.Apply(PurpleRedYellowFilter("blue"));
//    //        image1.Apply(PurpleRedYellowFilter("red"));
//    //        image1.Apply(PurpleRedYellowFilter("green"));
//
//    //доп фильтр №2: в зависимости от параметра раскрашивает в белое и черное
////            image1.Apply(GrayScaleToBinaryFilter(430));
//
//    //доп фильтр №3: добавляет на изображение шум в виде точек
//    //        image1.Apply(AddSaltAndPepperNoiseFilter());
//
        image1.Write("../copy1.bmp");
//
//    //    image2.Write("../projects/image_processor/examples/copy2.bmp");

    return 0;
}
