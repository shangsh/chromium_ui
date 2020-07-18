
#ifndef __background_h__
#define __background_h__

#pragma once

#include "base/basic_types.h"

#include "gfx/color.h"

namespace gfx
{
    class Canvas;
}

namespace view
{

    class Painter;
    class View;

    /////////////////////////////////////////////////////////////////////////////
    //
    // Background class
    //
    // A background implements a way for views to paint a background. The
    // background can be either solid or based on a gradient. Of course,
    // Background can be subclassed to implement various effects.
    //
    // Any View can have a background. See View::SetBackground() and
    // View::PaintBackground()
    //
    /////////////////////////////////////////////////////////////////////////////
    class Background
    {
    public:
        Background();
        virtual ~Background();

        // Creates a background that fills the canvas in the specified color.
        static Background* CreateSolidBackground(const gfx::Color& color);

        // Creates a background that fills the canvas in the specified color.
        static Background* CreateSolidBackground(int r, int g, int b)
        {
            return CreateSolidBackground(gfx::Color(r, g, b));
        }

        // Creates a background that fills the canvas in the specified color.
        static Background* CreateSolidBackground(int r, int g, int b, int a)
        {
            return CreateSolidBackground(gfx::Color(a, r, g, b));
        }

        // Creates a background that contains a vertical gradient that varies
        // from |color1| to |color2|
        static Background* CreateVerticalGradientBackground(
            const gfx::Color& color1,
            const gfx::Color& color2);

        // Creates Chrome's standard panel background
        static Background* CreateStandardPanelBackground();

        // Creates a Background from the specified Painter. If owns_painter is
        // true, the Painter is deleted when the Border is deleted.
        static Background* CreateBackgroundPainter(bool owns_painter,
            Painter* painter);

        // Render the background for the provided view
        virtual void Paint(gfx::Canvas* canvas, View* view) const = 0;

        // Set a solid, opaque color to be used when drawing backgrounds of native
        // controls.  Unfortunately alpha=0 is not an option.
        void SetNativeControlColor(const gfx::Color& color);

        // Returns the "background color".  This is equivalent to the color set in
        // SetNativeControlColor().  For solid backgrounds, this is the color; for
        // gradient backgrounds, it's the midpoint of the gradient; for painter
        // backgrounds, this is not useful (returns a default color).
        gfx::Color get_color() const { return color_; }

        // Get the brush that was specified by SetNativeControlColor
        HBRUSH GetNativeControlBrush() const { return native_control_brush_; };

    private:
        gfx::Color color_;
        HBRUSH native_control_brush_;

        DISALLOW_COPY_AND_ASSIGN(Background);
    };

} //namespace view

#endif //__background_h__