//
// Created by thomasgandy on 13/02/2022.
//

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <random>
#include <GLFW/glfw3.h>

#include "displayWindow.h"
#include "../ctDataLoader.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


void DisplayWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();

    Image image = CTDataLoader::getSlice(79);
    int newWidth = static_cast<int>(static_cast<float>(CTDataLoader::SLICE_WIDTH) * this->scaleValue);
    int newHeight = static_cast<int>(static_cast<float>(CTDataLoader::SLICE_HEIGHT) * this->scaleValue);
    image = Image::resize(image, newWidth, newHeight);

    int pbblx = static_cast<int>(static_cast<float>(this->WINDOW_WIDTH) / 2) - static_cast<int>(static_cast<float>(image.cols) / 2);
    int pbbly = static_cast<int>(static_cast<float>(this->WINDOW_HEIGHT) / 2) - static_cast<int>(static_cast<float>(image.rows) / 2);
    this->blitToPixelBuffer(image, pbblx, pbbly);
}

void DisplayWindow::initialise() {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
    glfwSetWindowPos(this->window, 750, 500);
}

void DisplayWindow::createImGuiGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Image Manipulation");
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowMinSize = ImVec2(static_cast<float>(this->WINDOW_WIDTH) / 2, 200);

    ImGui::SetWindowFontScale(3);

    ImGui::SliderFloat("Scale", &this->scaleValue, 0.25f, MAX_SCALE_VALUE);
    ImGui::SliderFloat("Gamma", &this->gammaValue, 0.4f, 0.9f);
    ImGui::End();

    ImGui::Render();
}

void DisplayWindow::render() {
    glfwMakeContextCurrent(this->window);
    this->createImGuiGUI();

    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (this->prevScaleValue != this->scaleValue) {
        this->windowContentModified = true;
        this->prevScaleValue = this->scaleValue;
    }
    if (this->windowContentModified) {
        this->updatePixelBuffer();
        this->windowContentModified = false;
    }
    glRasterPos2f(-1,1);
    glPixelZoom( 1, -1 );
    glDrawPixels(this->pixelBuffer.cols, this->pixelBuffer.rows, PixelBuffer::FORMAT, PixelBuffer::TYPE, pixelBuffer.data.data());
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(this->window);
}

DisplayWindow::DisplayWindow(std::string title, int width, int height):
Window(std::move(title), width, height),
MAX_SCALE_VALUE{std::min((float)width / (float)CTDataLoader::SLICE_WIDTH, (float)height / (float)CTDataLoader::SLICE_HEIGHT)}
{ }
