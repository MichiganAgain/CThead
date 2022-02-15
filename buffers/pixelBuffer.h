//
// Created by thomasgandy on 15/02/2022.
//

#ifndef CTHEAD_PIXELBUFFER_H
#define CTHEAD_PIXELBUFFER_H

#include <vector>
#include <GLFW/glfw3.h>


namespace Colour {
    enum {CHANNEL_RED = 0, CHANNEL_GREEN, CHANNEL_BLUE};
}

struct Pixel {
    GLubyte red = 0;
    GLubyte green = 0;
    GLubyte blue = 0;

    Pixel();
    Pixel(GLubyte red, GLubyte green, GLubyte blue);
};

struct PixelBuffer {
    static constexpr int COLOUR_CHANNELS = 3;
    static constexpr GLenum FORMAT = GL_RGB;
    static constexpr GLenum TYPE = GL_UNSIGNED_BYTE;

    std::vector<Pixel> data;
    unsigned int rows, cols;

    PixelBuffer(unsigned int rows, unsigned int cols);
    PixelBuffer(std::vector<Pixel> data, unsigned int rows, unsigned int cols);
    [[nodiscard]] long calculatePixelOffset(unsigned int r, unsigned int c) const;
    [[nodiscard]] Pixel getPixelAt(unsigned int r, unsigned int c) const;
    void setPixelAt(Pixel colourValues, unsigned int r, unsigned int c);
    void clear();
    [[nodiscard]] bool validIndex(int r, int c) const;
};

#endif //CTHEAD_PIXELBUFFER_H
