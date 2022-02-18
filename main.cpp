#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "main.h"
#include "application.h"


Application* globalApp = nullptr;

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    globalApp->scrollCallback(window, xOffset, yOffset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    globalApp->mouseButtonCallback(window, button, action, mods);
}

void gallerySelectedImageCallback(unsigned int newSliceNum) {
    globalApp->gallerySelectedImageChange(newSliceNum);
}


int main() {
    Application app;
    ::globalApp = &app;
    try {
        app.start();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}
