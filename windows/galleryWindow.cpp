//
// Created by thomasgandy on 13/02/2022.
//
#include <GLFW/glfw3.h>

#include "galleryWindow.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


// w = imgWidth * imageCount + gapWidth * (imageCount + 1)
void GalleryWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();
    constexpr int IMAGE_SIZE = 100;
    constexpr int GAP_SIZE = 10;


    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            Image thumbnail = CTDataLoader::getSlice(r * 10 + c);
            thumbnail = Image::resize(thumbnail, IMAGE_SIZE, IMAGE_SIZE);
            this->blitToPixelBuffer(thumbnail, c * IMAGE_SIZE, r * IMAGE_SIZE);
        }
    }
}

void GalleryWindow::initialise() {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
    glfwSetWindowPos(this->window, 2500, 500);
}

void GalleryWindow::render() {
    glfwMakeContextCurrent(this->window);

    this->prepareNewFrame();
//    if (this->pixelBufferNeedsUpdating()) this->updatePixelBuffer();
    if (!this->renderedOnce) {
        this->updatePixelBuffer();
        this->renderedOnce = true;
    }
    this->drawGeneratedImagePixels();

    glfwSwapBuffers(this->window);
}

GalleryWindow::GalleryWindow(std::string title, int width, int height): Window(std::move(title), width, height) {}