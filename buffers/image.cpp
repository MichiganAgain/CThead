//
// Created by thomasgandy on 15/02/2022.
//

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
    GLubyte lookup[256];
    Image::fillLookupTable(gamma, lookup);

    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->cols; c++) {
            Pixel adjustedPixel = this->getPixelAt(r, c).adjustGamma(gamma, lookup);
            this->setPixelAt(adjustedPixel, r, c);
        }
    }
}

void Image::fillLookupTable(float gamma, GLubyte lookup[256]) {
    constexpr float a = 1.f;

    for (int i = 0; i < 256; i++) {
        float iRatio = (float)i / (float)255;
        auto newValue = static_cast<GLubyte>(std::pow(iRatio / a, 1 / gamma) * 255);
        lookup[i] = newValue;
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
    for (unsigned int r = threadInfo.workerStartRow; r <= threadInfo.workerEndRow; r++) {
        for (unsigned int c = 0; c < threadInfo.newImage.cols; c++) {
            float ratioThroughNewImageWidth = static_cast<float>(c) / static_cast<float>(threadInfo.newImage.cols);
            float ratioThroughNewImageHeight = static_cast<float>(r) / static_cast<float>(threadInfo.newImage.rows);

            float xFloatPosInOldImage = ratioThroughNewImageWidth * static_cast<float>(threadInfo.oldImage.cols);
            float yFloatPosInOldImage = ratioThroughNewImageHeight * static_cast<float>(threadInfo.oldImage.rows);
            float xFloatPosInOldImageFractionalPart = xFloatPosInOldImage - static_cast<float>(static_cast<int>(xFloatPosInOldImage));
            float yFloatPosInOldImageFractionalPart = yFloatPosInOldImage - static_cast<float>(static_cast<int>(yFloatPosInOldImage));

            bool pixelOnLeftOfInterpolation = false;
            bool pixelOnTopOfInterpolation = false;
            if (xFloatPosInOldImageFractionalPart > 0.5f) pixelOnLeftOfInterpolation = true;
            if (yFloatPosInOldImageFractionalPart > 0.5f) pixelOnTopOfInterpolation = true;

            Pixel topLeftPixel, topRightPixel, bottomLeftPixel, bottomRightPixel;
            auto startPixelXIndex = static_cast<unsigned int>(xFloatPosInOldImage); // Start here just represents the first pixel
            auto startPixelYIndex = static_cast<unsigned int>(yFloatPosInOldImage); // the scaling down landed on
            unsigned int endPixelXIndex, endPixelYIndex; // These can be above or below the start index values

            if (pixelOnLeftOfInterpolation) endPixelXIndex = std::min(threadInfo.oldImage.cols - 1, startPixelXIndex + 1);
            else endPixelXIndex = std::max(0, (int)startPixelXIndex - 1);
            if (pixelOnTopOfInterpolation) endPixelYIndex = std::min(threadInfo.oldImage.rows - 1, startPixelYIndex + 1);
            else endPixelYIndex = std::max(0, (int)startPixelYIndex - 1);

            topLeftPixel = threadInfo.oldImage.getPixelAt(std::min(startPixelYIndex, endPixelYIndex), std::min(startPixelXIndex, endPixelXIndex));
            topRightPixel = threadInfo.oldImage.getPixelAt(std::min(startPixelYIndex, endPixelYIndex), std::max(startPixelXIndex, endPixelXIndex));
            bottomLeftPixel = threadInfo.oldImage.getPixelAt(std::max(startPixelYIndex, endPixelYIndex), std::min(startPixelXIndex, endPixelXIndex));
            bottomRightPixel = threadInfo.oldImage.getPixelAt(std::max(startPixelYIndex, endPixelYIndex), std::max(startPixelXIndex, endPixelXIndex));

            if (pixelOnLeftOfInterpolation) xFloatPosInOldImageFractionalPart -= .5f;
            else xFloatPosInOldImageFractionalPart += .5f;
            if (pixelOnTopOfInterpolation) yFloatPosInOldImageFractionalPart -= .5f;
            else yFloatPosInOldImageFractionalPart += .5f;

            Color topHorizontalInterpolation = Pixel::lerp(topLeftPixel, topRightPixel, xFloatPosInOldImageFractionalPart);
            Color bottomHorizontalInterpolation = Pixel::lerp(bottomLeftPixel, bottomRightPixel, xFloatPosInOldImageFractionalPart);
            Color verticalInterpolation = Pixel::lerp(topHorizontalInterpolation, bottomHorizontalInterpolation, yFloatPosInOldImageFractionalPart);

            threadInfo.newImage.setPixelAt(verticalInterpolation, r, c);
        }
    }
}
