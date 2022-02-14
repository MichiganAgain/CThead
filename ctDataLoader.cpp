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


Image::Image(std::vector<short> data, unsigned int rows, unsigned int cols): data{std::move(data)}, rows{rows}, cols{cols} { }

short Image::valueAt(int row, int col) {
    if (row < 0 || row >= this->rows || col < 0 || col >= this->cols)
        throw std::runtime_error("Out of bounds for image's valueAt function");
    return this->data[row * this->cols + col];
}

Image Image::resize(const Image &oldImage) {
    return {{}, 0, 0};
}

Image Image::nearestNeighbourResize(const Image &oldImage) {
    return {{}, 0, 0};
}

Image Image::bilinearResize(const Image &oldImage) {
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
