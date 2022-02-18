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


void CTDataLoader::loadData(CTDataOrientation orientation) {
    std::vector<short> rawData;
    std::ifstream ifs(this->file, std::ifstream::binary | std::ifstream::ate);
    std::ifstream::pos_type posType = ifs.tellg();
    rawData.resize(posType / sizeof(short));
    this->slices = rawData.size() / (this->sliceWidth * this->sliceHeight);

    ifs.seekg(0, std::ifstream::beg);
    ifs.read(reinterpret_cast<char *>(rawData.data()), posType);

    CTDataLoader::normaliseData(rawData);
    CTDataLoader::convertRawNormalisedDataToImageData(rawData);
    if (orientation == CT_ORIENTATION_FACE_UP) this->translateDataToFaceUp();

    this->dataLoaded = true;
}

void CTDataLoader::normaliseData(std::vector<short>& rawData) {
    short smallest = rawData[0];
    short largest = rawData[0];
    for (short i : rawData) {
        smallest = std::min(smallest, i);
        largest = std::max(largest, i);
    }

    int diff = largest - smallest;
    int diffForSmallestFromZero = -smallest;
    for (short& i : rawData)
        i = static_cast<short>(static_cast<float>((i + diffForSmallestFromZero)) / static_cast<float>(diff) * 256);
}

void CTDataLoader::convertRawNormalisedDataToImageData(std::vector<short> &rawData) {
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
