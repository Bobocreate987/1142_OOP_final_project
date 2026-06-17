#include"image.h"

Image::Image() : w(0), h(0) {}

Image::~Image() {}

Data_Loader Image::data_loader;

int Image::get_w() const {
    return w;
}

int Image::get_h() const {
    return h;
}