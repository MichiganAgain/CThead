//
// Created by thomasgandy on 13/02/2022.
//

#include <iostream>
#include <vector>
#include <utility>
#include <random>
#include <GLFW/glfw3.h>

#include "displayWindow.h"
#include "../ctDataLoader.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


void DisplayWindow::updatePixelBuffer() {
    std::random_device rng;
    static int index = 0;
    for (int r = 0; r < 1; r++) {
        for (int c = 0; c < 1; c++) {
            int randomNum = static_cast<int>(rng() % 113);
            Image image = CTDataLoader::getSlice(index);
            image = Image::resize(image, this->WINDOW_WIDTH / 2, this->WINDOW_HEIGHT / 2);
            this->blitToPixelBuffer(image, 500, 500);
            index = (index + 1) % 113;
        }
    }
}

void DisplayWindow::initialise() {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
}

void DisplayWindow::render() {
    glfwMakeContextCurrent(this->window);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Image Manipulation");
    ImGui::SetWindowFontScale(3);
    ImGui::SliderFloat("Scale", &this->scaleValue, 0.5f, 3);
    ImGui::SliderFloat("Gamma", &this->gammaValue, 0.4f, 0.9f);
    ImGui::End();

    ImGui::Render();


    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);


    if (this->windowContentModified) {
        this->updatePixelBuffer();
//        this->windowContentModified = false;
    }
    glDrawPixels(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(this->window);
}

DisplayWindow::DisplayWindow(std::string title, int width, int height): Window(std::move(title), width, height) { }
