//
// Created by Keegan Millard on 2019-12-09.
//

#ifndef PATH_TRACER_IMAGE_H
#define PATH_TRACER_IMAGE_H

#include <iostream>
#include <vector>

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
    std::vector<Pixel> pixels;
    int width;
    int height;

    Image(int width, int height);
    Image(std::string filename);

    Pixel &pxAt(int row, int col);
    const Pixel &pxAt(int row, int col) const;
    void writePgm(std::ostream &out) const;
    void writeBinaryPgm(std::ostream &out) const;


    Pixel sampleAt(double a, double b) const;
};

    typedef Image Texture;

}



#endif //PATH_TRACER_IMAGE_H
