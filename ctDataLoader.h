//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_CTDATALOADER_H
#define CTHEAD_CTDATALOADER_H

#include <vector>

#include "buffers/image.h"


enum CTDataOrientation {CT_ORIENTATION_HEAD_UP, CT_ORIENTATION_FACE_UP};

class CTDataLoader {
    bool dataLoaded = false;
    std::vector<Image> data;

    void normaliseData(std::vector<short>& rawData);
    void convertRawNormalisedDataToImageData(std::vector<short>& rawData);
    void translateDataToFaceUp();

public:
    std::string file;
    unsigned int slices{};
    unsigned int sliceWidth;
    unsigned int sliceHeight;

    CTDataLoader(std::string file, uint sliceWidth, uint sliceHeight);

    void loadData(CTDataOrientation orientation = CT_ORIENTATION_HEAD_UP);
    Image getSlice(unsigned int sliceNum);
};

#endif //CTHEAD_CTDATALOADER_H
