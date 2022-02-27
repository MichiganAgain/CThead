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
    /**
     * If the cursor lies in a column gap, simply return.
     * A gap belongs to the LHS of an image
     * If the cursor lies in the gap part of the image and gap (given by GAP_SIZE / IMAGE_SIZE), then it obviously
     * should not select any image, and hence returns.
     */
    double ratioCol = mouseX / (GalleryWindow::IMAGE_SIZE + static_cast<double>(GalleryWindow::GAP_SIZE));
    ratioCol = ratioCol - static_cast<int>(ratioCol);
    if (ratioCol < GAP_SIZE / static_cast<double>(IMAGE_SIZE)) return;

    /**
     * If the cursor lies in a row gap, simply return.
     * The same logic is carried out for the row gap as the colum gap.
     * The further down the user scrolls, the further negative the y scroll offset gets.
     */
    double ratioRow = (mouseY - this->yScrollOffset) / (GalleryWindow::IMAGE_SIZE + static_cast<double>(GalleryWindow::GAP_SIZE));
    ratioRow = ratioRow - static_cast<int>(ratioRow);
    if (ratioRow < GAP_SIZE / static_cast<double>(IMAGE_SIZE)) return;

    /**
     * Now we know the cursor definitely lies in an image, and not a gap.
     */
    int imageCol = static_cast<int>(mouseX) / (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE);
    int imageRow = static_cast<int>(mouseY - this->yScrollOffset) / (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE);
    unsigned int index = imageRow * this->IMAGES_HORIZONTALLY_WITH_GAPS + imageCol;
    if (index < 0 || index >= this->internalGalleryBuffer.size()) index = this->selectedIndex;

    if (index != this->selectedIndex) {
        this->pixelBufferNeedsUpdating = true;
        this->imageSelectedCallback(index);
    }

    this->selectedIndex = index;
}

int GalleryWindow::calculateMinScrollOffset() {
    int verticalImages = std::ceil(static_cast<float>(this->internalGalleryBuffer.size()) / static_cast<float>(IMAGES_HORIZONTALLY_WITH_GAPS));
    int verticalImageAndGapSpace = verticalImages * (IMAGE_SIZE + GAP_SIZE);
    /**
     * The further down the user scrolls, the further negative the scroll offset gets
     * Calculate the final offset by first taking the window height, subtracting the tiny margin of
     * ALLOWED_SCROLL_OFFSET, and then subtracting the amount of vertical space all the images take up.
     */
    int offset = this->WINDOW_HEIGHT - this->ALLOWED_SCROLL_OFFSET - verticalImageAndGapSpace;

    return offset;
}

void GalleryWindow::adjustScrollPosition() {
    if (this->yScrollOffset < this->minScrollOffset) this->yScrollOffset = this->minScrollOffset;
    if (this->yScrollOffset > this->ALLOWED_SCROLL_OFFSET) this->yScrollOffset = this->ALLOWED_SCROLL_OFFSET;
}

void GalleryWindow::updatePixelBuffer() {
    this->pixelBuffer.clear();
    this->minScrollOffset = this->calculateMinScrollOffset();

    int row = 0;
    int col = 0;
    for (int i = 0; i < this->ctDataLoader.slices; i++) {
        /**
         * Calculate the x and y coordinates the image should start at in the window's pixel buffer.
         *
         * For the x coordinate, simply multiply the current column number by the gap size (multiply by the gap
         * size to account for the previously added gap sizes), and then add a gap size onto the result, to actually
         * create the horizontal gap for the image.
         *
         * For the y coordinate, simply do the same, but add on the scroll offset.
         */
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
    this->minScrollOffset = this->calculateMinScrollOffset();
    this->adjustScrollPosition();
    this->selectedIndex = 0;
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
: Window(std::move(title), width, height), ctDataLoader{ctDataLoader}, imageSelectedCallback{cb} {
    /**
     * Calculate how many images will fit horizontally across the window with the specified image and gap size.
     * Firstly, find out how many images with their gaps will fit horizontally (this may be an underestimate at first)
     * Secondly, find out the pixel width of all these images and their gaps, plus the initial gap
     * Finally, if this total length is larger than the window width, subtract one image from the horizontal image count
     */
    this->IMAGES_HORIZONTALLY_WITH_GAPS = this->WINDOW_WIDTH / (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE);
    int COMPLETE_WIDTH = this->IMAGES_HORIZONTALLY_WITH_GAPS * (GalleryWindow::IMAGE_SIZE + GalleryWindow::GAP_SIZE) + GalleryWindow::GAP_SIZE;
    if (COMPLETE_WIDTH > this->WINDOW_WIDTH) this->IMAGES_HORIZONTALLY_WITH_GAPS -= 1;
}
