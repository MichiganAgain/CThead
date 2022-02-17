cmake_minimum_required(VERSION 3.21)
project(CThead)

set(CMAKE_CXX_STANDARD 17)

file(GLOB IMGUI_SOURCE "imgui/*.cpp")
add_executable(CThead main.cpp application.cpp application.h ${IMGUI_SOURCE} windows/displayWindow.cpp windows/displayWindow.h windows/window.cpp windows/window.h windows/galleryWindow.cpp windows/galleryWindow.h ctDataLoader.cpp ctDataLoader.h buffers/image.cpp buffers/image.h buffers/pixelBuffer.cpp buffers/pixelBuffer.h math/interpolation.cpp math/interpolation.h main.h gui/border.cpp gui/border.h)
target_link_libraries(CThead glfw GL dl pthread)
