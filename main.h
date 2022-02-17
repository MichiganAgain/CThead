//
// Created by thomasgandy on 17/02/2022.
//

#ifndef CTHEAD_MAIN_H
#define CTHEAD_MAIN_H

#include <GLFW/glfw3.h>

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void gallerySelectedImageCallback(unsigned int newSliceNum);

#endif //CTHEAD_MAIN_H
