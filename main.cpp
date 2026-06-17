#include "image.h"
#include "gray_image.h"
#include "rgb_image.h"
#include "photo_mosaic.h"
#include "bit_field_filter.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    /*
    //Test for gray image
    Image *img1 = new GrayImage();
    img1->LoadImage("Image-Folder/mnist/img_100.jpg");
    img1->DumpImage("img1.jpg");
    img1->Display_ASCII();
    img1->Display_CMD();

    //Test for rgb image
    Image *img2 = new RGBImage();
    img2->LoadImage("Image-Folder/lena.jpg");
    img2->DumpImage("img2.jpg");
    img2->Display_ASCII();
    img2->Display_CMD();
    */

    // some bit field filter driven code here
    RGBImage src;
    src.LoadImage("Image-Folder/bobo.jpg");

    cout << "=== Image Filter ===" << endl;
    cout << "Select filters to apply (enter numbers separated by spaces, then 0 to confirm):" << endl;
    cout << "  1. Box Filter (blur)" << endl;
    cout << "  2. Sobel Filter (edge detection)" << endl;
    cout << "  3. Contrast Stretching" << endl;
    cout << "  4. Mosaic Filter (pixelate)" << endl;

    uint8_t option = 0;
    int choice;
    while (cin >> choice && choice != 0)
    {
        if (choice == 1)
            option |= FILTER_BOX;
        else if (choice == 2)
            option |= FILTER_SOBEL;
        else if (choice == 3)
            option |= FILTER_CONTRAST;
        else if (choice == 4)
            option |= FILTER_MOSAIC;
    }

    RGBImage *out = applyFilters(src, option);
    out->DumpImage("filter_output.jpg");
    out->Display_CMD();
    delete out;

    /*
    // some photo mosaic driven code here
    PhotoMosaic pm(4);
    RGBImage *result = pm.Generate("Image-Folder/uu2.jpg", "Image-Folder/cifar10");
    result->DumpImage("mosaic_output.jpg");
    result->Display_CMD();
    delete result;
    */
    // more ...

    // delete img1;
    // delete img2;
    return 0;
}
