#include "image.h"
#include "gray_image.h"
#include "rgb_image.h"
#include "photo_mosaic.h"
#include "bit_field_filter.h"
#include <iostream>
#include <string>
#include <bitset>
#include <limits>
#include <vector>
using namespace std;

// ── UI Helpers ────────────────────────────────────────────

void separator(const string& title) {
    cout << "\n+--------------------------------------------------+\n";
    cout << "  " << title << "\n";
    cout << "+--------------------------------------------------+\n";
}

void wait_enter() {
    cout << "\n  [Press Enter to continue...] " << flush;
    cin.get();
}

// ── Step 1: Data Loader ───────────────────────────────────

void demo_step1() {
    separator("Step 1 — Data Loader");
    Data_Loader dl;

    cout << "\n[1/3] Load Gray image (mnist/img_0.jpg)\n";
    int w, h;
    int** gray = dl.Load_Gray("Image-Folder/mnist/img_0.jpg", &w, &h);
    cout << "  Size: " << w << " x " << h << "\n";
    cout << "  ASCII display:\n\n";
    dl.Display_Gray_ASCII(w, h, gray);
    dl.Dump_Gray(w, h, gray, "dl_gray.jpg");
    cout << "\n  CMD display:\n";
    dl.Display_Gray_CMD("dl_gray.jpg");
    cout << "  Saved -> dl_gray.jpg\n";
    for (int i = 0; i < h; i++) delete[] gray[i];
    delete[] gray;

    wait_enter();

    cout << "\n[2/3] Load RGB image (lena.jpg)\n";
    int w2, h2;
    int*** rgb = dl.Load_RGB("Image-Folder/lena.jpg", &w2, &h2);
    cout << "  Size: " << w2 << " x " << h2 << "\n";
    cout << "  ASCII display:\n\n";
    dl.Display_RGB_ASCII(w2, h2, rgb);
    dl.Dump_RGB(w2, h2, rgb, "dl_rgb.jpg");
    cout << "\n  CMD display:\n";
    dl.Display_RGB_CMD("dl_rgb.jpg");
    cout << "  Saved -> dl_rgb.jpg\n";
    for (int i = 0; i < h2; i++) {
        for (int j = 0; j < w2; j++) delete[] rgb[i][j];
        delete[] rgb[i];
    }
    delete[] rgb;

    wait_enter();

    cout << "\n[3/3] List directory: Image-Folder/mnist\n";
    vector<string> files;
    dl.List_Directory("Image-Folder/mnist", files);
    int shown = 0;
    for (const auto& f : files) {
        cout << "  " << f << "\n";
        if (++shown >= 8) {
            cout << "  ... (" << (int)files.size() - 8 << " more files)\n";
            break;
        }
    }
    cout << "  Total: " << files.size() << " files\n";
}

// ── Step 2: Gray & RGB Image (OOP / Polymorphism) ─────────

void demo_step2() {
    separator("Step 2 — Gray & RGB Image Loading");

    cout << "\n[1/2] GrayImage — loaded via polymorphic Image* interface\n";
    cout << "  Source: Image-Folder/mnist/img_0.jpg\n\n";
    Image* gray = new GrayImage();
    gray->LoadImage("Image-Folder/mnist/img_0.jpg");
    gray->DumpImage("out_gray.jpg");
    cout << "  ASCII display:\n";
    gray->Display_ASCII();
    cout << "\n  CMD display (catimg):\n";
    gray->Display_CMD();
    cout << "  Saved -> out_gray.jpg\n";
    delete gray;

    wait_enter();

    cout << "\n[2/2] RGBImage — loaded via polymorphic Image* interface\n";
    cout << "  Source: Image-Folder/lena.jpg\n\n";
    Image* rgb = new RGBImage();
    rgb->LoadImage("Image-Folder/lena.jpg");
    rgb->DumpImage("out_rgb.jpg");
    cout << "  ASCII display:\n";
    rgb->Display_ASCII();
    cout << "\n  CMD display (catimg):\n";
    rgb->Display_CMD();
    cout << "  Saved -> out_rgb.jpg\n";
    delete rgb;
}

