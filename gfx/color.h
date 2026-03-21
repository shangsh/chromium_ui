#ifndef GFX_COLOR_H_
#define GFX_COLOR_H_

#pragma once

#include <algorithm>
using std::min;
using std::max;

#include "base/basic_types.h"

#if defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <gdiplus.h>
#elif defined(PLATFORM_MACOS)
#import <AppKit/AppKit.h>
#include <CoreGraphics/CoreGraphics.h>
#endif

namespace gfx
{

class Color
{
public:
    Color() : a_(255), r_(0), g_(0), b_(0) {}
    
#if defined(PLATFORM_WINDOWS)
    Color(COLORREF color) { SetFromCOLORREF(color); }
    Color(const Gdiplus::Color& color) {
        a_ = color.GetA();
        r_ = color.GetR();
        g_ = color.GetG();
        b_ = color.GetB();
    }
#endif
    
    Color(uint8 r, uint8 g, uint8 b) : a_(255), r_(r), g_(g), b_(b) {}
    Color(uint8 a, uint8 r, uint8 g, uint8 b) : a_(a), r_(r), g_(g), b_(b) {}

    uint8 GetA() const { return a_; }
    uint8 GetR() const { return r_; }
    uint8 GetG() const { return g_; }
    uint8 GetB() const { return b_; }

    uint32 GetValue() const { return (a_ << 24) | (r_ << 16) | (g_ << 8) | b_; }
    void SetValue(uint32 argb) {
        a_ = (argb >> 24) & 0xFF;
        r_ = (argb >> 16) & 0xFF;
        g_ = (argb >> 8) & 0xFF;
        b_ = argb & 0xFF;
    }

#if defined(PLATFORM_WINDOWS)
    COLORREF ToCOLORREF() const { return RGB(r_, g_, b_); }
    void SetFromCOLORREF(COLORREF rgb)
    {
        r_ = GetRValue(rgb);
        g_ = GetGValue(rgb);
        b_ = GetBValue(rgb);
        a_ = 255;
    }
    const Gdiplus::Color& GetNativeColor() const { 
        static Gdiplus::Color color(a_, r_, g_, b_);
        return color;
    }
#endif

#if defined(PLATFORM_MACOS)
    CGColorRef CreateCGColor() const {
        CGFloat components[] = { r_ / 255.0, g_ / 255.0, b_ / 255.0, a_ / 255.0 };
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGColorRef color = CGColorCreate(colorSpace, components);
        CGColorSpaceRelease(colorSpace);
        return color;
    }

    NSColor* CreateNSColor() const {
        return [NSColor colorWithRed:(r_ / 255.0)
                                green:(g_ / 255.0)
                                 blue:(b_ / 255.0)
                                alpha:(a_ / 255.0)];
    }
#endif

    static Color White() { return Color(255, 255, 255); }
    static Color Black() { return Color(0, 0, 0); }
    static Color Red() { return Color(255, 0, 0); }
    static Color Green() { return Color(0, 255, 0); }
    static Color Blue() { return Color(0, 0, 255); }
    static Color Transparent() { return Color(0, 0, 0, 0); }

private:
    uint8 a_;
    uint8 r_;
    uint8 g_;
    uint8 b_;
};

} // namespace gfx

#endif // GFX_COLOR_H_
