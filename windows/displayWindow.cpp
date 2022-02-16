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


bool DisplayWindow::pixelBufferNeedsUpdating() {
    if (this->prevScaleValue != this->scaleValue) {
        this->prevScaleValue = this->scaleValue;
        return true;
    }
    if (this->prevGammaValue != this->gammaValue) {
        this->prevGammaValue = this->gammaValue;
        return true;
    }
    if (this->prevColorValue != this->colorValue) {
        this->prevColorValue = this->colorValue;
        return true;
    }
    if (this->previousNearestSelected != this->nearestNeighbourSelected) {
        this->previousNearestSelected = this->nearestNeighbourSelected;
        return true;
    }
    if (this->previousBilinear != this->bilinearSelected) {
        this->previousBilinear = this->bilinearSelected;
        return true;
    }

    return false;
}

void DisplayWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();

//    static bool countingUp = true;
//    static int index = 0;
//    if (countingUp) index++;
//    else index--;
//    if (index < 0) {
//        index = 0;
//        countingUp = true;
//    } else if (index >= 113) {
//        index = 112;
//        countingUp = false;
//    }

//    Image image = CTDataLoader::getSlice(index);
    Image image = CTDataLoader::getSlice(this->ctSliceToDraw);
    if (this->nearestNeighbourSelected) image = Image::nearestNeighbourResize(image, this->scaleValue, this->scaleValue);
    else if (this->bilinearSelected) image = Image::bilinearResize(image, this->scaleValue, this->scaleValue);
    image.adjustColor(Color(this->colorValue));

    int pbblx = static_cast<int>(static_cast<float>(this->WINDOW_WIDTH) / 2) - static_cast<int>(static_cast<float>(image.cols) / 2);
    int pbbly = static_cast<int>(static_cast<float>(this->WINDOW_HEIGHT) / 2) - static_cast<int>(static_cast<float>(image.rows) / 2);
    this->blitToPixelBuffer(image, 0, pbbly);
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
        }
    }
    else if (this->bilinearSelected) {
        if (ImGui::RadioButton("Nearest Neighbour Scaling", false)) {
            this->bilinearSelected = false;
            this->nearestNeighbourSelected = true;
        }
        ImGui::RadioButton("Bi-Linear Scaling", true);
    }

    ImGui::SliderInt("Scale", &this->scaleValue, this->MIN_SCALE_VALUE, this->MAX_SCALE_VALUE);
    ImGui::SliderFloat("Gamma", &this->gammaValue, this->MIN_GAMMA_VALUE, this->MAX_GAMMA_VALUE);

    ImGuiColorEditFlags flags = ImGuiColorEditFlags_InputRGB;
    ImGui::ColorPicker4("Color Picker", this->color, flags, this->color);
    this->colorValue = ImGui::ColorConvertFloat4ToU32(ImVec4(this->color[0], this->color[1], this->color[2], this->color[3]));

    ImGui::End();

    ImGui::Render();
}

void DisplayWindow::initialise() {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
    glfwSetWindowPos(this->window, 250, 400);
}

void DisplayWindow::prepareNewFrame() {
    int width, height;
    glfwGetFramebufferSize(this->window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void DisplayWindow::drawGeneratedImagePixels() {
    glRasterPos2f(-1,1);
    glPixelZoom( 1, -1 );
    glDrawPixels(static_cast<int>(this->pixelBuffer.cols), static_cast<int>(this->pixelBuffer.rows), PixelBuffer::FORMAT, PixelBuffer::TYPE, pixelBuffer.data.data());
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DisplayWindow::render() {
    glfwMakeContextCurrent(this->window);

    this->createImGuiGUI();
    this->prepareNewFrame();
    if (this->pixelBufferNeedsUpdating()) this->updatePixelBuffer();
    this->drawGeneratedImagePixels();

    glfwSwapBuffers(this->window);
}

DisplayWindow::DisplayWindow(std::string title, int width, int height):
Window(std::move(title), width, height)
//MAX_SCALE_VALUE{std::min((float)width / (float)CTDataLoader::SLICE_WIDTH, (float)height / (float)CTDataLoader::SLICE_HEIGHT)}
{ }
