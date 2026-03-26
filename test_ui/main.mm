#import <Cocoa/Cocoa.h>
#include <iostream>
#include "view_framework/window/window.h"
#include "view_framework/widget/widget.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        
        // Create a window
        view::Window* window = new view::Window();
        if (window->Init())
        {
            window->SetTitle(L"Chromium UI - macOS 🦞");
            window->SetBounds(gfx::Rect(100, 100, 800, 600));
            window->Show();
            std::cout << "Window created successfully!" << std::endl;
        }
        
        // Create a widget
        view::Widget* widget = new view::Widget();
        if (widget->Init())
        {
            widget->SetBounds(gfx::Rect(50, 50, 200, 100));
            widget->Show();
            std::cout << "Widget created successfully!" << std::endl;
        }
        
        [app run];
    }
    return 0;
}
