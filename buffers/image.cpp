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

            // Get the RGB floating values for the current pixel you are wanting to change (newPixelColor)
            float rf = static_cast<float>(newPixelColor.red) / 255;
            float gf = static_cast<float>(newPixelColor.green) / 255;
            float bf = static_cast<float>(newPixelColor.blue) / 255;

            // Get the RGB floating values for the adjustment colour
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
            Pixel adjustedPixel = this->getPixelAt(r, c).adjustGamma(lookup);
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
            float xInterpolationDistance = (xFloatPosInOldImageFractionalPart < .5f) ? xFloatPosInOldImageFractionalPart + .5f: xFloatPosInOldImageFractionalPart - .5f;
            float yInterpolationDistance = (yFloatPosInOldImageFractionalPart < .5f) ? yFloatPosInOldImageFractionalPart + .5f: yFloatPosInOldImageFractionalPart - .5f;

            unsigned int leftX = std::max(0, (int)std::floor(xFloatPosInOldImage - .5f));
            unsigned int rightX = std::min(threadInfo.oldImage.cols - 1, (uint)std::floor(xFloatPosInOldImage + .5f));
            unsigned int topY = std::max(0, (int)std::floor(yFloatPosInOldImage - .5f));
            unsigned int bottomY = std::min(threadInfo.oldImage.rows - 1, (uint)std::floor(yFloatPosInOldImage + .5f));

            Pixel topLeft = threadInfo.oldImage.getPixelAt(topY, leftX);
            Pixel topRight = threadInfo.oldImage.getPixelAt(topY, rightX);
            Pixel bottomLeft = threadInfo.oldImage.getPixelAt(bottomY, leftX);
            Pixel bottomRight = threadInfo.oldImage.getPixelAt(bottomY, rightX);

            Pixel topHorizontalInterpolation = Pixel::lerp(topLeft, topRight, xInterpolationDistance);
            Pixel bottomHorizontalInterpolation = Pixel::lerp(bottomLeft, bottomRight, xInterpolationDistance);
            Pixel verticalInterpolation = Pixel::lerp(topHorizontalInterpolation, bottomHorizontalInterpolation, yInterpolationDistance);

            threadInfo.newImage.setPixelAt(verticalInterpolation, r, c);
        }
    }
}
