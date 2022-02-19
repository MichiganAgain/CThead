//
// Created by thomasgandy on 12/02/2022.
//

#ifndef CTHEAD_APPLICATION_H
#define CTHEAD_APPLICATION_H

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "windows/displayWindow.h"
#include "windows/galleryWindow.h"


class Application {
    static bool glfwInitialised;
    const char* glslVersion = "#version 130";

    CTDataLoader ctDataLoader = CTDataLoader("/home/thomasgandy/CLionProjects/CThead/resources/CThead", 256, 256);;
    std::string dataFile = "";


    std::unique_ptr<GalleryWindow> galleryWindow;
    std::unique_ptr<DisplayWindow> displayWindow;

    static void initialiseGLFW();
    void createWindows();
    void initialiseImGui();
    void loadCTData();
    void mainloop();

public:
    void start();
    void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void gallerySelectedImageChange(unsigned int newSliceNum);
    void displayRotatedImageCallback();

    ~Application();
};


#endif //CTHEAD_APPLICATION_H
