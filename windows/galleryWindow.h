//
// Created by thomasgandy on 13/02/2022.
//

#ifndef CTHEAD_GALLERYWINDOW_H
#define CTHEAD_GALLERYWINDOW_H

#include "window.h"

class GalleryWindow: public Window {
    float gammaValue = 0.f;
    float scaleValue = 0.f;

public:
    GalleryWindow() = delete;
    GalleryWindow(std::string title, int width, int height);
    void initialise() override;
    void render() override;
};

#endif //CTHEAD_GALLERYWINDOW_H
