//
// Created by thomasgandy on 15/02/2022.
//

#ifndef CTHEAD_PIXELBUFFER_H
#define CTHEAD_PIXELBUFFER_H

#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

#include "../imgui/imgui.h"


#define IMGUI_COLOR_WHEEL_RED_MASK 0xff
#define IMGUI_COLOR_WHEEL_RED_SHIFTR 0x0
#define IMGUI_COLOR_WHEEL_GREEN_MASK 0xff00
#define IMGUI_COLOR_WHEEL_GREEN_SHIFTR 0x8
#define IMGUI_COLOR_WHEEL_BLUE_MASK 0xff0000
#define IMGUI_COLOR_WHEEL_BLUE_SHIFTR 0x10


/**
 * The fundamental data structure of the program.  It represents a pixel, and the RGB color that the pixel will
 * display on the screen.
 */
struct Pixel {
    GLubyte red = 0;
    GLubyte green = 0;
    GLubyte blue = 0;

    /**
     * Default constructor.  Default initialises rgb to {0, 0, 0} (black).
     */
    Pixel();

    /**
     * Color specific constructor.
     * @param red The red value the pixel should have in a range of [0,256)
     * @param green The green value the pixel should have in a range of [0,256)
     * @param blue The blue value the pixel should have in a range of [0,256)
     */
    Pixel(GLubyte red, GLubyte green, GLubyte blue);

    /**
     * ImGui specified color.  The color value can be given from converting an ImGui Vec4 struct to a ImU32 value.
     * @param colors Color represented as an unsigned 32-bit integer
     */
    explicit Pixel(ImU32 colors);

    /**
     * Adjust the pixel color intensity from a given gamma value, without using a lookup table.
     * @param gamma The new gamma / color intensity value (higher means brighter)
     * @return A new pixel representing the gamma change from the current pixel
     */
    [[nodiscard]] Pixel adjustGamma(float gamma) const;

    /**
     * Adjust the pixel color intensity from a given gamma value, using a lookup table.
     * @param gamma The new gamma / color intensity value (higher means brighter)
     * @param lookup The pre-generated lookup table, mapping pixel color bytes to their new value after applying gamma
     * @return A new pixel representing the gamma change from the current pixel
     */
    Pixel adjustGamma(float gamma, GLubyte lookup[255]) const;

    /**
     * Check if one pixel is equal to another in terms of RGB value.
     * @param otherPixel The other pixel to check against
     * @return A boolean representing if the pixels are equal (true) or not (false)
     */
    bool operator ==(const Pixel& otherPixel) const;

    /**
     * Apply linear interpolation to two pixels.  Given two pixels as input and a distance between them, linearly
     * interpolate between their same reds, then same greens, and same blues.  Send this interpolated color back as a
     * new pixel.
     * @param p1 The first pixel (considered in the interpolation as the left-most pixel)
     * @param p2 The second pixel (considered in the interpolation as the right-most pixel)
     * @param d The distance between the first and second pixel, represented as a float in the range [0,1]
     * @return A new pixel representing the interpolation result of the two input pixels
     */
    static Pixel lerp(Pixel p1, Pixel p2, float d);

    /**
     * A friend function defined outside of the class, that can access the properties of Pixel in order to output
     * the RGB value to stdout.
     * @param os Current output stream the pixel is going to be output to
     * @param p The Pixel to output to stdout
     * @return The updated output stream containing the output contents of the pixel
     */
    friend std::ostream& operator <<(std::ostream& os, const Pixel& p);
};
typedef Pixel Color;


/**
 * Represents a 2D matrix of Pixels.
 */
struct PixelBuffer {
    static constexpr int COLOUR_CHANNELS = 3;
    static constexpr GLenum FORMAT = GL_RGB;
    static constexpr GLenum TYPE = GL_UNSIGNED_BYTE;

    std::vector<Pixel> data;
    unsigned int rows, cols;

    /**
     * Initialise the PixelBuffer with its rows and columns, causing all its containing pixels to be default
     * initialised to black.
     * @param rows The number of rows the Pixel Buffer should have
     * @param cols The number of columns the Pixel Buffer should have
     */
    PixelBuffer(unsigned int rows, unsigned int cols);

    /**
     * Initialise the PixelBuffer with pre-existing pixel data, and the width (columns) and height (rows) of that data
     * @param data Pre-existing Pixel data to used for the 2D matrix
     * @param rows The number of rows this pre-existing data contains
     * @param cols The number of columns this pre-existing data contains
     */
    PixelBuffer(std::vector<Pixel> data, unsigned int rows, unsigned int cols);

    /**
     * Get the Pixel in the PixelBuffer's 2D grid at the respective row and column position
     * @param r The row to find the pixel in
     * @param c The column to find the pixel in
     * @return The found pixel at row r and column c
     */
    [[nodiscard]] Pixel getPixelAt(unsigned int r, unsigned int c) const;

    /**
     * Set the Pixel in the PixelBuffer's 2D grid at the respective row and column position
     * @param colourValues The new color value the pixel at row r and column c should have
     * @param r The row to find the pixel in
     * @param c The column to find the pixel in
     */
    void setPixelAt(Pixel colourValues, unsigned int r, unsigned int c);

    /**
     * Clear the colors of all the Pixels in the PixelBuffer to the specified value; if no value is specified, each
     * pixel is by default set to {0, 0, 0} (black)
     * @param c The optional new color to set each pixel to
     */
    void clear(Color c = Color(0, 0, 0));

    /**
     * Check if a row and column that is trying to be accessed to find a pixel is not out of the bounds for the pixel
     * buffer
     * @param r The row to test
     * @param c The column to test
     * @return A boolean value representing if the requested location in the pixel buffer is a valid one or not
     */
    [[nodiscard]] bool validIndex(int r, int c) const;

private:
    /**
     * Due to the 2D grid being represented as a 1D array under the hood, calculate the 1D offset in this array, based
     * off 2D location coordinates.
     * @param r The row to find the offset for
     * @param c The column to find the offset for
     * @return The 1D offset of the row and column in the array
     */
    [[nodiscard]] long calculatePixelOffset(unsigned int r, unsigned int c) const;
};

#endif //CTHEAD_PIXELBUFFER_H
