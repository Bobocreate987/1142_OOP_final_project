#ifndef _PHOTO_MOSAIC_H_
#define _PHOTO_MOSAIC_H_

#include <vector>
#include <string>
#include "rgb_image.h"

class PhotoMosaic {
public:
    PhotoMosaic(int tile_size = 16);
    ~PhotoMosaic();
    RGBImage* Generate(string target_path, string tiles_dir);

private:
    int tile_size;
    static Data_Loader data_loader;

    struct TileData {
        int*** pixels;   // original library image size
        int orig_w, orig_h;
        double avg_r, avg_g, avg_b;
    };

    void ComputeAvg(int*** px, int y0, int x0, int h, int w,
                    double& r, double& g, double& b);
    int*** ResizeRGB(int*** src, int sw, int sh, int dw, int dh);
    int    FindBestTile(const vector<TileData>& tiles,
                        double tr, double tg, double tb);
    int*** Alloc3D(int w, int h);
    void   Free3D(int w, int h, int*** arr);
};

#endif
