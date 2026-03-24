# CMakeLists.txt - chromium_ui Cross-platform Build
cmake_minimum_required(VERSION 3.10)
project(chromium_ui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Detect platform
if(WIN32)
    set(PLATFORM_WINDOWS 1)
    message(STATUS "Building for Windows")
elseif(APPLE)
    set(PLATFORM_MACOS 1)
    message(STATUS "Building for macOS")
elseif(UNIX)
    set(PLATFORM_LINUX 1)
    message(STATUS "Building for Linux")
endif()

# Base sources (Windows)
set(BASE_SOURCES
    gfx/bitmap.cpp
    gfx/insets.cpp
    gfx/path.cpp
    gfx/point.cpp
    gfx/rect.cpp
    gfx/size.cpp
    gfx/window_impl.cpp
    gfx/platform_bitmap.cpp
    gfx/platform_font.cpp
    gfx/gdiplus_initializer.cpp
    animation/animation.cpp
    animation/animation_container.cpp
    animation/linear_animation.cpp
    animation/multi_animation.cpp
    animation/slide_animation.cpp
    animation/throb_animation.cpp
    animation/tween.cpp
    base/logging.cpp
    base/ref_counted.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/base
    ${CMAKE_SOURCE_DIR}/gfx
)

# Create static library (works on all platforms)
add_library(chromium_ui STATIC
    ${BASE_SOURCES}
)

# Windows specific libraries
if(WIN32)
    target_link_libraries(chromium_ui
        gdi32
        user32
        kernel32
        advapi32
    )
endif()

# macOS UI framework - only build on macOS
if(APPLE)
    # Add macOS-specific Aura UI implementation
    # This only compiles when AURA_MAC is defined
    add_subdirectory(aura_mac)
endif()
