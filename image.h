//
// Created by Keegan Millard on 2019-12-09.
//

#ifndef PATH_TRACER_IMAGE_H
#define PATH_TRACER_IMAGE_H

#include <iostream>

namespace rt {

struct Pixel {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    Pixel() = default;
    Pixel(uint8_t r, uint8_t g, uint8_t b);

    void set(uint8_t r, uint8_t g, uint8_t b);
};

struct Image {
    Pixel *pixels;
    const int width;
    const int height;

    Image(int width, int height);
    ~Image();
    Pixel &pxAt(int row, int col);
    const Pixel &pxAt(int row, int col) const;
    void writePgm(std::ostream &out) const;
    void writeBinaryPgm(std::ostream &out) const;
};

}



#endif //PATH_TRACER_IMAGE_H
