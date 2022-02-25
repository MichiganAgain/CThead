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


    /**
     * Update the internal image buffer, so repeating calls to the data loader don't need to be made
     */
    void updateInternalGalleryBuffer();

    /**
     * Select the slice index that is chosen based off the x and y coordinates of the mouse
     * @param mouseX The x coordinate of the mouse in respect to the window
     * @param mouseY The y coordinate of the mouse in respect to the window
     */
    void selectGalleryImage(double mouseX, double mouseY);

    /**
     * @return The offset for how far down the user is allowed to scroll down
     */
    int calculateMinScrollOffset();

    /**
     * Adjust the offset if it is out of the allowed bounds
     */
    void adjustScrollPosition();

    /**
     * Update the pixel buffer with the images and borders
     */
    void updatePixelBuffer() override;

public:
    GalleryWindow() = delete;

    /**
     * Initialise the gallery window with window properties
     * @param title The title of the window
     * @param width The width of the window
     * @param height The height of the window
     * @param ctDataLoader The data source the window should get
     * @param cb The callback that should be called when an image has been selected and the selected slice number should change
     */
    GalleryWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader, void (*cb)(unsigned int newSliceNum) = nullptr);

    /**
     * Called when a scroll operation has happened over the window
     * @param yOffset The direction the scroll was in on the y-axis
     */
    void scrollCallback(double, double yOffset);

    /**
     * Called when a mouse button has been clicked over the window
     * @param button Mouse button that was clicked
     * @param action Whether the button was released / clicked etc
     * @param mods N/A
     */
    void mouseButtonCallback(int button, int action, int mods);

    /**
     * Update the internal gallery buffer when the data source has been changed by an external process
     */
    void dataSourceChanged();

    /**
     * Create the window and set its position
     */
    void initialise() override;

    /**
     * Draw the generated pixels onto the screen, as well as potentially update them if needed (e.g. a scroll)
     */
    void render() override;
};

#endif //CTHEAD_GALLERYWINDOW_H
