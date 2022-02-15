//
// Created by thomasgandy on 15/02/2022.
//

#include <stdexcept>
#include <GLFW/glfw3.h>

#include "pixelBuffer.h"


Pixel::Pixel(): Pixel(0, 0, 0) {}
Pixel::Pixel(GLubyte red, GLubyte green, GLubyte blue): red{red}, green{green}, blue{blue} {}


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
