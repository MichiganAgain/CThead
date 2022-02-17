//
// Created by thomasgandy on 13/02/2022.
//
#include <GLFW/glfw3.h>

#include "galleryWindow.h"


void GalleryWindow::updateInternalGalleryBuffer() {
    this->internalGalleryBuffer.clear();

    for (int i = 0; i < CTDataLoader::SLICES; i++) {
        Image thumbnail = CTDataLoader::getSlice(i);
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
    if (imageCol != this->selectedCol || imageRow != this->selectedRow) {
        this->pixelBufferNeedsUpdating = true;
        this->imageSelectedCallback(imageRow * this->IMAGES_HORIZONTALLY_WITH_GAPS + imageCol);
    }
    this->selectedRow = imageRow;
    this->selectedCol = imageCol;
}

void GalleryWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();

    this->IMAGES_HORIZONTALLY_WITH_GAPS = this->WINDOW_WIDTH / (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE);
    int COMPLETE_WIDTH = this->IMAGES_HORIZONTALLY_WITH_GAPS * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE;
    if (COMPLETE_WIDTH > this->WINDOW_WIDTH) this->IMAGES_HORIZONTALLY_WITH_GAPS -= 1;
    this->minScrollOffset = -(((int)this->internalGalleryBuffer.size() / IMAGES_HORIZONTALLY_WITH_GAPS) * (IMAGE_SIZE + GAP_SIZE));

    int row = 0;
    int col = 0;
    for (int i = 0; i < CTDataLoader::SLICES; i++) {
        int pbblx = col * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE;
        int pbbly = row * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE + this->yScrollOffset;
        this->blitToPixelBuffer(this->internalGalleryBuffer[i], pbblx, pbbly);

        Border b(GalleryWindow::IMAGE_SIZE, GalleryWindow::IMAGE_SIZE, 1, {70, 70, 70});
        this->blitToPixelBuffer(b, pbblx, pbbly);

        col++;
        if (col >= this->IMAGES_HORIZONTALLY_WITH_GAPS) {
            row++;
            col = 0;
        }
    }

    int pbblx = static_cast<int>(this->selectedCol) * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE;
    int pbbly = static_cast<int>(this->selectedRow) * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE + this->yScrollOffset;
    this->blitToPixelBuffer(this->selectedImageBorder, pbblx, pbbly);
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
    if (this->yScrollOffset > this->MAX_SCROLL_OFFSET) this->yScrollOffset = this->MAX_SCROLL_OFFSET;
    if (this->yScrollOffset < this->minScrollOffset) this->yScrollOffset = this->minScrollOffset;

    this->pixelBufferNeedsUpdating = true;
}

void GalleryWindow::mouseButtonCallback(int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (action != GLFW_PRESS) return;

    double mouseX, mouseY;
    glfwGetCursorPos(this->window, &mouseX, &mouseY);
    this->selectGalleryImage(mouseX, mouseY);
}

GalleryWindow::GalleryWindow(std::string title, int width, int height, void (*cb)(unsigned int)): Window(std::move(title), width, height),
    imageSelectedCallback{cb} { }
