# CMakeLists.txt - chromium_ui macOS UI Framework
cmake_minimum_required(VERSION 3.10)
project(chromium_ui_mac)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use system clang
set(CMAKE_CXX_COMPILER "/usr/bin/c++")
set(CMAKE_C_COMPILER "/usr/bin/cc")

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")

# macOS UI Sources
set(MACOS_UI_SOURCES
    ${ROOT_DIR}/view_framework/view_mac.cpp
    ${ROOT_DIR}/view_framework/window/window_mac.mm
    ${ROOT_DIR}/view_framework/widget/widget_mac.mm
    ${ROOT_DIR}/view_framework/controls/button_mac.cpp
    ${ROOT_DIR}/view_framework/controls/label_mac.cpp
    ${ROOT_DIR}/view_framework/controls/textfield_mac.cpp
)

# Include directories
include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/base
    ${ROOT_DIR}/gfx
    ${ROOT_DIR}/view_framework
    ${ROOT_DIR}/view_framework/controls
)

# Create library
add_library(chromium_ui_mac STATIC
    ${MACOS_UI_SOURCES}
)

# Link frameworks
target_link_libraries(chromium_ui_mac
    "-framework Cocoa"
    "-framework AppKit"
    "-framework Foundation"
)
