//
// Created by thomasgandy on 17/02/2022.
//

#ifndef CTHEAD_BORDER_H
#define CTHEAD_BORDER_H

#include "../buffers/pixelBuffer.h"

struct Border {
    unsigned int width, height;
    unsigned int thickness;
    Color color;

    Border(unsigned int width, unsigned int height, unsigned int thickness, Color color);
};

#endif //CTHEAD_BORDER_H
