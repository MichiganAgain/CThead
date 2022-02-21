#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "main.h"
#include "application.h"


Application* globalApp = nullptr;

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    globalApp->scrollCallback(window, xOffset, yOffset);
}

#ifdef GALLERY_CHANGE_IMAGE_ON_HOVER
void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos) {
    globalApp->mouseButtonCallback(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
}
#else
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    globalApp->mouseButtonCallback(window, button, action, mods);
}
#endif

void gallerySelectedImageCallback(unsigned int newSliceNum) {
    globalApp->gallerySelectedImageChange(newSliceNum);
}

void dataChangedCallback() {
    globalApp->dataChangedCallback();
}


int main() {
    ::globalApp = new Application;
    try {
        ::globalApp->start();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }
    delete ::globalApp;

    return 0;
}
