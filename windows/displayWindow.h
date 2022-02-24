//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_DISPLAYWINDOW_H
#define CTHEAD_DISPLAYWINDOW_H

#include <vector>
#include <GLFW/glfw3.h>

#include "window.h"
#include "../ctDataLoader.h"


/**
 * Represents the window shown to the user that will display the currently selected image in the gallery.  It will
 * display image operations the user can perform on the image they are displaying, and will ensure those image
 * operations are carried out on the desired image.
 */
class DisplayWindow: public Window {
    CTDataLoader& ctDataLoader;

    const float MIN_GAMMA_VALUE = 0.1;
    const float MAX_GAMMA_VALUE = 4;
    float gammaValue = 1.f;

    bool nearestNeighbourSelected = true;
    bool bilinearSelected = false;

    const int MIN_SCALE_VALUE = 32;
    const int MAX_SCALE_VALUE = 1024;
    int scaleValue = 256;

    bool rotateX = false;
    bool rotateY = false;
    bool rotateZ = false;
    bool reset = false;

    float color[4] = {255, 255, 255, 255};
    ImU32 colorValue = ImGui::ColorConvertFloat4ToU32(ImVec4(255, 0, 0, 255));

    static const int MAX_SOURCE_FILE_LENGTH = 512;
    char sourceFile[MAX_SOURCE_FILE_LENGTH] = {0};
    int newSliceWidth = 0;
    int newSliceHeight = 0;
    bool newDataGreyscaleFormat = true;
    bool newDataRgbFormat = false;
    bool dataSourceNeedsUpdating = false;

    unsigned int sliceToDraw = 0;
    void (*dataChangedCallback)();

    /**
     * Updates the window contents with the image that is selected, having performed any image operations
     */
    void updatePixelBuffer() override;

    /**
     * Tell ImGUI to start setting up the GUI layout
     */
    void createImGuiGUI();

    /**
     * Tell ImGUI how to create the image manipulation GUI
     */
    void createImGuiImageManipulationGUI();

    /**
     * Tell ImGUI how to create the image color GUI
     */
    void createImGuiImageColorGUI();

    void createImGuiDataSourceGUI();

    /**
     * Cause ImGUI to actually render its now made GUI data to an image in the swap-chain
     */
    static void drawImGuiGUI();

public:
    DisplayWindow() = delete;

    /**
     * Initialise the DisplayWindow attributes with the information it needs to create a window, load an image, and
     * a callback to tell the application when the gallery needs to update its grid
     * @param title The title of the display window
     * @param width The width of the display window
     * @param height The height of the display window
     * @param ctDataLoader The image data source the display window will retrieve the image slice specified from the gallery
     * @param imageRotatedCallback The callback the display window will call when it wants to change the underlying data from the data source
     */
    DisplayWindow(std::string title, int width, int height, CTDataLoader& ctDataLoader, void (*dataChangedCallback)());

    /**
     * Change the image slice the display window is to show
     * @param newSliceNum The new slice index in the data source the display window is to show
     */
    void changeDisplaySlice(unsigned int newSliceNum);

    /**
     * Used to notify the display window the data source from which it gets its image has changed
     */
    void dataSourceChanged();
    
    /**
     * Perform any pre-render initialisation operations the display needs before rendering, such as actually creating its window
     */
    void initialise() override;

    /**
     * Cause the created pixel data and GUI to be rendered to the window managed by the disaply window
     */
    void render() override;
};


#endif //CTHEAD_DISPLAYWINDOW_H
