#include "gray_image.h"
#include <cstdlib> // 必須引入此標頭檔以使用 system()

GrayImage::GrayImage(){
    pixels = nullptr;
}

GrayImage::~GrayImage(){
    for (int i = 0; i < h; ++i) {
        delete[] pixels[i];
    }
    delete[] pixels;
}

bool GrayImage::LoadImage(string filename){
    // Implementation for loading gray image
    pixels = data_loader.Load_Gray(filename, &w, &h);
    return pixels != nullptr;
}

void GrayImage::DumpImage(string filename){
    // Implementation for dumping gray image
    assert(pixels != nullptr && w > 0 && h > 0);
    data_loader.Dump_Gray(w, h, pixels, filename);
}

void GrayImage::Display_ASCII(){
    // Implementation for displaying gray image in ASCII
    assert(pixels != nullptr && w > 0 && h > 0);
    data_loader.Display_Gray_ASCII(w, h, pixels);
}

void GrayImage::Display_CMD(){
    // Implementation for displaying gray image in CMD
    assert(pixels != nullptr && w > 0 && h > 0);
    data_loader.Dump_Gray(w, h, pixels, "temp.jpg");
    data_loader.Display_Gray_CMD("temp.jpg");
    int ret = std::remove("temp.jpg"); // 刪除臨時文件
    (void)ret; // 避免未使用變數的警告
}