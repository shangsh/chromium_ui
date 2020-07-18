
#ifndef __color_h__
#define __color_h__

#pragma once

#include <algorithm>
using std::min;
using std::max;

#include <windows.h>
#include <gdiplus.h>

#include "base/basic_types.h"

namespace gfx
{

    class Color
    {
    public:
        Color() {}
        Color(COLORREF color) { SetFromCOLORREF(color); }
        Color(uint8 r, uint8 g, uint8 b) : color_(r, g, b) {}
        Color(uint8 a, uint8 r, uint8 g, uint8 b)
            : color_(a, r, g, b) {}

        uint8 GetA() const { return color_.GetA(); }
        uint8 GetR() const { return color_.GetR(); }
        uint8 GetG() const { return color_.GetG(); }
        uint8 GetB() const { return color_.GetB(); }

        uint32 GetValue() const { return color_.GetValue(); }
        void SetValue(uint32 argb) { color_.SetValue(argb); }
        COLORREF ToCOLORREF() const { return color_.ToCOLORREF(); }
        void SetFromCOLORREF(COLORREF rgb)
        {
            color_.SetFromCOLORREF(rgb);
        }
        const Gdiplus::Color& GetNativeColor() const { return color_; }

    private:
        Gdiplus::Color color_;
    };

    static const Color ColorWHITE = Gdiplus::Color::White;

} //namespace gfx

#endif //__color_h__