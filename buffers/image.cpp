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
    for (unsigned int r = threadInfo.workerStartRow; r <= threadInfo.workerEndRow; r++) {
        for (unsigned int c = 0; c < threadInfo.newWidth; c++) {
            // Get % of way through new image width and height as float value between 0 and 1 inclusive
            float colRatio = static_cast<float>(c) / static_cast<float>(threadInfo.newWidth);
            float rowRatio = static_cast<float>(r) / static_cast<float>(threadInfo.newHeight);

            // Get floating index in old image the current % way through the new image maps to
            float oldImageRow = rowRatio * static_cast<float>(threadInfo.oldImage.rows);
            float oldImageCol = colRatio * static_cast<float>(threadInfo.oldImage.cols);

            unsigned int topRow = std::max((int)0, static_cast<int>(std::floor(oldImageRow - 0.5f)));
            unsigned int bottomRow = std::min((int)threadInfo.oldImage.rows - 1, static_cast<int>(std::floor(oldImageRow + 0.5f)));
            unsigned int leftCol = std::max((int)0, static_cast<int>(std::floor(oldImageCol - 0.5f)));
            unsigned int rightCol = std::min((int)threadInfo.oldImage.cols - 1, static_cast<int>(std::floor(oldImageCol + 0.5f)));

            Pixel topLeftPixel = threadInfo.oldImage.getPixelAt(topRow, leftCol);
            Pixel topRightPixel = threadInfo.oldImage.getPixelAt(topRow, rightCol);
            Pixel bottomLeftPixel = threadInfo.oldImage.getPixelAt(bottomRow, leftCol);
            Pixel bottomRightPixel = threadInfo.oldImage.getPixelAt(bottomRow, rightCol);


            float columnDistance = oldImageCol - 0.5f - static_cast<float>(static_cast<int>(oldImageCol));
            float rowDistance = oldImageRow - 0.5f - static_cast<float>(static_cast<int>(oldImageRow));
//            std::cout << "OldImageRow " << oldImageRow << "\tOldImageCol " << oldImageCol << std::endl;
//            std::cout << "Column Distance " << columnDistance << "\tRow Distance " << rowDistance << std::endl;
            if (columnDistance < 0) columnDistance += 1.f;
            if (rowDistance < 0) rowDistance += 1.f;

//            std::cout << "TLP " << topLeftPixel << "\nTRP " << topRightPixel << "\nBLP " << bottomLeftPixel << "\nBRP " << bottomRightPixel << std::endl;
            Pixel topHorizontalInterpolation = Pixel::lerp(topLeftPixel, topRightPixel, columnDistance);
            Pixel bottomHorizontalInterpolation = Pixel::lerp(bottomLeftPixel, bottomRightPixel, columnDistance);
            Pixel verticalInterpolation = Pixel::lerp(topHorizontalInterpolation, bottomHorizontalInterpolation, rowDistance);
//            std::cout << "THL " << topHorizontalInterpolation << "\nBHL " << bottomHorizontalInterpolation << "\nVL " << verticalInterpolation << std::endl;
//            std::cout << "Column Distance " << columnDistance << "\tRow Distance " << rowDistance << std::endl << std::endl;

            threadInfo.newImage.setPixelAt(verticalInterpolation, r, c);
        }
    }
}
