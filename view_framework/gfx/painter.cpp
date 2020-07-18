
#include "painter.h"

#include "base/logging.h"

#include "gfx/bitmap.h"
#include "gfx/brush.h"
#include "gfx/canvas.h"
#include "gfx/insets.h"
#include "gfx/point.h"

#include "../app/resource_bundle.h"

namespace view
{

    namespace
    {

        class GradientPainter : public Painter
        {
        public:
            GradientPainter(bool horizontal, const gfx::Color& top,
                const gfx::Color& bottom) : horizontal_(horizontal)
            {
                colors_[0] = top;
                colors_[1] = bottom;
            }

            virtual ~GradientPainter() {}

            void Paint(int w, int h, gfx::Canvas* canvas)
            {
                gfx::Brush brush(gfx::Point(0, 0), gfx::Point(w, h),
                    colors_[0], colors_[1], horizontal_);
                canvas->FillRectInt(brush, 0, 0, w, h);
            }

        private:
            bool horizontal_;
            gfx::Color colors_[2];

            DISALLOW_COPY_AND_ASSIGN(GradientPainter);
        };


        class ImagePainter : public Painter
        {
        public:
            ImagePainter(const gfx::Bitmap& image,
                const gfx::Insets& insets,
                bool paint_center)
                : image_(image),
                insets_(insets),
                paint_center_(paint_center)
            {
                DCHECK(image.Width()>insets.width() &&
                    image.Height()>insets.height());
            }

            // Paints the images.
            virtual void Paint(int w, int h, gfx::Canvas* canvas)
            {
                if(w==image_.Width() && h==image_.Height())
                {
                    // Early out if the size we're to render at equals the size of the image.
                    canvas->DrawBitmapInt(image_, 0, 0);
                    return;
                }
                // Upper left.
                canvas->DrawBitmapInt(image_, 0, 0, insets_.left(), insets_.top(),
                    0, 0, insets_.left(), insets_.top());
                // Top edge.
                canvas->DrawBitmapInt(
                    image_,
                    insets_.left(), 0, image_.Width()-insets_.width(), insets_.top(),
                    insets_.left(), 0, w-insets_.width(), insets_.top());
                // Upper right.
                canvas->DrawBitmapInt(
                    image_,
                    image_.Width()-insets_.right(), 0, insets_.right(), insets_.top(),
                    w - insets_.right(), 0, insets_.right(), insets_.top());
                // Right edge.
                canvas->DrawBitmapInt(
                    image_,
                    image_.Width()-insets_.right(), insets_.top(),
                    insets_.right(), image_.Height()-insets_.height(),
                    w - insets_.right(), insets_.top(), insets_.right(),
                    h - insets_.height());
                // Bottom right.
                canvas->DrawBitmapInt(
                    image_,
                    image_.Width()-insets_.right(), image_.Height()-insets_.bottom(),
                    insets_.right(), insets_.bottom(),
                    w - insets_.right(), h - insets_.bottom(), insets_.right(),
                    insets_.bottom());
                // Bottom edge.
                canvas->DrawBitmapInt(
                    image_,
                    insets_.left(), image_.Height()-insets_.bottom(),
                    image_.Width()-insets_.width(), insets_.bottom(),
                    insets_.left(), h-insets_.bottom(), w-insets_.width(),
                    insets_.bottom());
                // Bottom left.
                canvas->DrawBitmapInt(
                    image_,
                    0, image_.Height()-insets_.bottom(), insets_.left(),
                    insets_.bottom(),
                    0, h - insets_.bottom(), insets_.left(), insets_.bottom());
                // Left.
                canvas->DrawBitmapInt(
                    image_,
                    0, insets_.top(), insets_.left(), image_.Height()-insets_.height(),
                    0, insets_.top(), insets_.left(), h-insets_.height());
                // Center.
                if(paint_center_)
                {
                    canvas->DrawBitmapInt(
                        image_,
                        insets_.left(), insets_.top(),
                        image_.Width()-insets_.width(), image_.Height()-insets_.height(),
                        insets_.left(), insets_.top(),
                        w-insets_.width(), h-insets_.height());
                }
            }

        private:
            gfx::Bitmap image_;
            const gfx::Insets insets_;
            bool paint_center_;

            DISALLOW_COPY_AND_ASSIGN(ImagePainter);
        };

    }

    // static
    void Painter::PaintPainterAt(int x, int y, int w, int h,
        gfx::Canvas* canvas, Painter* painter)
    {
        DCHECK(canvas && painter);
        if(w<0 || h<0)
        {
            return;
        }
        canvas->Save();
        canvas->TranslateInt(x, y);
        painter->Paint(w, h, canvas);
        canvas->Restore();
    }

    // static
    Painter* Painter::CreateHorizontalGradient(const gfx::Color& c1,
        const gfx::Color& c2)
    {
        return new GradientPainter(true, c1, c2);
    }

    // static
    Painter* Painter::CreateVerticalGradient(const gfx::Color& c1,
        const gfx::Color& c2)
    {
        return new GradientPainter(false, c1, c2);
    }

    // static
    Painter* Painter::CreateImagePainter(const gfx::Bitmap& image,
        const gfx::Insets& insets,
        bool paint_center)
    {
        return new ImagePainter(image, insets, paint_center);
    }

    HorizontalPainter::HorizontalPainter(const int image_resource_names[])
    {
        ResourceBundle& rb = ResourceBundle::GetSharedInstance();
        for(int i=0; i<3; ++i)
        {
            images_[i] = rb.GetBitmapNamed(image_resource_names[i]);
        }
        height_ = images_[LEFT].Height();
        DCHECK(images_[LEFT].Height()==images_[RIGHT].Height() &&
            images_[LEFT].Height()==images_[CENTER].Height());
    }

    void HorizontalPainter::Paint(int w, int h, gfx::Canvas* canvas)
    {
        if(w < (images_[LEFT].Width()+images_[CENTER].Width()+
            images_[RIGHT].Width()))
        {
            // No room to paint.
            return;
        }
        canvas->DrawBitmapInt(images_[LEFT], 0, 0);
        canvas->DrawBitmapInt(images_[RIGHT], w-images_[RIGHT].Width(), 0);
        canvas->TileImageInt(images_[CENTER],
            images_[LEFT].Width(),
            0,
            w-images_[LEFT].Width()-images_[RIGHT].Width(),
            height_);
    }

} //namespace view