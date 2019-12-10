//
// Created by Keegan Millard on 2019-12-09.
//

#include "image.h"

using namespace rt;

void Pixel::set(uint8_t r, uint8_t g, uint8_t b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

Pixel::Pixel(uint8_t r_, uint8_t g_, uint8_t b_): r(r_), g(g_), b(b_) {}

Image::Image(const int width, const int height):width(width), height(height) {
    this->pixels = new Pixel[width*height];
    memset(this->pixels, 0, sizeof(Pixel)*width*height);
}

Image::~Image() {
    delete pixels;
}

Pixel &Image::pxAt(int row, int col) {
    return pixels[width*row+col];
}

const Pixel &Image::pxAt(int row, int col) const {
    return pixels[width*row+col];
}

void Image::writePgm(std::ostream &out) const {
    out << "P3\n" << width << " " << height << " 255\n";
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            Pixel px = pxAt(r, c);
            out << std::to_string(px.r) << " " << std::to_string(px.g) << " " << std::to_string(px.b) << " ";
        }
        out << "\n";
    }
}

void Image::writeBinaryPgm(std::ostream &out) const {
    out << "P6\n" << width << " " << height << " 255\n";
    char *row = new char[width*3];
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            const Pixel &px = pxAt(r, c);
            row[3*c+0] = px.r;
            row[3*c+1] = px.g;
            row[3*c+2] = px.b;
        }
        out.write(row, width*3);
    }
    delete[] row;
}

