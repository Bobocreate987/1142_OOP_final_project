#ifndef _RGB_IMAGE_H_
#define _RGB_IMAGE_H_

#include "image.h"

class RGBImage : public Image{
private:
    int ***pixels;
public:
    RGBImage();
    RGBImage(int w, int h, int ***pixels) : pixels(pixels) {
        this->w = w;
        this->h = h;
    }
    RGBImage(const RGBImage& other);
    RGBImage& operator=(const RGBImage& other);
    ~RGBImage();
    int*** getPixels() const { return pixels; }
    bool LoadImage(string filename) override;
    void DumpImage(string filename) override;
    void Display_ASCII() override;
    void Display_CMD() override;

};

#endif