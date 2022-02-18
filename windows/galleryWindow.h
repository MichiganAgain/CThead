//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_GALLERYWINDOW_H
#define CTHEAD_GALLERYWINDOW_H

#include <vector>

#include "window.h"
#include "../gui/border.h"

class GalleryWindow: public Window {
    CTDataLoader& ctDataLoader;

    static constexpr int IMAGE_SIZE = 150;
    static constexpr int GAP_SIZE = 25;
    int IMAGES_HORIZONTALLY_WITH_GAPS = 0;

    std::vector<Image> internalGalleryBuffer;
    bool renderedOnce = false;

    unsigned int selectedRow = 0;
    unsigned int selectedCol = 0;
    Border selectedImageBorder{GalleryWindow::IMAGE_SIZE, GalleryWindow::IMAGE_SIZE, 4, {255, 255, 0}};
    void (*imageSelectedCallback)(unsigned int);

    int yScrollOffset = 0;
    float yScrollSensitivity = 30.f;
    const int MAX_SCROLL_OFFSET = 50;
    int minScrollOffset = 0;

    void updateInternalGalleryBuffer();
    void selectGalleryImage(double mouseX, double mouseY);
    void updatePixelBuffer() override;

public:
    GalleryWindow() = delete;
    GalleryWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader, void (*cb)(unsigned int newSliceNum) = nullptr);
    void initialise() override;
    void render() override;
    void scrollCallback(double, double yOffset);
    void mouseButtonCallback(int button, int action, int mods);
};

#endif //CTHEAD_GALLERYWINDOW_H
