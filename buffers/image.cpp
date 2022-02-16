//
// Created by thomasgandy on 15/02/2022.
//

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <utility>

#include "image.h"


Image::Image(unsigned int rows, unsigned int cols): PixelBuffer(rows, cols) { }
Image::Image(std::vector<Pixel> data, unsigned int rows, unsigned int cols): PixelBuffer(std::move(data), rows, cols){ }


void Image::adjustColor(Color color) {
    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->cols; c++) {
            Color newPixelColor = this->getPixelAt(r, c);
            newPixelColor.red = std::min((newPixelColor.red / (float)color.red) * 255, (float)255);
            newPixelColor.green = std::min((newPixelColor.green / (float)color.green) * 255, (float)255);
            newPixelColor.blue = std::min((newPixelColor.blue / (float)color.blue) * 255, (float)255);
            this->setPixelAt(newPixelColor, r, c);
        }
    }
}

Image Image::resize(const Image &oldImage, unsigned int newWidth, unsigned int newHeight) {
    return Image::nearestNeighbourResize(oldImage, newWidth, newHeight);
//    return Image::bilinearResize(oldImage, newWidth, newHeight);
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

    for (int r = 0; r < newHeight; r++) {
        for (int c = 0; c < newWidth; c++) {
            float colRatio = static_cast<float>(c) / static_cast<float>(newWidth);
            float rowRatio = static_cast<float>(r) / static_cast<float>(newHeight);
            auto oldImageRow = static_cast<unsigned int>(rowRatio * static_cast<float>(oldImage.rows));
            auto oldImageCol = static_cast<unsigned int>(colRatio * static_cast<float>(oldImage.cols));
            unsigned int oldImageNextRow = std::min(oldImageRow + 1, oldImage.rows - 1);
            unsigned int oldImageNextCol = std::min(oldImageCol + 1, oldImage.cols - 1);

            Pixel topLeftPixel = oldImage.getPixelAt(oldImageRow, oldImageCol);
            Pixel topRightPixel = oldImage.getPixelAt(oldImageRow, oldImageNextCol);
            Pixel bottomLeftPixel = oldImage.getPixelAt(oldImageNextRow, oldImageCol);
            Pixel bottomRightPixel = oldImage.getPixelAt(oldImageNextRow, oldImageNextCol);

            Pixel topHorizontalInterpolation = Pixel::lerp(topLeftPixel, topRightPixel, colRatio);
            Pixel bottomHorizontalInterpolation = Pixel::lerp(bottomLeftPixel, bottomRightPixel, colRatio);
            Pixel verticalInterpolation = Pixel::lerp(topHorizontalInterpolation, bottomHorizontalInterpolation, rowRatio);

            newImage.setPixelAt(verticalInterpolation, r, c);
        }
    }

    return newImage;
}
