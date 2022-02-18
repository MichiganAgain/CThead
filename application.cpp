//
// Created by thomasgandy on 12/02/2022.
//

#include <stdexcept>

#include "application.h"
#include "ctDataLoader.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "main.h"


bool Application::glfwInitialised = false;

void Application::initialiseGLFW() {
    if (Application::glfwInitialised) return;

    if (!glfwInit()) throw std::runtime_error("Failed to initialise GLFW");
    Application::glfwInitialised = true;
}

void Application::createWindows() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    this->galleryWindow = std::make_unique<GalleryWindow>(GalleryWindow("Gallery", 1100, 1500, this->ctDataLoader, ::gallerySelectedImageCallback));
    this->displayWindow = std::make_unique<DisplayWindow>(DisplayWindow("Display", 1500, 1500, this->ctDataLoader));
    this->galleryWindow->initialise();
    this->displayWindow->initialise();

    glfwMakeContextCurrent(this->displayWindow->getWindow());
    glfwSetScrollCallback(this->galleryWindow->getWindow(), ::scrollCallback);
    glfwSetMouseButtonCallback(this->galleryWindow->getWindow(), ::mouseButtonCallback);
}

void Application::initialiseImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->displayWindow->getWindow(), true);
    ImGui_ImplOpenGL3_Init(this->glslVersion);
}

inline void Application::loadCTData() {
    this->ctDataLoader.loadData<short>(CT_ORIENTATION_HEAD_UP);
}

void Application::mainloop() {
    bool galleryClose = false;
    bool displayClose = false;
    ImGuiIO& io = ImGui::GetIO();

    while (!galleryClose && !displayClose) {
        glfwPollEvents();

        this->galleryWindow->render();
        this->displayWindow->render();

        if (this->galleryWindow->windowShouldClose()) galleryClose = true;
        if (this->displayWindow->windowShouldClose()) displayClose = true;
    }
}

void Application::start() {
    this->loadCTData();
    this->initialiseGLFW();
    this->createWindows();
    this->initialiseImGui();

    this->mainloop();
}

void Application::scrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
    if (window == this->galleryWindow->getWindow()) this->galleryWindow->scrollCallback(xOffset, yOffset);
}

void Application::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (window == this->galleryWindow->getWindow()) this->galleryWindow->mouseButtonCallback(button, action, mods);
}

void Application::gallerySelectedImageChange(unsigned int newSliceNum) {
    this->displayWindow->changeDisplaySlice(newSliceNum);
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}
