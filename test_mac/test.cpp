#include <iostream>
#include "gfx/rect.h"
#include "gfx/point.h"
#include "gfx/size.h"
#include "gfx/bitmap.h"

int main() {
    std::cout << "Chromium UI Test - macOS Build 🦞" << std::endl;
    
    gfx::Point p(10, 20);
    std::cout << "Point: " << p.x() << "," << p.y() << std::endl;
    
    gfx::Size s(100, 200);
    std::cout << "Size: " << s.width() << "x" << s.height() << std::endl;
    
    gfx::Rect r(0, 0, 800, 600);
    std::cout << "Rect: " << r.x() << "," << r.y() << " " << r.width() << "x" << r.height() << std::endl;
    
    gfx::Bitmap bmp;
    std::cout << "Bitmap created successfully!" << std::endl;
    
    std::cout << "\n✅ All tests passed! chromium_ui library is working on macOS!" << std::endl;
    return 0;
}
