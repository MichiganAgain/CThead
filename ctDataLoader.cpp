//
// Created by thomasgandy on 13/02/2022.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include "ctDataLoader.h"
#include "windows/window.h"


bool CTDataLoader::dataLoaded = false;
std::vector<short> CTDataLoader::data;

void CTDataLoader::loadData() {
    if (CTDataLoader::dataLoaded) return;

    std::ifstream file("/home/thomasgandy/CLionProjects/CThead/resources/CThead", std::ifstream::binary | std::ifstream::ate);
    std::ifstream::pos_type posType = file.tellg();
    CTDataLoader::data.resize(posType);

    file.seekg(0, std::ifstream::beg);
    file.read(reinterpret_cast<char *>(CTDataLoader::data.data()), posType);
    CTDataLoader::normaliseData();

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


Image CTDataLoader::getSlice(unsigned int sliceNum) {
    Image newImage(CTDataLoader::SLICE_HEIGHT, CTDataLoader::SLICE_WIDTH);
    for (int r = 0; r < CTDataLoader::SLICE_HEIGHT; r++) {
        for (int c = 0; c < CTDataLoader::SLICE_WIDTH; c++) {
            long rawDataOffset = sliceNum * CTDataLoader::SLICE_HEIGHT * CTDataLoader::SLICE_WIDTH + r * CTDataLoader::SLICE_WIDTH + c;
            GLubyte ctPixelIntensity = CTDataLoader::data[rawDataOffset];
            newImage.setPixelAt({ctPixelIntensity, ctPixelIntensity, ctPixelIntensity}, r, c);
        }
    }

    return newImage;
}
