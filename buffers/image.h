//
// Created by thomasgandy on 15/02/2022.
//

#ifndef CTHEAD_IMAGE_H
#define CTHEAD_IMAGE_H

#include "pixelBuffer.h"
#include <vector>


struct ImageResizeInfo;

/**
 * Represents an image that can be drawn onto the screen.  Due to an image having an underlying grid of pixels, it
 * naturally inherits from PixelBuffer to inherit these properties.
 */
struct Image: public PixelBuffer {
    // The maximum amount of threads that can be used when performing concurrent image operations
    static constexpr int MAX_THREADS = 1;

    /**
     * Initialise the image's internal pixel grid to have a width and height specified.  All pixels will be initialised
     * by default to their default color value.
     * @param rows The number of pixel rows the image should have
     * @param cols The number of pixel columns the image should have
     */
    Image(unsigned int rows, unsigned int cols);

    /**
     * Initialise the image to have the grid given.  The width and height of the passed grid must be specified, due
     * to the passed grid being 1D.
     * @param data The grid containing pixel location and information for the image
     * @param rows How many rows the given grid has
     * @param cols How many columns the given grid has
     */
    Image(std::vector<Pixel> data, unsigned int rows, unsigned int cols);

    /**
     * Adjust the color of each pixel in the image
     * @param c The color to adjust each pixel by
     */
    void adjustColor(Color c);

    /**
     * Adjust the gamma of each pixel in the image
     * @param gamma The new gamma value for each pixel
     */
    void adjustGamma(float gamma);

    /**
     * Resize an image using the nearest neighbour algorithm
     * @param oldImage The image you want to resize
     * @param newWidth The width of the new image you want to create
     * @param newHeight The height of the new image you want to create
     * @return A new image with the content of oldImage, but resized to newWidth and newHeight
     */
    static Image nearestNeighbourResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);

    /**
     * Resize an image using the bi-linear algorithm
     * @param oldImage The image you want to resize
     * @param newWidth The width of the new image you want to create
     * @param newHeight The height of the new image you want to create
     * @return A new image with the content of oldImage, but resized to newWidth and newHeight
     */
    static Image bilinearResize(const Image& oldImage, unsigned int newWidth, unsigned int newHeight);

private:
    /**
     * Fill the given lookup table with data, mapping each value from [0,256) to its adjusted gamma value.  This
     * adjusted gamma value will also be in the range from [0,256)
     * @param gamma The gamma value you want to apply to each value in the range [0,256)
     * @param lookup The lookup table that is to be filled
     */
    static void fillLookupTable(float gamma, GLubyte lookup[256]);

    /**
     * Method used by threads to resize an image using bi-linear interpolation
     * @param threadInfo The information needed by the thread to isolate its work from other threads
     */
    static void bilinearResizeWorker(ImageResizeInfo threadInfo);
};

/**
 * Data structure used by threads when carrying out image manipulation, in order to make sure the work they are
 * doing is isolated from other threads.
 */
struct ImageResizeInfo {
    const Image& oldImage;
    Image& newImage;
    uint workerStartRow{}, workerEndRow{};

    /**
     * Constructor used to set the images the thread should perform operations on
     * @param oldImage The old image the thread may use to calculate new pixel data for the new image from
     * @param newImage The new image the thread should write new pixel data to
     */
    ImageResizeInfo(const Image& oldImage, Image& newImage);
};

#endif //CTHEAD_IMAGE_H
