//
// Created by thomasgandy on 13/02/2022.
//

#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>

#include "ctDataLoader.h"
#include "windows/window.h"


void CTDataLoader::convertRawNormalisedDataToImageData(std::vector<GLubyte> &rawData) {
    this->data.clear();

    for (int sliceNum = 0; sliceNum < this->slices; sliceNum++) {
        Image newImage(this->sliceHeight, this->sliceWidth);
        for (int r = 0; r < this->sliceHeight; r++) {
            for (int c = 0; c < this->sliceWidth; c++) {
                long rawDataOffset = sliceNum * this->sliceHeight * this->sliceWidth +
                                     r * this->sliceWidth + c;
                GLubyte ctPixelIntensity = rawData[rawDataOffset];
                newImage.setPixelAt({ctPixelIntensity, ctPixelIntensity, ctPixelIntensity}, r, c);
            }
        }
        this->data.push_back(newImage);
    }
}

void CTDataLoader::translateDataToFaceUp() {
    std::vector<Image> translatedData;
    int translatedRow, translatedCol;

    for (unsigned int row = 0; row < this->sliceHeight; row++) {
        Image newImage(this->slices, this->sliceWidth);
        translatedRow = 0;
        for (unsigned int slice = 0; slice < this->slices; slice++) {
            translatedCol = 0;
            // cast sliceWidth to int to avoid uint wrapping round to UINT_MAX
            for (int col = (int)this->sliceWidth - 1; col >= 0; col--) {
                Color color = this->data[slice].getPixelAt(row, col);
                newImage.setPixelAt(color, translatedRow, translatedCol++);
            }
            translatedRow++;
        }
        translatedData.push_back(newImage);
    }

    uint temp = slices;
    this->slices = this->sliceHeight;
    this->sliceHeight = temp;
    this->data = translatedData;
}

Image CTDataLoader::getSlice(unsigned int sliceNum) {
    if (!this->dataLoaded) throw std::runtime_error("Error retrieving data slice, as data has not yet been loaded");
    return this->data[sliceNum];
}

CTDataLoader::CTDataLoader(std::string file, uint sliceWidth, uint sliceHeight)
: file{std::move(file)}, sliceWidth{sliceWidth}, sliceHeight{sliceHeight} {

}
