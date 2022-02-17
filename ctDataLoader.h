//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_CTDATALOADER_H
#define CTHEAD_CTDATALOADER_H

#include <vector>

#include "buffers/image.h"


enum CTDataOrientation {CT_ORIENTATION_HEAD_UP, CT_ORIENTATION_FACE_UP};

class CTDataLoader {
    static bool dataLoaded;
    static std::vector<short> data;

    static void normaliseData();
    static void translateDataToFaceUp();

public:
    static constexpr int SLICES = 113;
    static constexpr int SLICE_WIDTH = 256;
    static constexpr int SLICE_HEIGHT = 256;

    static void loadData(CTDataOrientation orientation = CT_ORIENTATION_HEAD_UP);
    static Image getSlice(unsigned int sliceNum);
};

#endif //CTHEAD_CTDATALOADER_H
