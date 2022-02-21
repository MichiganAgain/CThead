//
// Created by thomasgandy on 17/02/2022.
//

#ifndef CTHEAD_BORDER_H
#define CTHEAD_BORDER_H

#include "../buffers/pixelBuffer.h"

/**
 * Represents a rectangular border
 */
struct Border {
    unsigned int width, height;
    unsigned int thickness;
    Color color;

    /**
     * Initialise the width, height, thickness and color of the border
     * @param width How wide the border should be from the left outside column, to the right outside column
     * @param height How high the border should be from the top outside row, to the bottom outside row
     * @param thickness How thick the border should be around the edges
     * @param color The border color
     */
    Border(unsigned int width, unsigned int height, unsigned int thickness, Color color);
};

#endif //CTHEAD_BORDER_H
