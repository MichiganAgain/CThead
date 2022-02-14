//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_CTDATALOADER_H
#define CTHEAD_CTDATALOADER_H

#include <vector>


struct Image {
    std::vector<short> data;
    unsigned int rows, cols;

    Image(unsigned int rows, unsigned int cols);
    Image(std::vector<short> data, unsigned int rows, unsigned int cols);
    [[nodiscard]] short valueAt(int row, int col) const;
    void setValueAt(short newValue, int row, int col);

    static Image resize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);
    static Image nearestNeighbourResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);
    static Image bilinearResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);
};

class CTDataLoader {
    static bool dataLoaded;
    static std::vector<short> data;

    static void normaliseData();

public:
    static constexpr size_t PIXEL_ELEMENT_SIZE = sizeof(short);
    static constexpr int SLICES = 113;
    static constexpr int SLICE_WIDTH = 256;
    static constexpr int SLICE_HEIGHT = 256;

    static void loadData();
    static Image getSlice(int sliceNum);
};

#endif //CTHEAD_CTDATALOADER_H
