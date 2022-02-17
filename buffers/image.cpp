//
// Created by thomasgandy on 15/02/2022.
//

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <algorithm>
#include <utility>
#include <thread>
#include <cmath>

#include "image.h"


Image::Image(unsigned int rows, unsigned int cols): PixelBuffer(rows, cols) { }
Image::Image(std::vector<Pixel> data, unsigned int rows, unsigned int cols): PixelBuffer(std::move(data), rows, cols){ }

ImageResizeInfo::ImageResizeInfo(const Image &oldImage, Image &newImage): oldImage{oldImage}, newImage{newImage} { }

void Image::adjustColor(Color color) {
    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->cols; c++) {
            Color newPixelColor = this->getPixelAt(r, c);

//            newPixelColor.red = std::min((newPixelColor.red / (float)color.red) * 255, (float)255);
//            newPixelColor.green = std::min((newPixelColor.green / (float)color.green) * 255, (float)255);
//            newPixelColor.blue = std::min((newPixelColor.blue / (float)color.blue) * 255, (float)255);
//
            float rf = static_cast<float>(newPixelColor.red) / 255;
            float gf = static_cast<float>(newPixelColor.green) / 255;
            float bf = static_cast<float>(newPixelColor.blue) / 255;
            float crf = static_cast<float>(color.red) / 255;
            float cgf = static_cast<float>(color.green) / 255;
            float cbf = static_cast<float>(color.blue) / 255;

            newPixelColor.red = static_cast<GLubyte>(rf * crf * 255);
            newPixelColor.green = static_cast<GLubyte>(gf * cgf * 255);
            newPixelColor.blue = static_cast<GLubyte>(bf * cbf * 255);
            this->setPixelAt(newPixelColor, r, c);
        }
    }
}

void Image::adjustGamma(float gamma) {
//    std::unordered_map<Pixel, Pixel> gammaLookup;
    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->cols; c++) {
            Pixel adjustedPixel = this->getPixelAt(r, c).adjustGamma(gamma);
            this->setPixelAt(adjustedPixel, r, c);
        }
    }
}

Image Image::nearestNeighbourResize(const Image &oldImage, unsigned int newWidth, unsigned int newHeight) {
    Image newImage(newHeight, newWidth);
    for (int r = 0; r < newHeight; r++) {
        for (int c = 0; c < newWidth; c++) {
            int oldImageRow = static_cast<int>(static_cast<float>(r) * static_cast<float>(oldImage.rows) / static_cast<float>(newHeight));
            int oldImageCol = static_cast<int>(static_cast<float>(c) * static_cast<float>(oldImage.cols) / static_cast<float>(newWidth));
            newImage.setPixelAt(oldImage.getPixelAt(oldImageRow, oldImageCol), r, c);
        }
    }

    return newImage;
}

Image Image::bilinearResize(const Image &oldImage, unsigned int newWidth, unsigned int newHeight) {
    Image newImage(newHeight, newWidth);

    ImageResizeInfo imageResizeInfo(oldImage, newImage);
    imageResizeInfo.newWidth = newWidth;
    imageResizeInfo.newHeight = newHeight;

    std::vector<std::thread> workerThreads(MAX_THREADS);
    for (int i = 0; i < Image::MAX_THREADS; i++) {
        unsigned int threadStartRow = (newHeight / Image::MAX_THREADS) * i;
        unsigned int threadEndRow = (newHeight / Image::MAX_THREADS) * (i + 1);
        if (i == Image::MAX_THREADS - 1) threadEndRow = newHeight;

        imageResizeInfo.workerStartRow = threadStartRow;
        imageResizeInfo.workerEndRow = threadEndRow;
        workerThreads[i] = std::thread(Image::bilinearResizeWorker, imageResizeInfo);
    }
    for (std::thread& t : workerThreads) t.join();

    return newImage;
}

void Image::bilinearResizeWorker(const ImageResizeInfo threadInfo) {
    for (unsigned int r = threadInfo.workerStartRow; r < threadInfo.workerEndRow; r++) {
        for (unsigned int c = 0; c < threadInfo.newWidth; c++) {
            float colRatio = static_cast<float>(c) / static_cast<float>(threadInfo.newWidth);
            float rowRatio = static_cast<float>(r) / static_cast<float>(threadInfo.newHeight);
            auto oldImageRow = static_cast<unsigned int>(rowRatio * static_cast<float>(threadInfo.oldImage.rows));
            auto oldImageCol = static_cast<unsigned int>(colRatio * static_cast<float>(threadInfo.oldImage.cols));
            unsigned int oldImageNextRow = std::min(oldImageRow + 1, threadInfo.oldImage.rows - 1);
            unsigned int oldImageNextCol = std::min(oldImageCol + 1, threadInfo.oldImage.cols - 1);

            float rd = rowRatio * static_cast<float>(threadInfo.oldImage.rows) - static_cast<float>(oldImageRow);
            float cd = colRatio * static_cast<float>(threadInfo.oldImage.cols) - static_cast<float>(oldImageCol);

            Pixel topLeftPixel = threadInfo.oldImage.getPixelAt(oldImageRow, oldImageCol);
            Pixel topRightPixel = threadInfo.oldImage.getPixelAt(oldImageRow, oldImageNextCol);
            Pixel bottomLeftPixel = threadInfo.oldImage.getPixelAt(oldImageNextRow, oldImageCol);
            Pixel bottomRightPixel = threadInfo.oldImage.getPixelAt(oldImageNextRow, oldImageNextCol);

            Pixel topHorizontalInterpolation = Pixel::lerp(topLeftPixel, topRightPixel, cd);
            Pixel bottomHorizontalInterpolation = Pixel::lerp(bottomLeftPixel, bottomRightPixel, cd);

            Pixel verticalInterpolation = Pixel::lerp(topHorizontalInterpolation, bottomHorizontalInterpolation, rd);

            threadInfo.newImage.setPixelAt(verticalInterpolation, r, c);
        }
    }
}
