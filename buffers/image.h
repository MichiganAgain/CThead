//
// Created by thomasgandy on 15/02/2022.
//

#ifndef CTHEAD_IMAGE_H
#define CTHEAD_IMAGE_H

#include "pixelBuffer.h"
#include <vector>

struct Image: public PixelBuffer {
    Image(unsigned int rows, unsigned int cols);
    Image(std::vector<Pixel> data, unsigned int rows, unsigned int cols);

    static Image resize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);
    static Image nearestNeighbourResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);
    static Image bilinearResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);
};

#endif //CTHEAD_IMAGE_H
