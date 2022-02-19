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

    this->createImGuiImageManipulationGUI();
    this->createImGuiImageColorGUI();

    ImGui::Render();
}

void DisplayWindow::createImGuiImageManipulationGUI() {
    ImGui::Begin("Image Scaling");
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowMinSize = ImVec2(static_cast<float>(this->WINDOW_WIDTH) / 4, 200);
    ImGui::SetWindowFontScale(3);

    if (this->nearestNeighbourSelected) {
        ImGui::RadioButton("Nearest Neighbour", true);
        if (ImGui::RadioButton("Bi-Linear", false)) {
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
    ImGui::NewLine();
    this->pixelBufferNeedsUpdating |= this->rotateX |= ImGui::SmallButton("Rotate Around X-Axis");
    this->pixelBufferNeedsUpdating |= this->rotateY |= ImGui::SmallButton("Rotate Around Y-Axis");
    this->pixelBufferNeedsUpdating |= this->rotateZ |= ImGui::SmallButton("Rotate Around Z-Axis");
    this->pixelBufferNeedsUpdating |= this->reset |= ImGui::SmallButton("Reset Axis");

    ImGui::End();
}

void DisplayWindow::createImGuiImageColorGUI() {
    ImGui::Begin("Image Color");
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowMinSize = ImVec2(static_cast<float>(this->WINDOW_WIDTH) / 4, 200);
    ImGui::SetWindowFontScale(3);

    this->pixelBufferNeedsUpdating |= ImGui::SliderFloat("Gamma", &this->gammaValue, this->MIN_GAMMA_VALUE, this->MAX_GAMMA_VALUE);

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_InputRGB
                                | ImGuiColorEditFlags_PickerHueWheel
                                | ImGuiColorEditFlags_NoInputs
                                | ImGuiColorEditFlags_NoSidePreview;
    this->pixelBufferNeedsUpdating |= ImGui::ColorPicker4("", this->color, flags, this->color);
    this->colorValue = ImGui::ColorConvertFloat4ToU32(ImVec4(this->color[0], this->color[1], this->color[2], this->color[3]));

    ImGui::End();
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

    if (this->rotateX) {
        this->ctDataLoader.rotateAlongX();
        this->rotateX = false;
        this->imageRotatedCallback();
    }else if (this->rotateY) {
        this->ctDataLoader.rotateAlongY();
        this->rotateY = false;
        this->imageRotatedCallback();
    }else if (this->rotateZ) {
        this->ctDataLoader.rotateAlongZ();
        this->rotateZ = false;
        this->imageRotatedCallback();
    }else if (this->reset) {
        this->ctDataLoader.loadData<short>();
        this->reset = false;
        this->imageRotatedCallback();
    }

    if (this->pixelBufferNeedsUpdating) {
        this->updatePixelBuffer();
        this->pixelBufferNeedsUpdating = false;
    }

    this->drawGeneratedImagePixels();
    this->drawImGuiGUI();

    glfwSwapBuffers(this->window);
}

DisplayWindow::DisplayWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader, void (*cb)()):
Window(std::move(title), width, height), ctDataLoader{ctDataLoader}, imageRotatedCallback{cb} { }
