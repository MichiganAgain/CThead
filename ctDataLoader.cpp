//
// Created by thomasgandy on 13/02/2022.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "ctDataLoader.h"


bool CTDataLoader::dataLoaded = false;
std::vector<short> CTDataLoader::data;


Image::Image(unsigned int rows, unsigned int cols): rows{rows}, cols{cols}, data(rows * cols) { }
Image::Image(std::vector<short> data, unsigned int rows, unsigned int cols): data{std::move(data)}, rows{rows}, cols{cols} { }

short Image::valueAt(int row, int col) const {
    if (row < 0 || row >= this->rows || col < 0 || col >= this->cols)
        throw std::runtime_error("Out of bounds for image's valueAt function");
    return this->data[row * this->cols + col];
}

void Image::setValueAt(short newValue, int row, int col) {
    if (row < 0 || row >= this->rows || col < 0 || col >= this->cols)
        throw std::runtime_error("Out of bounds for image's valueAt function");
    this->data[row * this->cols + col] = newValue;
}

Image Image::resize(const Image &oldImage, unsigned int newWidth, unsigned int newHeight) {
    return Image::nearestNeighbourResize(oldImage, newWidth, newHeight);
}

Image Image::nearestNeighbourResize(const Image &oldImage, unsigned int newWidth, unsigned int newHeight) {
    Image newImage(newHeight, newWidth);
    for (int r = 0; r < newHeight; r++) {
        for (int c = 0; c < newWidth; c++) {
            int oldImageRow = static_cast<int>(static_cast<float>(r) * static_cast<float>(oldImage.rows) / static_cast<float>(newHeight));
            int oldImageCol = static_cast<int>(static_cast<float>(c) * static_cast<float>(oldImage.cols) / static_cast<float>(newWidth));
            newImage.setValueAt(oldImage.valueAt(oldImageRow, oldImageCol), r, c);
        }
    }

    return newImage;
}

Image Image::bilinearResize(const Image &oldImage, unsigned int newWidth, unsigned int newHeight) {
    Image newImage(newHeight, newWidth);
    return {{}, 0, 0};
}

void CTDataLoader::loadData() {
    if (CTDataLoader::dataLoaded) return;

    std::ifstream file("/home/thomasgandy/CLionProjects/CThead/resources/CThead", std::ifstream::binary | std::ifstream::ate);
    std::ifstream::pos_type posType = file.tellg();
    CTDataLoader::data.resize(posType);

    file.seekg(0, std::ifstream::beg);
    file.read(reinterpret_cast<char *>(CTDataLoader::data.data()), posType);
    CTDataLoader::normaliseData();


//    FILE* file = fopen("/home/thomasgandy/CLionProjects/CThead/resources/CThead", "rb");
//    fseek(file, 0L, SEEK_END);
//    long fileSizeInBytes = ftell(file);
//    long fileSizeInShorts = fileSizeInBytes / (signed long)sizeof(short);
//    fseek(file, 0L, SEEK_SET);
//
//    CTDataLoader::data.resize(fileSizeInShorts);
//    std::cout << "Bytes read into buffer: " << fread(CTDataLoader::data.data(), sizeof(short), fileSizeInShorts, file) << std::endl;

    CTDataLoader::dataLoaded = true;
}

void CTDataLoader::normaliseData() {
    short smallest = CTDataLoader::data[0];
    short largest = CTDataLoader::data[0];
    for (short i : CTDataLoader::data) {
        smallest = std::min(smallest, i);
        largest = std::max(largest, i);
    }

    int diff = largest - smallest;
    int diffForSmallestFromZero = -smallest;
    for (short& i : CTDataLoader::data)
        i = static_cast<short>(static_cast<float>((i + diffForSmallestFromZero)) / static_cast<float>(diff) * 256);
}


Image CTDataLoader::getSlice(int sliceNum) {
    auto startIterator = CTDataLoader::data.begin() + sliceNum * CTDataLoader::SLICE_HEIGHT * CTDataLoader::SLICE_WIDTH;
    auto endIterator = startIterator + CTDataLoader::SLICE_HEIGHT * CTDataLoader::SLICE_WIDTH;

    return {{startIterator, endIterator}, CTDataLoader::SLICE_HEIGHT, CTDataLoader::SLICE_WIDTH};
}
