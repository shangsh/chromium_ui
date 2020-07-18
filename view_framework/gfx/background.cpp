
#include "background.h"

#include "base/logging.h"

#include "gfx/canvas_gdiplus.h"
#include "gfx/color_utils.h"

#include "painter.h"
#include "../view.h"

namespace view
{

    // SolidBackground is a trivial Background implementation that fills the
    // background in a solid color.
    class SolidBackground : public Background
    {
    public:
        explicit SolidBackground(const gfx::Color& color)
        {
            SetNativeControlColor(color);
        }

        void Paint(gfx::Canvas* canvas, View* view) const
        {
            // Fill the background. Note that we don't constrain to the bounds as
            // canvas is already clipped for us.
            canvas->AsCanvasGdiplus()->Clear(get_color());
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(SolidBackground);
    };

    class BackgroundPainter : public Background
    {
    public:
        BackgroundPainter(bool owns_painter, Painter* painter)
            : owns_painter_(owns_painter), painter_(painter)
        {
            DCHECK(painter);
        }

        virtual ~BackgroundPainter()
        {
            if(owns_painter_)
            {
                delete painter_;
            }
        }


        void Paint(gfx::Canvas* canvas, View* view) const
        {
            Painter::PaintPainterAt(0, 0, view->width(), view->height(),
                canvas, painter_);
        }

    private:
        bool owns_painter_;
        Painter* painter_;

        DISALLOW_COPY_AND_ASSIGN(BackgroundPainter);
    };

    Background::Background() : color_(gfx::ColorWHITE),
        native_control_brush_(NULL) {}

    Background::~Background()
    {
        DeleteObject(native_control_brush_);
    }

    void Background::SetNativeControlColor(const gfx::Color& color)
    {
        color_ = color;
        DeleteObject(native_control_brush_);
        native_control_brush_ = CreateSolidBrush(color.ToCOLORREF());
    }

    //static
    Background* Background::CreateSolidBackground(const gfx::Color& color)
    {
        return new SolidBackground(color);
    }

    //static
    Background* Background::CreateStandardPanelBackground()
    {
        return CreateVerticalGradientBackground(gfx::Color(246, 250, 255),
            gfx::Color(219, 235, 255));
    }

    //static
    Background* Background::CreateVerticalGradientBackground(
        const gfx::Color& color1, const gfx::Color& color2)
    {
        Background* background = CreateBackgroundPainter(
            true, Painter::CreateVerticalGradient(color1, color2));
        background->SetNativeControlColor(
            gfx::AlphaBlend(color1, color2, 128));

        return background;
    }

    //static
    Background* Background::CreateBackgroundPainter(bool owns_painter,
        Painter* painter)
    {
        return new BackgroundPainter(owns_painter, painter);
    }

} //namespace view