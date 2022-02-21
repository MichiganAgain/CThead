//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_CTDATALOADER_H
#define CTHEAD_CTDATALOADER_H

#include <vector>
#include <fstream>
#include <filesystem>

#include "buffers/image.h"
#include "math/interpolation.h"


enum CTDataOrientation {CT_ORIENTATION_HEAD_UP, CT_ORIENTATION_FACE_UP};
enum CTDataFormat {CT_FORMAT_GREYSCALE, CT_FORMAT_RGB};

class CTDataLoader {
    bool dataLoaded = false;
    std::vector<Image> data;
    unsigned int defaultSliceWidth;
    unsigned int defaultSliceHeight;
    CTDataFormat dataFormat = CT_FORMAT_GREYSCALE;

    template<typename T>
    std::vector<GLubyte> normaliseData(std::vector<T>& rawData);
    void convertRawNormalisedDataToImageData(std::vector<GLubyte>& rawData);

public:
    std::string file;
    unsigned int slices{};
    unsigned int sliceWidth;
    unsigned int sliceHeight;
    bool dataModified = false;

    CTDataLoader(std::string file, uint sliceWidth, uint sliceHeight);

    void rotateAlongX();
    void rotateAlongY();
    void rotateAlongZ();

    template<typename T>
    bool changeDataSource(const std::string& filename, int width, int height, CTDataFormat format = CT_FORMAT_GREYSCALE);
    template<typename T>
    void loadData(CTDataOrientation orientation = CT_ORIENTATION_HEAD_UP);
    Image getSlice(unsigned int sliceNum);
    [[nodiscard]] std::string getFilename() const;
    [[nodiscard]] unsigned int getSliceWidth() const;
    [[nodiscard]] unsigned int getSliceHeight() const;
};



template<typename T>
bool CTDataLoader::changeDataSource(const std::string& filename, int width, int height, CTDataFormat format) {
    if (!std::filesystem::exists(filename)) return false;

    this->file = filename;
    this->sliceWidth = this->defaultSliceWidth = width;
    this->sliceHeight = this->defaultSliceHeight = height;
    this->dataFormat = format;
    this->loadData<T>();

    this->dataModified = true;
    return true;
}

template<typename T>
void CTDataLoader::loadData(CTDataOrientation orientation) {
    this->sliceWidth = this->defaultSliceWidth;
    this->sliceHeight = this->defaultSliceHeight;

    std::vector<T> rawData;
    std::ifstream ifs(this->file, std::ifstream::binary | std::ifstream::ate);
    std::ifstream::pos_type posType = ifs.tellg();
    rawData.resize(posType / sizeof(T));
    this->slices = rawData.size() / (this->sliceWidth * this->sliceHeight);

    ifs.seekg(0, std::ifstream::beg);
    ifs.read(reinterpret_cast<char *>(rawData.data()), posType);

    std::vector<GLubyte> normalisedData = this->normaliseData(rawData);
    this->convertRawNormalisedDataToImageData(normalisedData);

    this->dataLoaded = true;
    this->dataModified = true;
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
    long double smallestAllowedValue = MyMath::lerp(smallest, largest, 0.0);
    smallest = smallestAllowedValue;
    diff = largest - smallestAllowedValue;
    long double diffForSmallestFromZero = -smallest;

    std::vector<GLubyte> normalisedData(rawData.size());
    long long normalisedDataIndex = 0;
    for (T& i : rawData) {
        auto valueToNonNegative = static_cast<double>(i + diffForSmallestFromZero);
        if (valueToNonNegative < 0) normalisedData[normalisedDataIndex++] = 0;
        else {
            // normalised byte
            GLubyte normalByte = std::min(static_cast<GLubyte>(valueToNonNegative / static_cast<double>(diff) * 256), (GLubyte)255);
            normalisedData[normalisedDataIndex++] = normalByte;
        }
    }

    return normalisedData;
}

#endif //CTHEAD_CTDATALOADER_H
