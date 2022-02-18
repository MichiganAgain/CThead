//
// Created by thomasgandy on 15/02/2022.
//

#ifndef CTHEAD_IMAGE_H
#define CTHEAD_IMAGE_H

#include "pixelBuffer.h"
#include <vector>


struct ImageResizeInfo;

struct Image: public PixelBuffer {
    static constexpr int MAX_THREADS = 4;

    Image(unsigned int rows, unsigned int cols);
    Image(std::vector<Pixel> data, unsigned int rows, unsigned int cols);

    void adjustColor(Color c);
    void adjustGamma(float gamma);
    static void fillLookupTable(float gamma, GLubyte lookup[256]);
    static Image nearestNeighbourResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);
    static Image bilinearResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);

private:
    static void bilinearResizeWorker(ImageResizeInfo threadInfo);
};

struct ImageResizeInfo {
    const Image& oldImage;
    Image& newImage;
    uint newWidth, newHeight;
    uint workerStartRow, workerEndRow;

    ImageResizeInfo(const Image& oldImage, Image& newImage);
};

#endif //CTHEAD_IMAGE_H
