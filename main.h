//
// Created by thomasgandy on 17/02/2022.
//

#ifndef CTHEAD_MAIN_H
#define CTHEAD_MAIN_H

#include <GLFW/glfw3.h>
#include "application.h"


/**
 * Called when GLFW realises a scroll action is being done over the window requested for the callback
 * @param window The window the callback is listening on
 * @param xOffset The x offset the scroll was in (i.e. to the left or right)
 * @param yOffset The y offset the scroll was in (i.e. up or down)
 */
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

/**
 * Called when GLFW realises mouse buttons action is being done over the window requested for the callback
 * @param window The window the callback is listening on
 * @param button Which button on the mouse the callback was called for
 * @param action Whether the mouse button was being released, pressed or held etc
 * @param mods N/A
 */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

/**
 * Called when GLFW realises a mouse move action is being done over the window requested for the callback
 * @param window The window the callback is listening on
 * @param xPos The mouse x position over the window in the range [0, windowWidth)
 * @param yPos The mouse y position over the window in the range [0, windowHeight)
 */
void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos);

/**
 * Called when the gallery wants to tell the application and other windows that its selected image has changed
 * @param newSliceNum The new slice that was selected in the gallery
 */
void gallerySelectedImageCallback(unsigned int newSliceNum);

/**
 * Called when the display window has modified the data source in some way, and the rest of the application needs
 * to update in order to recognise this change
 */
void dataChangedCallback();


#endif //CTHEAD_MAIN_H
