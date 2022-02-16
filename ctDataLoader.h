//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_CTDATALOADER_H
#define CTHEAD_CTDATALOADER_H

#include <vector>

#include "buffers/image.h"


class CTDataLoader {
    static bool dataLoaded;
    static std::vector<short> data;

    static void normaliseData();

public:
    static constexpr int SLICES = 113;
    static constexpr int SLICE_WIDTH = 256;
    static constexpr int SLICE_HEIGHT = 256;

    static void loadData();
    static Image getSlice(unsigned int sliceNum);
};

#endif //CTHEAD_CTDATALOADER_H
