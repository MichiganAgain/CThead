#include <iostream>
#include <stdexcept>
#include <GLFW/glfw3.h>

#include "main.h"
#include "application.h"


static Application app;

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    app.scrollCallback(window, xOffset, yOffset);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    app.mouseButtonCallback(window, button, action, mods);
}


int main() {
    try {
        app.start();
    } catch (const std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    return 0;
}
