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

struct Pixel {
    GLubyte red = 0;
    GLubyte green = 0;
    GLubyte blue = 0;

    Pixel();
    Pixel(GLubyte red, GLubyte green, GLubyte blue);
    explicit Pixel(ImU32 colors);

    Pixel adjustGamma(float gamma);
    bool operator ==(const Pixel& otherPixel) const;

    static Pixel lerp(Pixel p1, Pixel p2, float d);
    friend std::ostream& operator <<(std::ostream& os, const Pixel& p);
};
typedef Pixel Color;


//TODO: Look at changing getPixelAt to return a reference, rather than a newly created instance.
struct PixelBuffer {
    static constexpr int COLOUR_CHANNELS = 3;
    static constexpr GLenum FORMAT = GL_RGB;
    static constexpr GLenum TYPE = GL_UNSIGNED_BYTE;

    std::vector<Pixel> data;
    unsigned int rows, cols;


    PixelBuffer(unsigned int rows, unsigned int cols);
    PixelBuffer(std::vector<Pixel> data, unsigned int rows, unsigned int cols);

    [[nodiscard]] Pixel getPixelAt(unsigned int r, unsigned int c) const;
    void setPixelAt(Pixel colourValues, unsigned int r, unsigned int c);
    void clear(Color c = Color(0, 0, 0));
    [[nodiscard]] bool validIndex(int r, int c) const;

private:
    [[nodiscard]] long calculatePixelOffset(unsigned int r, unsigned int c) const;
};

#endif //CTHEAD_PIXELBUFFER_H
