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


void DisplayWindow::renderPixelBuffer() {
    std::random_device rng;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            int randomNum = rng() % 113;
            Image image = CTDataLoader::getSlice(static_cast<int>(randomNum));
            this->blitToPixelBuffer(image, c * 256, r * 256);
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

    this->renderPixelBuffer();
    glDrawPixels(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(this->window);
}

DisplayWindow::DisplayWindow(std::string title, int width, int height): Window(std::move(title), width, height) { }
