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
    static bool glfwInitialised;
    const char* glslVersion = "#version 130";
    DisplayWindow displayWindow{"Display", 2000, 2000};
    GalleryWindow galleryWindow{"Gallery", 1000, 1000};

    static void initialiseGLFW();
    void createWindows();
    void initialiseImGui();
    static void loadCTData();
    void mainloop();

public:
    ~Application();
    void start();
};


#endif //CTHEAD_APPLICATION_H
