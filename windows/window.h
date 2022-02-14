//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_WINDOW_H
#define CTHEAD_WINDOW_H

#include <string>
#include <vector>
#include <GLFW/glfw3.h>

#include "../ctDataLoader.h"


class Window {
protected:
    Window(std::string t, int w, int h);
    const std::string WINDOW_TITLE;
    const int WINDOW_WIDTH;
    const int WINDOW_HEIGHT;
    GLFWwindow* window = nullptr;
    GLubyte* pixelBuffer = nullptr;

    void blitToPixelBuffer(const Image& image, int pbblx, int pbbly);

public:
    ~Window();

    virtual void initialise() = 0;
    virtual void render() = 0;
    GLFWwindow* getWindow();
    bool windowShouldClose();
};


#endif //CTHEAD_WINDOW_H
