//
// Created by thomasgandy on 13/02/2022.
//
#include <GLFW/glfw3.h>
#include <cmath>
#include <algorithm>

#include "galleryWindow.h"


void GalleryWindow::updateInternalGalleryBuffer() {
    this->internalGalleryBuffer.clear();

    for (int i = 0; i < this->ctDataLoader.slices; i++) {
        Image thumbnail = this->ctDataLoader.getSlice(i);
        thumbnail = Image::nearestNeighbourResize(thumbnail, GalleryWindow::IMAGE_SIZE, GalleryWindow::IMAGE_SIZE);
        this->internalGalleryBuffer.push_back(thumbnail);
    }
}

void GalleryWindow::selectGalleryImage(double mouseX, double mouseY) {
    double ratioCol1 = mouseX / (GalleryWindow::IMAGE_SIZE + (double)GalleryWindow::GAP_SIZE);
    double ratioCol2 = ratioCol1 - static_cast<int>(ratioCol1);
    if (ratioCol2 < GAP_SIZE / (double)IMAGE_SIZE) return;

    double ratioRow1 = (mouseY - this->yScrollOffset) / (GalleryWindow::IMAGE_SIZE + (double)GalleryWindow::GAP_SIZE);
    double ratioRow2 = ratioRow1 - static_cast<int>(ratioRow1);
    if (ratioRow2 < GAP_SIZE / (double)IMAGE_SIZE) return;

    int imageCol = static_cast<int>(mouseX) / (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE);
    int imageRow = static_cast<int>(mouseY - this->yScrollOffset) / (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE);
    unsigned int index = imageRow * this->IMAGES_HORIZONTALLY_WITH_GAPS + imageCol;

    if (index != this->selectedIndex) {
        this->pixelBufferNeedsUpdating = true;
        this->imageSelectedCallback(index);
    }

    this->selectedIndex = index;
}

int GalleryWindow::calculateMinScrollOffset() {
    this->IMAGES_HORIZONTALLY_WITH_GAPS = this->WINDOW_WIDTH / (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE);
    int COMPLETE_WIDTH = this->IMAGES_HORIZONTALLY_WITH_GAPS * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE;
    if (COMPLETE_WIDTH > this->WINDOW_WIDTH) this->IMAGES_HORIZONTALLY_WITH_GAPS -= 1;
    int verticalImages = std::ceil(static_cast<float>(this->internalGalleryBuffer.size()) / (float)IMAGES_HORIZONTALLY_WITH_GAPS);
    int verticalImageAndGapSpace = verticalImages * (IMAGE_SIZE + GAP_SIZE);
    int offset = this->WINDOW_HEIGHT - this->ALLOWED_SCROLL_OFFSET - verticalImageAndGapSpace;

    return offset;
}

void GalleryWindow::adjustScrollPosition() {
    if (this->yScrollOffset > this->ALLOWED_SCROLL_OFFSET) this->yScrollOffset = this->ALLOWED_SCROLL_OFFSET;
    if (this->yScrollOffset < this->minScrollOffset) this->yScrollOffset = this->minScrollOffset;
}

void GalleryWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();
    this->minScrollOffset = this->calculateMinScrollOffset();

    int row = 0;
    int col = 0;
    for (int i = 0; i < this->ctDataLoader.slices; i++) {
        int pbblx = col * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE;
        int pbbly = row * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE + this->yScrollOffset;
        this->blitToPixelBuffer(this->internalGalleryBuffer[i], pbblx, pbbly);

        if (i == this->selectedIndex) this->blitToPixelBuffer(this->selectedImageBorder, pbblx, pbbly);
        else {
            Border b(GalleryWindow::IMAGE_SIZE, GalleryWindow::IMAGE_SIZE, 1, {70, 70, 70});
            this->blitToPixelBuffer(b, pbblx, pbbly);
        }

        col++;
        if (col >= this->IMAGES_HORIZONTALLY_WITH_GAPS) {
            row++;
            col = 0;
        }
    }
}

void GalleryWindow::dataSourceChanged() {
    this->updateInternalGalleryBuffer();
    if (this->selectedIndex >= this->internalGalleryBuffer.size())
        this->selectedIndex = std::min(this->selectedIndex / 2, (unsigned int) this->internalGalleryBuffer.size() - 1);
    
    this->adjustScrollPosition();
    this->pixelBufferNeedsUpdating = true;
}

void GalleryWindow::initialise() {
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, this->WINDOW_TITLE.c_str(), nullptr, nullptr);
    glfwSetWindowPos(this->window, 2100, 500);
    this->updateInternalGalleryBuffer();
}

void GalleryWindow::render() {
    glfwMakeContextCurrent(this->window);
    glfwPollEvents();

    this->prepareNewFrame();

    if (this->pixelBufferNeedsUpdating) {
        this->updatePixelBuffer();
        this->pixelBufferNeedsUpdating = false;
    }

    this->drawGeneratedImagePixels();

    glfwSwapBuffers(this->window);
}

void GalleryWindow::scrollCallback(double, double yOffset) {
    this->yScrollOffset += static_cast<int>(yOffset * this->yScrollSensitivity);
    this->adjustScrollPosition();
    this->pixelBufferNeedsUpdating = true;
}

void GalleryWindow::mouseButtonCallback(int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (action != GLFW_PRESS) return;

    double mouseX, mouseY;
    glfwGetCursorPos(this->window, &mouseX, &mouseY);
    this->selectGalleryImage(mouseX, mouseY);
}

GalleryWindow::GalleryWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader, void (*cb)(unsigned int))
: Window(std::move(title), width, height), ctDataLoader{ctDataLoader}, imageSelectedCallback{cb} { }
