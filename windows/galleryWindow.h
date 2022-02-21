//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_GALLERYWINDOW_H
#define CTHEAD_GALLERYWINDOW_H

#define GALLERY_CHANGE_IMAGE_ON_HOVER


#include <vector>

#include "window.h"
#include "../gui/border.h"


/**
 * Represents the window the user will use to select images they want to display in the display window
 */
class GalleryWindow: public Window {
    CTDataLoader& ctDataLoader;

    static constexpr int IMAGE_SIZE = 150;
    static constexpr int GAP_SIZE = 25;
    int IMAGES_HORIZONTALLY_WITH_GAPS = 0;

    std::vector<Image> internalGalleryBuffer;
    bool renderedOnce = false;

    unsigned int selectedIndex = 0;
    Border selectedImageBorder{GalleryWindow::IMAGE_SIZE, GalleryWindow::IMAGE_SIZE, 4, {255, 255, 0}};
    void (*imageSelectedCallback)(unsigned int);

    int yScrollOffset = 0;
    float yScrollSensitivity = 30.f;
    const int ALLOWED_SCROLL_OFFSET = 75;
    int minScrollOffset = 0;


    void updateInternalGalleryBuffer();
    void selectGalleryImage(double mouseX, double mouseY);
    int calculateMinScrollOffset();
    void adjustScrollPosition();
    void updatePixelBuffer() override;

public:
    GalleryWindow() = delete;
    GalleryWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader, void (*cb)(unsigned int newSliceNum) = nullptr);

    void scrollCallback(double, double yOffset);
    void mouseButtonCallback(int button, int action, int mods);
    void dataSourceChanged();
    void initialise() override;
    void render() override;
};

#endif //CTHEAD_GALLERYWINDOW_H
