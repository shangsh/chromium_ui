# demo_mac - macOS GUI Demo for chromium_ui
cmake_minimum_required(VERSION 3.10)

if(APPLE)
    add_executable(demo_mac
        main.mm
    )

    target_compile_definitions(demo_mac PUBLIC AURA_MAC)

    target_link_libraries(demo_mac
        aura_mac
        chromium_ui
        "-framework Cocoa"
        "-framework AppKit"
        "-framework Foundation"
    )

    target_include_directories(demo_mac PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/../gfx
        ${CMAKE_CURRENT_SOURCE_DIR}/../aura
    )
endif()
