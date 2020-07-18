
#ifndef __brush_h__
#define __brush_h__

#pragma once

#include "base/logging.h"
#include "base/scoped_ptr.h"

#include "color.h"
#include "point.h"

namespace gfx
{

    class Brush
    {
    public:
        // 创建透明的填充画刷.
        Brush() : brush_(
            new Gdiplus::SolidBrush(Gdiplus::Color::Transparent)) {}

        // 创建颜色填充画刷.
        explicit Brush(const Color& color) : brush_(
            new Gdiplus::SolidBrush(color.GetNativeColor())) {}
        
        // 根据本地画刷创建Brush对象.
        explicit Brush(Gdiplus::Brush* native_brush)
            : brush_(native_brush)
        {
            DCHECK(native_brush);
        }
        
        // 创建渐变画刷.
        explicit Brush(const Point& point1, const Point& point2,
            const Color& color1, const Color& color2, bool horizontal)
        {
            Gdiplus::Rect rect(point1.x(), point1.y(),
                point2.x()-point1.x(), point2.y()-point1.y());

            brush_.reset(new Gdiplus::LinearGradientBrush(rect,
                color1.GetNativeColor(), color2.GetNativeColor(),
                horizontal?Gdiplus::LinearGradientModeHorizontal:
                Gdiplus::LinearGradientModeVertical));
        }

        Brush(const Brush& other)
        {
            brush_.reset(other.brush_->Clone());
        }

        Brush& operator=(const Brush& other)
        {
            brush_.reset(other.brush_->Clone());
        }

        Gdiplus::Brush* GetNativeBrush() const { return brush_.get(); }

    private:
        scoped_ptr<Gdiplus::Brush> brush_;
    };

} //namespace gfx

#endif //__brush_h__