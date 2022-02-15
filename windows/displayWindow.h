//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_DISPLAYWINDOW_H
#define CTHEAD_DISPLAYWINDOW_H

#include <vector>
#include <GLFW/glfw3.h>

#include "window.h"
#include "../ctDataLoader.h"

class DisplayWindow: public Window {
    float gammaValue = 1.f;

    const float MAX_SCALE_VALUE;
    float scaleValue = 1.f;
    float prevScaleValue = scaleValue;



    void updatePixelBuffer();
    void createImGuiGUI();

public:
    DisplayWindow() = delete;
    DisplayWindow(std::string title, int width, int height);
    void initialise() override;
    void render() override;
};

#endif //CTHEAD_DISPLAYWINDOW_H
