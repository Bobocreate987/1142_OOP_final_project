#include "rgb_image.h"
#include <cstdlib> // 必須引入此標頭檔以使用 system()

RGBImage::RGBImage(){
    pixels = nullptr;
}

RGBImage::RGBImage(const RGBImage& other) {
    w = other.w;
    h = other.h;
    if (other.pixels == nullptr) {
        pixels = nullptr;
        return;
    }
    pixels = new int**[h];
    for (int i = 0; i < h; ++i) {
        pixels[i] = new int*[w];
        for (int j = 0; j < w; ++j) {
            pixels[i][j] = new int[3];
            for (int k = 0; k < 3; ++k)
                pixels[i][j][k] = other.pixels[i][j][k];
        }
    }
}

RGBImage& RGBImage::operator=(const RGBImage& other) {
    if (this == &other) return *this;
    if (pixels != nullptr) {
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) delete[] pixels[i][j];
            delete[] pixels[i];
        }
        delete[] pixels;
    }
    w = other.w;
    h = other.h;
    if (other.pixels == nullptr) {
        pixels = nullptr;
        return *this;
    }
    pixels = new int**[h];
    for (int i = 0; i < h; ++i) {
        pixels[i] = new int*[w];
        for (int j = 0; j < w; ++j) {
            pixels[i][j] = new int[3];
            for (int k = 0; k < 3; ++k)
                pixels[i][j][k] = other.pixels[i][j][k];
        }
    }
    return *this;
}

RGBImage::~RGBImage(){
    if (pixels == nullptr) return;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) delete[] pixels[i][j];
        delete[] pixels[i];
    }
    delete[] pixels;
}

bool RGBImage::LoadImage(string filename){
    // Implementation for loading RGB image
    pixels = data_loader.Load_RGB(filename, &w, &h);
    return pixels != nullptr;
}

void RGBImage::DumpImage(string filename){
    // Implementation for dumping RGB image
    assert(pixels != nullptr && w > 0 && h > 0);
    data_loader.Dump_RGB(w, h, pixels, filename);
}

void RGBImage::Display_ASCII(){
    // Implementation for displaying RGB image in ASCII
    assert(pixels != nullptr && w > 0 && h > 0);
    data_loader.Display_RGB_ASCII(w, h, pixels);
}

void RGBImage::Display_CMD(){
    // Implementation for displaying RGB image in CMD
    assert(pixels != nullptr && w > 0 && h > 0);
    data_loader.Dump_RGB(w, h, pixels, "temp.jpg");
    data_loader.Display_RGB_CMD("temp.jpg");
    int ret = std::remove("temp.jpg"); // 刪除臨時文件
    (void)ret; // 避免未使用變數的警告
}