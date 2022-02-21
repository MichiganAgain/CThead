//
// Created by thomasgandy on 17/02/2022.
//

#ifndef CTHEAD_MAIN_H
#define CTHEAD_MAIN_H

#include <GLFW/glfw3.h>
#include "application.h"


void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseMoveCallback(GLFWwindow* window, double xPos, double yPos);
void gallerySelectedImageCallback(unsigned int newSliceNum);
void dataChangedCallback();


#endif //CTHEAD_MAIN_H
