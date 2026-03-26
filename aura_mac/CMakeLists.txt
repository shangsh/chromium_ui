# CMakeLists.txt - Aura UI macOS-specific implementation
# Only built when -DAURA_MAC is defined

cmake_minimum_required(VERSION 3.10)

# This library only compiles on macOS with AURA_MAC flag
if(APPLE)

set(AURA_MAC_SOURCES
    view/view_mac.mm
    controls/controls_mac.mm
    controls/controls_extra_mac.mm
)

add_library(aura_mac STATIC
    ${AURA_MAC_SOURCES}
)

target_include_directories(aura_mac PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../gfx
)

target_compile_definitions(aura_mac PUBLIC AURA_MAC)

target_link_libraries(aura_mac
    "-framework Cocoa"
    "-framework AppKit"
    "-framework Foundation"
)

endif()
