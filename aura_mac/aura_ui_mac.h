// Aura UI - macOS Platform Implementation
// Only compiled when building for macOS with AURA_MAC defined
// Windows builds ignore this file completely

#ifdef AURA_MAC

#ifndef AURA_UI_MAC_H_
#define AURA_UI_MAC_H_

// Platform detection
#if defined(__APPLE__)
    #define AURA_PLATFORM_MAC 1
#elif defined(_WIN32)
    #define AURA_PLATFORM_WIN 1
#elif defined(__linux__)
    #define AURA_PLATFORM_LINUX 1
#endif

// Include platform-specific headers
#if AURA_PLATFORM_MAC
    #import <Cocoa/Cocoa.h>
    #import <AppKit/AppKit.h>
#endif

#include <string>
#include <vector>

// Forward declarations
namespace aura {
    class View;
    class Widget;
    class Button;
    class Label;
    class TextField;
    class Checkbox;
    class ComboBox;
    class ProgressBar;
    class Slider;
    class TabControl;
    class Menu;
    class Toolbar;
    class StatusBar;
    class Dialog;
    class TreeView;
}

#endif // AURA_UI_MAC_H_
#endif // AURA_MAC
