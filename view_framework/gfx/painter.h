
#ifndef __painter_h__
#define __painter_h__

#pragma once

#include "base/basic_types.h"

#include "gfx/color.h"
#include "gfx/bitmap.h"

namespace gfx
{
    class Canvas;
    class Insets;
}

namespace view
{

    // Painter, as the name implies, is responsible for painting in a particular
    // region. Think of Painter as a Border or Background that can be painted
    // in any region of a View.
    class Painter
    {
    public:
        // A convenience method for painting a Painter in a particular region.
        // This translates the canvas to x/y and paints the painter.
        static void PaintPainterAt(int x, int y, int w, int h,
            gfx::Canvas* canvas, Painter* painter);

        // Creates a painter that draws a gradient between the two colors.
        static Painter* CreateHorizontalGradient(const gfx::Color& c1,
            const gfx::Color& c2);
        static Painter* CreateVerticalGradient(const gfx::Color& c1,
            const gfx::Color& c2);

        // Creates a painter that divides |image| into nine regions. The four corners
        // are rendered at the size specified in insets (for example, the upper
        // left corners is rendered at 0x0 with a size of
        // insets.left()xinsets.right()). The four edges are stretched to fill the
        // destination size.
        // Ownership is passed to the caller.
        static Painter* CreateImagePainter(const gfx::Bitmap& image,
            const gfx::Insets& insets, bool paint_center);

        virtual ~Painter() {}

        // Paints the painter in the specified region.
        virtual void Paint(int w, int h, gfx::Canvas* canvas) = 0;
    };

    // HorizontalPainter paints 3 images into a box: left, center and right. The
    // left and right images are drawn to size at the left/right edges of the
    // region. The center is tiled in the remaining space. All images must have the
    // same height.
    class HorizontalPainter : public Painter
    {
    public:
        // Constructs a new HorizontalPainter loading the specified image names.
        // The images must be in the order left, right and center.
        explicit HorizontalPainter(const int image_resource_names[]);

        virtual ~HorizontalPainter() {}

        // Paints the images.
        virtual void Paint(int w, int h, gfx::Canvas* canvas);

        // Height of the images.
        int height() const { return height_; }

    private:
        // The image chunks.
        enum BorderElements
        {
            LEFT,
            CENTER,
            RIGHT
        };

        // The height.
        int height_;
        // NOTE: the images are owned by ResourceBundle. Don't free them.
        gfx::Bitmap images_[3];

        DISALLOW_COPY_AND_ASSIGN(HorizontalPainter);
    };

} //namespace view

#endif //__painter_h__