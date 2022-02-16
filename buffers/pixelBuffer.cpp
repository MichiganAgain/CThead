//
// Created by thomasgandy on 15/02/2022.
//

#include <stdexcept>
#include <GLFW/glfw3.h>

#include "pixelBuffer.h"
#include "../math/interpolation.h"


Pixel::Pixel(): Pixel(0, 0, 0) {}
Pixel::Pixel(GLubyte red, GLubyte green, GLubyte blue): red{red}, green{green}, blue{blue} {}
Pixel::Pixel(ImU32 colors) {
    this->red = (colors & IMGUI_COLOR_WHEEL_RED_MASK) >> IMGUI_COLOR_WHEEL_RED_SHIFTR;
    this->green = (colors & IMGUI_COLOR_WHEEL_GREEN_MASK) >> IMGUI_COLOR_WHEEL_GREEN_SHIFTR;
    this->blue = (colors & IMGUI_COLOR_WHEEL_BLUE_MASK) >> IMGUI_COLOR_WHEEL_BLUE_SHIFTR;
}

Pixel Pixel::lerp(Pixel p1, Pixel p2, float d) {
    auto interpolatedRed = static_cast<GLubyte>(MyMath::lerp(p1.red, p2.red, d));
    auto interpolatedGreen = static_cast<GLubyte>(MyMath::lerp(p1.green, p2.green, d));
    auto interpolatedBlue = static_cast<GLubyte>(MyMath::lerp(p1.blue, p2.blue, d));

    return {interpolatedRed, interpolatedGreen, interpolatedBlue};
}

std::ostream& operator << (std::ostream& os, const Pixel& p) {
    return os << "RED: " << (int)p.red << " GREEN: " << (int)p.green << " BLUE: " << (int)p.blue;
}



PixelBuffer::PixelBuffer(unsigned int rows, unsigned int cols): rows{rows}, cols{cols}, data(rows * cols * PixelBuffer::COLOUR_CHANNELS) {}
PixelBuffer::PixelBuffer(std::vector<Pixel> data, unsigned int rows, unsigned int cols): rows{rows}, cols{cols}, data(std::move(data)) {}

long PixelBuffer::calculatePixelOffset(unsigned int r, unsigned int c) const {
    return r * this->cols + c;
}

Pixel PixelBuffer::getPixelAt(unsigned int r, unsigned int c) const {
    long pixelOffset = this->calculatePixelOffset(r, c);
    return this->data[pixelOffset];
}

void PixelBuffer::setPixelAt(Pixel colourValues, unsigned int r, unsigned int c) {
    long pixelOffset = this->calculatePixelOffset(r, c);
    this->data[pixelOffset] = colourValues;
}

void PixelBuffer::clear() {
    std::fill(this->data.begin(), this->data.end(), Pixel());
}

bool PixelBuffer::validIndex(int r, int c) const {
    if (r < 0 || r >= this->rows || c < 0 || c >= this->cols) return false;
    return true;
}
