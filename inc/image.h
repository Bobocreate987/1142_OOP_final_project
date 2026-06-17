#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "data_loader.h"
using namespace std;

class Image{
protected:
    int w;
    int h;
    static Data_Loader data_loader;

public:
    Image();
    virtual ~Image();
    int get_w() const;
    int get_h() const;
    virtual bool LoadImage(string filename) = 0;
    virtual void DumpImage(string filename) = 0;
    virtual void Display_ASCII() = 0;
    virtual void Display_CMD() = 0;
};

#endif