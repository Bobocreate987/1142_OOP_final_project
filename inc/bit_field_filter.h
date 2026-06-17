#ifndef _BIT_FIELD_FILTER_H_
#define _BIT_FIELD_FILTER_H_

#include "rgb_image.h"
#include <cstdint>

#define FILTER_BOX      0b00000001
#define FILTER_SOBEL    0b00000010
#define FILTER_CONTRAST 0b00000100
#define FILTER_MOSAIC   0b00001000

RGBImage* applyFilters(const RGBImage& src, uint8_t option);

class Filter {
public:
    virtual ~Filter() = default;
    virtual RGBImage* apply(const RGBImage& src) = 0;
};

class BoxFilter : public Filter {
    int radius;
public:
    BoxFilter(int radius = 3);
    RGBImage* apply(const RGBImage& src) override;
};

class SobelFilter : public Filter {
public:
    RGBImage* apply(const RGBImage& src) override;
};

class ContrastStretching : public Filter {
public:
    RGBImage* apply(const RGBImage& src) override;
};

class MosaicFilter : public Filter {
    int tile_size;
public:
    MosaicFilter(int tile_size = 10);
    RGBImage* apply(const RGBImage& src) override;
};

#endif
