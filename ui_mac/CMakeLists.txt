# CMakeLists.txt - chromium_ui UI framework for macOS
cmake_minimum_required(VERSION 3.10)
project(chromium_ui_ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use clang for Objective-C++ support
set(CMAKE_CXX_COMPILER "/usr/bin/c++")
set(CMAKE_C_COMPILER "/usr/bin/cc")

# Get the parent directory (chromium_ui root)
get_filename_component(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# macOS UI framework sources
set(MACOS_UI_SOURCES
    ${ROOT_DIR}/view_framework/window/window_mac.mm
    ${ROOT_DIR}/view_framework/widget/widget_mac.mm
)

# Include directories
include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/base
    ${ROOT_DIR}/gfx
    ${ROOT_DIR}/view_framework
    ${ROOT_DIR}/view_framework/window
    ${ROOT_DIR}/view_framework/widget
)

# Create UI library
add_library(chromium_ui_ui STATIC
    ${MACOS_UI_SOURCES}
)

# macOS specific frameworks
target_link_libraries(chromium_ui_ui
    "-framework Cocoa"
    "-framework CoreGraphics"
    "-framework CoreText"
    "-framework AppKit"
    "-framework Foundation"
)
