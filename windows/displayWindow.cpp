//
// Created by thomasgandy on 13/02/2022.
//

#include <utility>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <bitset>

#include "displayWindow.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"


void DisplayWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();

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
    this->createImGuiDataSourceGUI();

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
    } else {
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
    this->pixelBufferNeedsUpdating |= this->reset |= ImGui::SmallButton("Reset Axis Rotations");

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
//                                | ImGuiColorEditFlags_PickerHueWheel
                                | ImGuiColorEditFlags_NoInputs
                                | ImGuiColorEditFlags_NoSidePreview;
    this->pixelBufferNeedsUpdating |= ImGui::ColorPicker4("", this->color, flags, this->color);
    this->colorValue = ImGui::ColorConvertFloat4ToU32(ImVec4(this->color[0], this->color[1], this->color[2], this->color[3]));

    ImGui::End();
}

void DisplayWindow::createImGuiDataSourceGUI() {
    ImGui::Begin("Data Source");
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 0;
    style.WindowMinSize = ImVec2(static_cast<float>(this->WINDOW_WIDTH) / 4, 200);
    ImGui::SetWindowFontScale(3);

    ImGui::InputText("File", sourceFile, MAX_SOURCE_FILE_LENGTH);
    ImGui::InputInt("Slice Width", &newSliceWidth);
    ImGui::InputInt("Slice Height", &newSliceHeight);

    if (this->newDataGreyscaleFormat) {
        ImGui::RadioButton("Greyscale", true);
        if (ImGui::RadioButton("RGB", false)) {
            this->newDataGreyscaleFormat = false;
            this->newDataRgbFormat = true;
        }
    } else {
        if (ImGui::RadioButton("Greyscale", false)) {
            this->newDataRgbFormat = false;
            this->newDataGreyscaleFormat = true;
        }
        ImGui::RadioButton("RGB", true);
    }

    this->dataSourceNeedsUpdating |= ImGui::SmallButton("Change File");

    ImGui::End();
}

void DisplayWindow::handleRotations() {
    if (this->rotateX) {
        this->ctDataLoader.rotateAlongX();
        this->rotateX = false;
        this->dataChangedCallback();
    } else if (this->rotateY) {
        this->ctDataLoader.rotateAlongY();
        this->rotateY = false;
        this->dataChangedCallback();
    } else if (this->rotateZ) {
        this->ctDataLoader.rotateAlongZ();
        this->rotateZ = false;
        this->dataChangedCallback();
    } else if (this->reset) {
        this->ctDataLoader.loadData<short>();
        this->reset = false;
        this->dataChangedCallback();
    }
}

void DisplayWindow::handleDataSourceUpdate() {
    if (this->dataSourceNeedsUpdating) {
        CTDataFormat colorFormat = this->newDataGreyscaleFormat ? CT_FORMAT_GREYSCALE: CT_FORMAT_RGB;
        this->ctDataLoader.changeDataSource<short>(this->sourceFile, this->newSliceWidth, this->newSliceHeight, colorFormat);
        this->dataChangedCallback();
        this->pixelBufferNeedsUpdating = true;
        this->dataSourceNeedsUpdating = false;
    }
}

void DisplayWindow::changeDisplaySlice(unsigned int newSliceNum) {
    this->sliceToDraw = newSliceNum;
    this->pixelBufferNeedsUpdating = true;
}

void DisplayWindow::drawImGuiGUI() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void DisplayWindow::dataSourceChanged() {
    this->sliceToDraw = 0;
}

void DisplayWindow::initialise() {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
    glfwSetWindowPos(this->window, 500, 500);
}

void DisplayWindow::render() {
    glfwMakeContextCurrent(this->window);

    this->handleDataSourceUpdate();
    this->createImGuiGUI();
    this->prepareNewFrame();
    this->handleRotations();

    if (this->pixelBufferNeedsUpdating) {
        this->updatePixelBuffer();
        this->pixelBufferNeedsUpdating = false;
    }

    this->drawGeneratedImagePixels();
    this->drawImGuiGUI();

    glfwSwapBuffers(this->window);
}

DisplayWindow::DisplayWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader, void(*dataChangedCallback)()):
        Window(std::move(title), width, height), ctDataLoader{ctDataLoader}, dataChangedCallback{dataChangedCallback} {
    memcpy(this->sourceFile, this->ctDataLoader.getFilename().data(), this->ctDataLoader.getFilename().size());
    this->newSliceWidth = static_cast<int>(this->ctDataLoader.getSliceWidth());
    this->newSliceHeight = static_cast<int>(this->ctDataLoader.getSliceHeight());
}
