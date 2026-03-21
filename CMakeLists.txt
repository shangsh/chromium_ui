# CMakeLists.txt - Cross-platform build for chromium_ui
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
    include_directories(/System/Library/Frameworks)
elseif(UNIX)
    set(PLATFORM_LINUX 1)
    message(Status "Building for Linux")
endif()

# Platform-specific sources
set(PLATFORM_SOURCES "")
set(PLATFORM_HEADERS "")

if(WIN32)
    list(APPEND PLATFORM_SOURCES
        gfx/platform/canvas_win.cpp
        gfx/platform/bitmap_win.cpp
        gfx/platform/font_win.cpp
        gfx/platform/window_win.cpp
        gfx/platform/native_theme_win.cpp
    )
    list(APPEND PLATFORM_HEADERS
        gfx/platform/canvas_win.h
        gfx/platform/bitmap_win.h
        gfx/platform/font_win.h
        gfx/platform/window_win.h
        gfx/platform/native_theme_win.h
    )
elseif(APPLE)
    list(APPEND PLATFORM_SOURCES
        gfx/platform/canvas_mac.mm
        gfx/platform/bitmap_mac.mm
        gfx/platform/font_mac.mm
        gfx/platform/window_mac.mm
        gfx/platform/native_theme_mac.mm
    )
    list(APPEND PLATFORM_HEADERS
        gfx/platform/canvas_mac.h
        gfx/platform/bitmap_mac.h
        gfx/platform/font_mac.h
        gfx/platform/window_mac.h
        gfx/platform/native_theme_mac.h
    )
endif()

# Base sources (platform-independent)
set(BASE_SOURCES
    gfx/bitmap.cpp
    gfx/bitmap_operations.cpp
    gfx/brush.cpp
    gfx/canvas.cpp
    gfx/color_utils.cpp
    gfx/font.cpp
    gfx/gdi_util.cpp
    gfx/gdiplus_initializer.cpp
    gfx/insets.cpp
    gfx/path.cpp
    gfx/point.cpp
    gfx/rect.cpp
    gfx/size.cpp
    gfx/window_impl.cpp
    animation/animation.cpp
    animation/animation_container.cpp
    animation/linear_animation.cpp
    animation/multi_animation.cpp
    animation/slide_animation.cpp
    animation/throb_animation.cpp
    animation/tween.cpp
    base/at_exit.cpp
    base/command_line.cpp
    base/cpu.cpp
    base/data_pack.cpp
    base/file_path.cpp
    base/lazy_instance.cpp
    base/logging.cpp
    base/message_loop.cpp
    base/object_watcher.cpp
    base/path_service.cpp
    base/platform_thread.cpp
    base/rand_util.cpp
    base/scoped_handle.cpp
    base/string_util.cpp
    base/thread_local.cpp
    base/time.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/base
    ${CMAKE_SOURCE_DIR}/gfx
)

# Create shared library
add_library(chromium_ui STATIC
    ${BASE_SOURCES}
    ${PLATFORM_SOURCES}
    ${PLATFORM_HEADERS}
)

# macOS specific frameworks
if(APPLE)
    target_link_libraries(chromium_ui
        "-framework Cocoa"
        "-framework CoreGraphics"
        "-framework CoreText"
        "-framework AppKit"
        "-framework Foundation"
    )
endif()

# Windows specific libraries
if(WIN32)
    target_link_libraries(chromium_ui
        gdi32
        user32
        kernel32
        advapi32
    )
endif()
