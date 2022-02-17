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

void CTDataLoader::loadData(CTDataOrientation orientation) {
    if (CTDataLoader::dataLoaded) return;

    std::ifstream file("/home/thomasgandy/CLionProjects/CThead/resources/CThead", std::ifstream::binary | std::ifstream::ate);
    std::ifstream::pos_type posType = file.tellg();
    CTDataLoader::data.resize(posType);

    file.seekg(0, std::ifstream::beg);
    file.read(reinterpret_cast<char *>(CTDataLoader::data.data()), posType);
    CTDataLoader::normaliseData();

    if (orientation == CT_ORIENTATION_FACE_UP) CTDataLoader::translateDataToFaceUp();

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

void CTDataLoader::translateDataToFaceUp() {
    std::vector<short> translatedData(CTDataLoader::data.size());
    int translatedSlice = 0;
    int translatedRow, translatedCol;

    for (int row = 0; row < CTDataLoader::SLICE_HEIGHT; row++) {
        translatedRow = 0;
        for (int slice = 0; slice < CTDataLoader::SLICES; slice++) {
            translatedCol = 0;
            for (int col = CTDataLoader::SLICE_WIDTH - 1; col >= 0; col--) {
                long existingDataOffset = slice * CTDataLoader::SLICE_WIDTH * CTDataLoader::SLICE_HEIGHT + row * CTDataLoader::SLICE_WIDTH + col;
                long translatedOffset = translatedSlice * SLICE_WIDTH * SLICE_HEIGHT + translatedRow * SLICE_WIDTH + translatedCol;
                translatedCol++;
                translatedData[translatedOffset] = CTDataLoader::data[existingDataOffset];
            }
            translatedRow++;
            translatedCol = 0;
            for (int col = CTDataLoader::SLICE_WIDTH - 1; col >= 0; col--) {
                long existingDataOffset = slice * CTDataLoader::SLICE_WIDTH * CTDataLoader::SLICE_HEIGHT + row * CTDataLoader::SLICE_WIDTH + col;
                long translatedOffset = translatedSlice * SLICE_WIDTH * SLICE_HEIGHT + translatedRow * SLICE_WIDTH + translatedCol;
                translatedCol++;
                translatedData[translatedOffset] = CTDataLoader::data[existingDataOffset];
            }
            translatedRow++;
        }
        translatedSlice++;
    }

    CTDataLoader::data = translatedData;
}

Image CTDataLoader::getSlice(unsigned int sliceNum) {
    if (!CTDataLoader::dataLoaded) throw std::runtime_error("Error retrieving data slice, as data has not yet been loaded");
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
