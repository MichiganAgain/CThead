//
// Created by thomasgandy on 13/02/2022.
//
#include <GLFW/glfw3.h>

#include "galleryWindow.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


void GalleryWindow::initialise() {
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
    glfwSetWindowPos(this->window, 2000, 500);
}

void GalleryWindow::render() {
    int width, height;
    glfwMakeContextCurrent(this->window);
    glfwGetFramebufferSize(this->window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawPixels(this->pixelBuffer.cols, this->pixelBuffer.rows, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer.data.data());

    glfwSwapBuffers(this->window);
}

GalleryWindow::GalleryWindow(std::string title, int width, int height): Window(std::move(title), width, height) {}