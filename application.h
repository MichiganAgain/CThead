//
// Created by thomasgandy on 12/02/2022.
//

#ifndef CTHEAD_APPLICATION_H
#define CTHEAD_APPLICATION_H

#include <vector>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "windows/displayWindow.h"
#include "windows/galleryWindow.h"


class Application {
    const char* glslVersion = "#version 130";
    std::vector<short> rawCTData;
    DisplayWindow displayWindow{"Display", 1000, 1000};
    GalleryWindow galleryWindow{"Gallery", 1000, 1000};

    void initialiseGLFW();
    void createWindows();
    void initialiseImGui();
    void mainloop();

public:
    ~Application();
    void start();
};


#endif //CTHEAD_APPLICATION_H
