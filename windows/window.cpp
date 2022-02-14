//
// Created by thomasgandy on 13/02/2022.
//

#include <GLFW/glfw3.h>

#include <utility>
#include "window.h"


void Window::blitToPixelBuffer(const Image& image, int pbblx, int pbbly) {
    for (int dataRow = 0; dataRow < image.rows; dataRow++) {
        for (int dataCol = 0; dataCol < image.cols; dataCol++) {
            long dataOffset = dataRow * image.cols + dataCol;
            long pixelBufferOffset = (pbbly + dataRow) * this->WINDOW_WIDTH * 3 + (pbblx + dataCol) * 3;
            this->pixelBuffer[pixelBufferOffset + 0] = image.data[dataOffset];
            this->pixelBuffer[pixelBufferOffset + 1] = image.data[dataOffset];
            this->pixelBuffer[pixelBufferOffset + 2] = image.data[dataOffset];
        }
    }
}

Window::Window(std::string t, int w, int h): WINDOW_TITLE(std::move(t)), WINDOW_WIDTH(w), WINDOW_HEIGHT(h) {
    this->pixelBuffer = new GLubyte[this->WINDOW_WIDTH * this->WINDOW_HEIGHT * 3];
}

Window::~Window() {
    delete this->pixelBuffer;
    glfwDestroyWindow(this->window);
}

GLFWwindow* Window::getWindow() {
    return this->window;
}

bool Window::windowShouldClose() {
    return glfwWindowShouldClose(this->window);
}
