//
// Created by thomasgandy on 13/02/2022.
//

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <random>
#include <GLFW/glfw3.h>
#include <bitset>

#include "displayWindow.h"
#include "../ctDataLoader.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


void DisplayWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();

    static bool countingUp = true;
    static int index = 0;
    if (countingUp) index++;
    else index--;
    if (index < 0) {
        index = 0;
        countingUp = true;
    } else if (index >= 113) {
        index = 112;
        countingUp = false;
    }

//    Image image = CTDataLoader::getSlice(index);
    Image image = this->ctDataLoader.getSlice(this->sliceToDraw);
    if (this->nearestNeighbourSelected) image = Image::nearestNeighbourResize(image, this->scaleValue, this->scaleValue);
    else if (this->bilinearSelected) image = Image::bilinearResize(image, this->scaleValue, this->scaleValue);
    image.adjustColor(Color(this->colorValue));
    image.adjustGamma(this->gammaValue);

    int pbblx = static_cast<int>(static_cast<float>(this->WINDOW_WIDTH) / 2) - static_cast<int>(static_cast<float>(image.cols) / 2);
    int pbbly = static_cast<int>(static_cast<float>(this->WINDOW_HEIGHT) / 2) - static_cast<int>(static_cast<float>(image.rows) / 2);

    this->blitToPixelBuffer(image, pbblx, pbbly);
}

void DisplayWindow::createImGuiGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Image Manipulation");
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowMinSize = ImVec2(static_cast<float>(this->WINDOW_WIDTH) / 4, 200);
    ImGui::SetWindowFontScale(3);

    if (this->nearestNeighbourSelected) {
        ImGui::RadioButton("Nearest Neighbour Scaling", true);
        if (ImGui::RadioButton("Bi-Linear Scaling", false)) {
            this->nearestNeighbourSelected = false;
            this->bilinearSelected = true;
            this->pixelBufferNeedsUpdating = true;
        }
    }
    else if (this->bilinearSelected) {
        if (ImGui::RadioButton("Nearest Neighbour Scaling", false)) {
            this->bilinearSelected = false;
            this->nearestNeighbourSelected = true;
            this->pixelBufferNeedsUpdating = true;
        }
        ImGui::RadioButton("Bi-Linear Scaling", true);
    }

    this->pixelBufferNeedsUpdating |= ImGui::SliderInt("Scale", &this->scaleValue, this->MIN_SCALE_VALUE, this->MAX_SCALE_VALUE);
    this->pixelBufferNeedsUpdating |= ImGui::SliderFloat("Gamma", &this->gammaValue, this->MIN_GAMMA_VALUE, this->MAX_GAMMA_VALUE);

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueWheel;
    this->pixelBufferNeedsUpdating |= ImGui::ColorPicker4("Color Picker", this->color, flags, this->color);
    this->colorValue = ImGui::ColorConvertFloat4ToU32(ImVec4(this->color[0], this->color[1], this->color[2], this->color[3]));

    ImGui::End();

    ImGui::Render();
}

void DisplayWindow::changeDisplaySlice(unsigned int newSliceNUm) {
    this->sliceToDraw = newSliceNUm;
    this->pixelBufferNeedsUpdating = true;
}

void DisplayWindow::drawImGuiGUI() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DisplayWindow::initialise() {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
    glfwSetWindowPos(this->window, 500, 500);
}

void DisplayWindow::render() {
    glfwMakeContextCurrent(this->window);

    this->createImGuiGUI();
    this->prepareNewFrame();
    if (this->pixelBufferNeedsUpdating) {
        this->updatePixelBuffer();
        this->pixelBufferNeedsUpdating = false;
    }
    this->drawGeneratedImagePixels();
    this->drawImGuiGUI();

    glfwSwapBuffers(this->window);
}

DisplayWindow::DisplayWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader):
Window(std::move(title), width, height), ctDataLoader{ctDataLoader}
//MAX_SCALE_VALUE{std::min((float)width / (float)CTDataLoader::SLICE_WIDTH, (float)height / (float)CTDataLoader::SLICE_HEIGHT)}
{ }