// ── Step 3: Bit Field Filter ──────────────────────────────

void demo_step3() {
    separator("Step 3 — Bit Field Filters");

    cout << "\n  Each filter maps to one bit in a uint8_t:\n\n";
    cout << "    FILTER_BOX      = 0b" << bitset<8>(FILTER_BOX)      << "  (bit 0)\n";
    cout << "    FILTER_SOBEL    = 0b" << bitset<8>(FILTER_SOBEL)    << "  (bit 1)\n";
    cout << "    FILTER_CONTRAST = 0b" << bitset<8>(FILTER_CONTRAST) << "  (bit 2)\n";
    cout << "    FILTER_MOSAIC   = 0b" << bitset<8>(FILTER_MOSAIC)   << "  (bit 3)\n";

    cout << "\n  Combine with OR: e.g. BOX | SOBEL = 0b"
         << bitset<8>(FILTER_BOX | FILTER_SOBEL) << "\n";

    cout << "\nSelect filters to apply (space-separated, 0 to confirm):\n";
    cout << "  1=Box  2=Sobel  3=Contrast  4=Mosaic\n> ";

    uint8_t option = 0;
    int sel;
    while (cin >> sel && sel != 0) {
        if      (sel == 1) option |= FILTER_BOX;
        else if (sel == 2) option |= FILTER_SOBEL;
        else if (sel == 3) option |= FILTER_CONTRAST;
        else if (sel == 4) option |= FILTER_MOSAIC;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "\n  option = 0b" << bitset<8>(option)
         << "  (decimal: " << (int)option << ")\n";
    cout << "  Active filters:";
    if (!option)                  cout << " (none)";
    if (option & FILTER_BOX)      cout << " [Box]";
    if (option & FILTER_SOBEL)    cout << " [Sobel]";
    if (option & FILTER_CONTRAST) cout << " [Contrast]";
    if (option & FILTER_MOSAIC)   cout << " [Mosaic]";
    cout << "\n\n  Applying to Image-Folder/bobo.jpg ...\n";

    RGBImage src;
    src.LoadImage("Image-Folder/bobo.jpg");
    RGBImage* out = applyFilters(src, option);
    out->DumpImage("filter_output.jpg");
    cout << "  CMD display:\n";
    out->Display_CMD();
    cout << "  Saved -> filter_output.jpg\n";
    delete out;
}

// ── Step 4: Photo Mosaic ──────────────────────────────────

void demo_step4() {
    separator("Step 4 — Photo Mosaic");
    cout << "\n  Target image : Image-Folder/uu2.jpg\n";
    cout << "  Tile library : Image-Folder/cifar10/\n";
    cout << "  Tile size    : 4 px\n";
    cout << "\n  Generating mosaic (this may take a moment)...\n";

    PhotoMosaic pm(4);
    RGBImage* result = pm.Generate("Image-Folder/uu2.jpg", "Image-Folder/cifar10");
    if (result) {
        result->DumpImage("mosaic_output.jpg");
        cout << "\n  CMD display:\n";
        result->Display_CMD();
        cout << "  Saved -> mosaic_output.jpg\n";
        delete result;
    } else {
        cout << "  [Error] Failed to generate mosaic.\n";
    }
}

// ── Main Menu ─────────────────────────────────────────────

int main() {
    int choice;
    do {
        separator("Image Processing — Main Menu");
        cout << "  [1] Step 1  Data Loader\n";
        cout << "  [2] Step 2  Gray & RGB Image Loading\n";
        cout << "  [3] Step 3  Bit Field Filters\n";
        cout << "  [4] Step 4  Photo Mosaic\n";
        cout << "  [0] Exit\n";
        cout << "> ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: demo_step1(); break;
            case 2: demo_step2(); break;
            case 3: demo_step3(); break;
            case 4: demo_step4(); break;
            case 0: cout << "\nGoodbye!\n"; break;
            default: cout << "  Invalid choice, please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
