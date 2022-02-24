//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_WINDOW_H
#define CTHEAD_WINDOW_H

#include <string>
#include <vector>
#include <GLFW/glfw3.h>

#include "../ctDataLoader.h"
#include "../buffers/pixelBuffer.h"
#include "../gui/border.h"


/**
 * Represents a general window that has common properties between them
 */
class Window {
protected:
    Window(std::string t, int w, int h);
    const std::string WINDOW_TITLE;
    const int WINDOW_WIDTH;
    const int WINDOW_HEIGHT;
    GLFWwindow* window = nullptr;
    PixelBuffer pixelBuffer;
    bool pixelBufferNeedsUpdating = true;

    /**
     * Copy the data from an Image onto the underlying PixelBuffer that will be displayed on the window
     * @param image The image to copy onto the screen pixels
     * @param pbblx The left x coordinate of the image
     * @param pbbly The top y coordinate of the image
     */
    void blitToPixelBuffer(const Image& image, int pbblx, int pbbly);

    /**
     * Copy the data from a Border onto the underlying PixelBuffer, so it will be displayed on the window
     * @param border The border to write to the screen pixels
     * @param pbblx The left-most outer x pixel of the border
     * @param pbbly The top-most outer y pixel of the border
     */
    void blitToPixelBuffer(const Border& border, int pbblx, int pbbly);

    /**
     * Clear the pixel buffer and initialise any GUI elements needed before re-drawing
     */
    virtual void prepareNewFrame();

    /**
     * Actually write the pixels to the physical screen
     */
    virtual void drawGeneratedImagePixels();

    /**
     * Write pixel information to the underlying buffer that will eventually be drawn to the physical screen
     */
    virtual void updatePixelBuffer() = 0;

public:
    Window() = delete;
    ~Window();

    /**
     * Create any windows and pre-render initialisation
     */
    virtual void initialise() = 0;

    /**
     * Render any GUIs to the screen, as well as potentially updating the underlying pixel buffer, before
     * the drawGeneratedImagePixels function is called
     */
    virtual void render() = 0;

    /**
     * @return the window pointer respective to the window object
     */
    GLFWwindow* getWindow();

    /**
     * @return true if a terminate signal has been sent to the window
     */
    bool windowShouldClose();
};


#endif //CTHEAD_WINDOW_H
