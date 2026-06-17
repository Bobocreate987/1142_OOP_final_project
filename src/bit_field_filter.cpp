#include "bit_field_filter.h"
#include <cmath>
#include <algorithm>

RGBImage* applyFilters(const RGBImage& src, uint8_t option) {
    RGBImage* cur = new RGBImage(src);
    if (option & FILTER_BOX) {
        BoxFilter f;
        RGBImage* next = f.apply(*cur);
        delete cur; cur = next;
    }
    if (option & FILTER_SOBEL) {
        SobelFilter f;
        RGBImage* next = f.apply(*cur);
        delete cur; cur = next;
    }
    if (option & FILTER_CONTRAST) {
        ContrastStretching f;
        RGBImage* next = f.apply(*cur);
        delete cur; cur = next;
    }
    if (option & FILTER_MOSAIC) {
        MosaicFilter f;
        RGBImage* next = f.apply(*cur);
        delete cur; cur = next;
    }
    return cur;
}

static int*** alloc_pixels(int w, int h) {
    int*** p = new int**[h];
    for (int i = 0; i < h; i++) {
        p[i] = new int*[w];
        for (int j = 0; j < w; j++)
            p[i][j] = new int[3];
    }
    return p;
}

// BoxFilter

BoxFilter::BoxFilter(int radius) : radius(radius) {}

RGBImage* BoxFilter::apply(const RGBImage& src) {
    int w = src.get_w(), h = src.get_h();
    int*** in = src.getPixels();
    int*** out = alloc_pixels(w, h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            for (int c = 0; c < 3; c++) {
                int sum = 0, count = 0;
                for (int dy = -radius; dy <= radius; dy++) {
                    for (int dx = -radius; dx <= radius; dx++) {
                        int ny = y + dy, nx = x + dx;
                        if (ny >= 0 && ny < h && nx >= 0 && nx < w) {
                            sum += in[ny][nx][c];
                            count++;
                        }
                    }
                }
                out[y][x][c] = sum / count;
            }
        }
    }
    return new RGBImage(w, h, out);
}

// SobelFilter

RGBImage* SobelFilter::apply(const RGBImage& src) {
    int w = src.get_w(), h = src.get_h();
    int*** in = src.getPixels();

    // Convert to grayscale using luminance weights from data_loader.h
    int** gray = new int*[h];
    for (int i = 0; i < h; i++) {
        gray[i] = new int[w];
        for (int j = 0; j < w; j++)
            gray[i][j] = (int)(R_FACTOR * in[i][j][0] + G_FACTOR * in[i][j][1] + B_FACTOR * in[i][j][2]);
    }

    const int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    int*** out = alloc_pixels(w, h);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int gx = 0, gy = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int ny = y + dy, nx = x + dx;
                    int val = (ny >= 0 && ny < h && nx >= 0 && nx < w) ? gray[ny][nx] : 0;
                    gx += Gx[dy + 1][dx + 1] * val;
                    gy += Gy[dy + 1][dx + 1] * val;
                }
            }
            int mag = (int)sqrt((double)(gx * gx + gy * gy));
            if (mag > 255) mag = 255;
            out[y][x][0] = out[y][x][1] = out[y][x][2] = mag;
        }
    }

    for (int i = 0; i < h; i++) delete[] gray[i];
    delete[] gray;

    return new RGBImage(w, h, out);
}

// ContrastStretching

RGBImage* ContrastStretching::apply(const RGBImage& src) {
    int w = src.get_w(), h = src.get_h();
    int*** in = src.getPixels();
    int*** out = alloc_pixels(w, h);

    for (int c = 0; c < 3; c++) {
        int min_val = 255, max_val = 0;
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++) {
                min_val = std::min(min_val, in[y][x][c]);
                max_val = std::max(max_val, in[y][x][c]);
            }
        for (int y = 0; y < h; y++)
            for (int x = 0; x < w; x++) {
                if (max_val == min_val)
                    out[y][x][c] = in[y][x][c];
                else
                    out[y][x][c] = (in[y][x][c] - min_val) * 255 / (max_val - min_val);
            }
    }
    return new RGBImage(w, h, out);
}

// MosaicFilter

MosaicFilter::MosaicFilter(int tile_size) : tile_size(tile_size) {}

RGBImage* MosaicFilter::apply(const RGBImage& src) {
    int w = src.get_w(), h = src.get_h();
    int*** in = src.getPixels();
    int*** out = alloc_pixels(w, h);

    for (int ty = 0; ty < h; ty += tile_size) {
        for (int tx = 0; tx < w; tx += tile_size) {
            int sum[3] = {0, 0, 0}, count = 0;
            for (int dy = 0; dy < tile_size && ty + dy < h; dy++)
                for (int dx = 0; dx < tile_size && tx + dx < w; dx++) {
                    for (int c = 0; c < 3; c++)
                        sum[c] += in[ty + dy][tx + dx][c];
                    count++;
                }
            int avg[3] = {sum[0] / count, sum[1] / count, sum[2] / count};
            for (int dy = 0; dy < tile_size && ty + dy < h; dy++)
                for (int dx = 0; dx < tile_size && tx + dx < w; dx++)
                    for (int c = 0; c < 3; c++)
                        out[ty + dy][tx + dx][c] = avg[c];
        }
    }
    return new RGBImage(w, h, out);
}
