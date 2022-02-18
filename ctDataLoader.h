//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_CTDATALOADER_H
#define CTHEAD_CTDATALOADER_H

#include <vector>
#include <fstream>

#include "buffers/image.h"


enum CTDataOrientation {CT_ORIENTATION_HEAD_UP, CT_ORIENTATION_FACE_UP};

class CTDataLoader {
    bool dataLoaded = false;
    std::vector<Image> data;

    template<typename T>
    std::vector<GLubyte> normaliseData(std::vector<T>& rawData);

    void convertRawNormalisedDataToImageData(std::vector<GLubyte>& rawData);
    void translateDataToFaceUp();

public:
    std::string file;
    unsigned int slices{};
    unsigned int sliceWidth;
    unsigned int sliceHeight;

    CTDataLoader(std::string file, uint sliceWidth, uint sliceHeight);

    template<typename T>
    void loadData(CTDataOrientation orientation = CT_ORIENTATION_HEAD_UP);
    Image getSlice(unsigned int sliceNum);
};

template<typename T>
void CTDataLoader::loadData(CTDataOrientation orientation) {
    std::vector<T> rawData;
    std::ifstream ifs(this->file, std::ifstream::binary | std::ifstream::ate);
    std::ifstream::pos_type posType = ifs.tellg();
    rawData.resize(posType / sizeof(T));
    this->slices = rawData.size() / (this->sliceWidth * this->sliceHeight);

    ifs.seekg(0, std::ifstream::beg);
    ifs.read(reinterpret_cast<char *>(rawData.data()), posType);

    std::vector<GLubyte> normalisedData = this->normaliseData(rawData);
    this->convertRawNormalisedDataToImageData(normalisedData);
    if (orientation == CT_ORIENTATION_FACE_UP) this->translateDataToFaceUp();

    this->dataLoaded = true;
}

template<typename T>
std::vector<GLubyte> CTDataLoader::normaliseData(std::vector<T>& rawData) {
    T smallest = rawData[0];
    T largest = rawData[0];
    for (T i : rawData) {
        smallest = std::min(smallest, i);
        largest = std::max(largest, i);
    }

    long double diff = largest - smallest;
    long double diffForSmallestFromZero = -smallest;
    std::vector<GLubyte> normalisedData(rawData.size());
    long long normalisedDataIndex = 0;
    for (T& i : rawData)
        normalisedData[normalisedDataIndex++] = static_cast<GLubyte>(static_cast<double>((i + diffForSmallestFromZero)) / static_cast<double>(diff) * 256);

    return normalisedData;
}

#endif //CTHEAD_CTDATALOADER_H
