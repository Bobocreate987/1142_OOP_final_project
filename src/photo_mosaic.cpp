#include "photo_mosaic.h"
#include <algorithm>
#include <cmath>
using namespace std;

Data_Loader PhotoMosaic::data_loader;

PhotoMosaic::PhotoMosaic(int tile_size) : tile_size(tile_size) {}

PhotoMosaic::~PhotoMosaic() {}

int*** PhotoMosaic::Alloc3D(int w, int h) {
    int*** arr = new int**[h];
    for (int y = 0; y < h; y++) {
        arr[y] = new int*[w];
        for (int x = 0; x < w; x++)
            arr[y][x] = new int[3]();
    }
    return arr;
}

void PhotoMosaic::Free3D(int w, int h, int*** arr) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            delete[] arr[y][x];
        delete[] arr[y];
    }
    delete[] arr;
}

void PhotoMosaic::ComputeAvg(int*** px, int y0, int x0, int h, int w,
                              double& r, double& g, double& b) {
    double sr = 0, sg = 0, sb = 0;
    for (int y = y0; y < y0 + h; y++)
        for (int x = x0; x < x0 + w; x++) {
            sr += px[y][x][0];
            sg += px[y][x][1];
            sb += px[y][x][2];
        }
    double n = (double)(h * w);
    r = sr / n;
    g = sg / n;
    b = sb / n;
}

int*** PhotoMosaic::ResizeRGB(int*** src, int sw, int sh, int dw, int dh) {
    int*** dst = Alloc3D(dw, dh);
    for (int y = 0; y < dh; y++)
        for (int x = 0; x < dw; x++) {
            int sy = y * sh / dh;
            int sx = x * sw / dw;
            dst[y][x][0] = src[sy][sx][0];
            dst[y][x][1] = src[sy][sx][1];
            dst[y][x][2] = src[sy][sx][2];
        }
    return dst;
}

int PhotoMosaic::FindBestTile(const vector<TileData>& tiles,
                               double tr, double tg, double tb) {
    int best = 0;
    double best_dist = 1e18;
    for (int i = 0; i < (int)tiles.size(); i++) {
        double dr = tr - tiles[i].avg_r;
        double dg = tg - tiles[i].avg_g;
        double db = tb - tiles[i].avg_b;
        double dist = dr*dr + dg*dg + db*db;
        if (dist < best_dist) { best_dist = dist; best = i; }
    }
    return best;
}

RGBImage* PhotoMosaic::Generate(string target_path, string tiles_dir) {
    // Step 1: load all library images at original resolution, compute avg RGB
    vector<string> files;
    data_loader.List_Directory(tiles_dir, files);

    int out_tile_w = 0, out_tile_h = 0;
    vector<TileData> tiles;

    for (const auto& f : files) {
        int tw, th;
        int*** raw = data_loader.Load_RGB(f, &tw, &th);
        if (!raw) continue;

        // Use the first image's size as the common output tile size
        if (out_tile_w == 0) { out_tile_w = tw; out_tile_h = th; }

        TileData td;
        td.orig_w = out_tile_w;
        td.orig_h = out_tile_h;

        // Normalise to common size only if this image differs
        if (tw == out_tile_w && th == out_tile_h) {
            td.pixels = raw;
        } else {
            td.pixels = ResizeRGB(raw, tw, th, out_tile_w, out_tile_h);
            Free3D(tw, th, raw);
        }

        ComputeAvg(td.pixels, 0, 0, out_tile_h, out_tile_w,
                   td.avg_r, td.avg_g, td.avg_b);
        tiles.push_back(td);
    }

    if (tiles.empty()) return nullptr;

    // Step 2: load target image
    int iw, ih;
    int*** target = data_loader.Load_RGB(target_path, &iw, &ih);
    if (!target) return nullptr;

    // Grid is determined by tile_size on the target
    int num_x = (iw + tile_size - 1) / tile_size;
    int num_y = (ih + tile_size - 1) / tile_size;

    // Output canvas: each grid cell becomes one full library image
    int out_w = num_x * out_tile_w;
    int out_h = num_y * out_tile_h;
    int*** out = Alloc3D(out_w, out_h);

    // Step 3: for each grid cell, find best library image and paste it
    for (int gy = 0; gy < num_y; gy++) {
        for (int gx = 0; gx < num_x; gx++) {
            int region_h = min(tile_size, ih - gy * tile_size);
            int region_w = min(tile_size, iw - gx * tile_size);

            double tr, tg, tb;
            ComputeAvg(target, gy * tile_size, gx * tile_size,
                       region_h, region_w, tr, tg, tb);

            int best = FindBestTile(tiles, tr, tg, tb);

            for (int y = 0; y < out_tile_h; y++)
                for (int x = 0; x < out_tile_w; x++) {
                    int oy = gy * out_tile_h + y;
                    int ox = gx * out_tile_w + x;
                    out[oy][ox][0] = tiles[best].pixels[y][x][0];
                    out[oy][ox][1] = tiles[best].pixels[y][x][1];
                    out[oy][ox][2] = tiles[best].pixels[y][x][2];
                }
        }
    }

    Free3D(iw, ih, target);
    for (auto& td : tiles) Free3D(out_tile_w, out_tile_h, td.pixels);

    return new RGBImage(out_w, out_h, out);
}
