//
// Created by thomasgandy on 12/02/2022.
//

#ifndef CTHEAD_APPLICATION_H
#define CTHEAD_APPLICATION_H

#include <vector>
#include <memory>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "windows/displayWindow.h"
#include "windows/galleryWindow.h"


/**
 * Class that holds all information relating to the entire application
 */
class Application {
    static bool glfwInitialised;
    const char* glslVersion = "#version 130";

    CTDataLoader ctDataLoader = CTDataLoader("/home/thomasgandy/CLionProjects/CThead/resources/CThead", 256, 256);
    std::string dataFile = "";


    std::unique_ptr<GalleryWindow> galleryWindow;
    std::unique_ptr<DisplayWindow> displayWindow;

    /**
     * Initialise the window library
     */
    static void initialiseGLFW();

    /**
     * Create the respective display and gallery window objects
     */
    void createWindows();

    /**
     * Initialise the GUI library
     */
    void initialiseImGui();

    /**
     * Cause the data object to initialise its data ready for queries
     */
    void loadCTData();

    /**
     * Render loop
     */
    void mainloop();

public:
    /**
     * Entry point to the application that launched the rest of the program
     */
    void start();

    /**
     * Called when a mouse scroll occurs over the gallery window
     * @param window The window the scroll event occurred in (in this case the gallery)
     * @param xOffset The x offset of the scroll
     * @param yOffset The y offset of the scroll
     */
    void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    /**
     * Called when a mouse click has been recognised
     * @param window The window the mouse button event occurred in (in this case the gallery)
     * @param button The button type
     * @param action The action done on the button
     * @param mods N/A
     */
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    /**
     * Called when the gallery has called its callback, stating its selected image has changed
     */
    void gallerySelectedImageChange(unsigned int newSliceNum);

    /**
     * Called when the data source that objects used has changed, so it can either handle the event, or delegate it
     * to objects that need to know
     */
    void dataChangedCallback();

    ~Application();
};


#endif //CTHEAD_APPLICATION_H
