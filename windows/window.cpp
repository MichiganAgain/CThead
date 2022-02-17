//
// Created by thomasgandy on 13/02/2022.
//

#include <stdexcept>
#include <utility>
#include <GLFW/glfw3.h>

#include "window.h"



void Window::blitToPixelBuffer(const Image& image, int pbblx, int pbbly) {
    for (int dataRow = 0; dataRow < image.rows; dataRow++) {
        for (int dataCol = 0; dataCol < image.cols; dataCol++) {
            int pixelBufferRow = dataRow + pbbly;
            int pixelBufferCol = dataCol + pbblx;

            if (this->pixelBuffer.validIndex(pixelBufferRow, pixelBufferCol)) {
                this->pixelBuffer.setPixelAt(image.getPixelAt(dataRow, dataCol), pixelBufferRow, pixelBufferCol);
            }
        }
    }
}

void Window::prepareNewFrame() {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::drawGeneratedImagePixels() {
    glRasterPos2f(-1,1);
    glPixelZoom( 1, -1 );
    glDrawPixels(static_cast<int>(this->pixelBuffer.cols), static_cast<int>(this->pixelBuffer.rows), PixelBuffer::FORMAT, PixelBuffer::TYPE, pixelBuffer.data.data());
}

bool Window::pixelBufferNeedsUpdating() {
    return false;
}

Window::Window(std::string t, int w, int h): WINDOW_TITLE(std::move(t)), WINDOW_WIDTH(w), WINDOW_HEIGHT(h), pixelBuffer(h, w) { }

Window::~Window() {
    glfwDestroyWindow(this->window);
}

GLFWwindow* Window::getWindow() {
    return this->window;
}

bool Window::windowShouldClose() {
    return glfwWindowShouldClose(this->window);
}
