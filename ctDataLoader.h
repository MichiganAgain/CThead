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


/**
 * Data loader has all needed attributes and methods to store and handle the source data
 */
class CTDataLoader {
    bool dataLoaded = false;
    std::vector<Image> data;
    unsigned int defaultSliceWidth;
    unsigned int defaultSliceHeight;
    CTDataFormat dataFormat = CT_FORMAT_GREYSCALE;
    static constexpr int RGB_SIZE = 3;

    /**
     * Normalise each data element to a value in the range [0, 255], then return these new normalised values
     * @tparam T The data type each raw data element is interpreted as
     * @param rawData A vector containing the raw data to be normalised
     * @return The normalised data as GLubyte's.  AKA uchar.
     */
    template<typename T>
    std::vector<GLubyte> normaliseData(std::vector<T>& rawData);

    /**
     * Interpret the previously normalised data, and format them into Image instances, then save each new image
     * the the loaders internal vector, so future queries for image slices can be made.
     * @param rawData The raw, normalised slice data that should be converted into image objects
     */
    void convertRawNormalisedDataToImageData(std::vector<GLubyte>& rawData);

public:
    std::string file;
    unsigned int slices{};
    unsigned int sliceWidth;
    unsigned int sliceHeight;
    bool dataModified = false;

    /**
     * Initialise the data loader with the file it should read from, as well as the width and height of each image slice.
     * @param file The file the data loader should read from upon the load data call
     * @param sliceWidth The slice width the loader should interpret each image as being
     * @param sliceHeight The slice height the loader should interpret each image as being
     */
    CTDataLoader(std::string file, uint sliceWidth, uint sliceHeight);

    /**
     * Rotate the image along the imaginary x-axis, so that the 3D image cuboid looks like it
     * is rolling forward away from the user, allowing you to see the underside of the image cuboid.
     */
    void rotateAlongX();

    /**
     * Rotate the image along the imaginary y-axis, so the 3D image looks like it is rotating clockwise
     * if you were looking at it from the top.
     */
    void rotateAlongY();

    /**
     * Rotate the image along the imaginary z-axis, so simply just rotate the current image 90 degrees to the right
     */
    void rotateAlongZ();

    /**
     * Update the file the loader should read data from
     * @tparam T The data type the loader should interpret the raw data as being (e.g. unsigned char, float)
     * @param filename The new filepath the loader should get its data from
     * @param width The new slice width of each image in the file
     * @param height The new slice height of each image in the file
     * @param format The new format (e.g. CT_FORMAT_RGB) the loader should interpret the file as having
     * @return A boolean value depending if the data source update was successful (e.g. could fail if filepath doesn't exist)
     */
    template<typename T>
    bool changeDataSource(const std::string& filename, int width, int height, CTDataFormat format = CT_FORMAT_GREYSCALE);

    /**
     * Actually read
     * @tparam T
     * @param orientation
     */
    template<typename T>
    void loadData(CTDataOrientation orientation = CT_ORIENTATION_HEAD_UP);

    /**
     * Get the image object representing the wanted slice number
     * @param sliceNum The image slice in the internal Image to be fetched
     * @return An image object representing the wanted slice
     */
    Image getSlice(unsigned int sliceNum);

    /**
     * @return The filename of the current data source file
     */
    [[nodiscard]] std::string getFilename() const;

    /**
     * @return The slice width of each image in the current data source file
     */
    [[nodiscard]] unsigned int getSliceWidth() const;

    /**
     * @return The slice height of each image in the current data source file
     */
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
    if (this->dataFormat == CT_FORMAT_RGB) this->slices = rawData.size() / (this->sliceWidth * this->sliceHeight * RGB_SIZE);
    else this->slices = rawData.size() / (this->sliceWidth * this->sliceHeight);

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
    diff = largest - smallest;
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
