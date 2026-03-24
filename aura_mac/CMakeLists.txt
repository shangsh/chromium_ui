# CMakeLists.txt - Aura UI macOS-specific implementation
# Only built when -DAURA_MAC is defined

cmake_minimum_required(VERSION 3.10)
project(aura_mac)

set(CMAKE_CXX_STANDARD 17)

# This library only compiles on macOS with AURA_MAC flag
if(APPLE)

set(AURA_MAC_SOURCES
    view/view_mac.cpp
    controls/controls_mac.cpp
    controls/controls_extra_mac.cpp
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../gfx
)

add_library(aura_mac STATIC
    ${AURA_MAC_SOURCES}
)

target_compile_definitions(aura_mac PUBLIC AURA_MAC)

target_link_libraries(aura_mac
    "-framework Cocoa"
    "-framework AppKit"
    "-framework Foundation"
)

endif()
