
#include "canvas_gdiplus.h"

#include <limits>

#include "base/rtl.h"
#include "base/logging.h"

#include "brush.h"
#include "font.h"
#include "rect.h"

namespace
{

    // We make sure that LTR text we draw in an RTL context is modified
    // appropriately to make sure it maintains it LTR orientation.
    void DoDrawText(HDC hdc,
        const std::wstring& text,
        RECT* text_bounds,
        int flags)
    {
        std::wstring localized_text;
        const wchar_t* string_ptr = text.c_str();
        int string_size = static_cast<int>(text.length());
        // Only adjust string directionality if both of the following are true:
        // 1. The current locale is RTL.
        // 2. The string itself has RTL directionality.
        if(flags & DT_RTLREADING)
        {
            if(base::AdjustStringForLocaleDirection(text, &localized_text))
            {
                string_ptr = localized_text.c_str();
                string_size = static_cast<int>(localized_text.length());
            }
        }

        DrawText(hdc, string_ptr, string_size, text_bounds, flags);
    }

    // Compute the windows flags necessary to implement the provided text Canvas
    // flags.
    int ComputeFormatFlags(int flags, const std::wstring& text)
    {
        // Setting the text alignment explicitly in case it hasn't already been set.
        // This will make sure that we don't align text to the left on RTL locales
        // just because no alignment flag was passed to DrawStringInt().
        if(!(flags & (gfx::Canvas::TEXT_ALIGN_CENTER|
            gfx::Canvas::TEXT_ALIGN_RIGHT|gfx::Canvas::TEXT_ALIGN_LEFT)))
        {
            flags |= gfx::CanvasGdiplus::DefaultCanvasTextAlignment();
        }

        // horizontal alignment
        int f = 0;
        if(flags & gfx::Canvas::TEXT_ALIGN_CENTER)
        {
            f |= DT_CENTER;
        }
        else if(flags & gfx::Canvas::TEXT_ALIGN_RIGHT)
        {
            f |= DT_RIGHT;
        }
        else
        {
            f |= DT_LEFT;
        }

        // vertical alignment
        if(flags & gfx::Canvas::TEXT_VALIGN_TOP)
        {
            f |= DT_TOP;
        }
        else if(flags & gfx::Canvas::TEXT_VALIGN_BOTTOM)
        {
            f |= DT_BOTTOM;
        }
        else
        {
            f |= DT_VCENTER;
        }

        if(flags & gfx::Canvas::MULTI_LINE)
        {
            f |= DT_WORDBREAK;
            if(flags & gfx::Canvas::CHARACTER_BREAK)
            {
                f |= DT_EDITCONTROL; // Turns on character breaking (not documented)
            }
            else if(!(flags & gfx::Canvas::NO_ELLIPSIS))
            {
                f |= DT_WORD_ELLIPSIS;
            }
        }
        else
        {
            f |= DT_SINGLELINE;
        }

        if(flags & gfx::Canvas::HIDE_PREFIX)
        {
            f |= DT_HIDEPREFIX;
        }
        else if((flags & gfx::Canvas::SHOW_PREFIX) == 0)
        {
            f |= DT_NOPREFIX;
        }

        if(!(flags & gfx::Canvas::NO_ELLIPSIS))
        {
            f |= DT_END_ELLIPSIS;
        }

        // In order to make sure RTL/BiDi strings are rendered correctly, we must
        // pass the flag DT_RTLREADING to DrawText (when the locale's language is
        // a right-to-left language) so that Windows does the right thing.
        //
        // In addition to correctly displaying text containing both RTL and LTR
        // elements (for example, a string containing a telephone number within a
        // sentence in Hebrew, or a sentence in Hebrew that contains a word in
        // English) this flag also makes sure that if there is not enough space to
        // display the entire string, the ellipsis is displayed on the left hand side
        // of the truncated string and not on the right hand side.
        //
        // We make a distinction between Chrome UI strings and text coming from a web
        // page.
        //
        // For text coming from a web page we determine the alignment based on the
        // first character with strong directionality. If the directionality of the
        // first character with strong directionality in the text is LTR, the
        // alignment is set to DT_LEFT, and the directionality should not be set as
        // DT_RTLREADING.
        //
        // This heuristic doesn't work for Chrome UI strings since even in RTL
        // locales, some of those might start with English text but we know they're
        // localized so we always want them to be right aligned, and their
        // directionality should be set as DT_RTLREADING.
        //
        // Caveat: If the string is purely LTR, don't set DTL_RTLREADING since when
        // the flag is set, LRE-PDF don't have the desired effect of rendering
        // multiline English-only text as LTR.
        //
        // Note that if the caller is explicitly requesting displaying the text
        // using RTL directionality then we respect that and pass DT_RTLREADING to
        // ::DrawText even if the locale is LTR.
        if((flags & gfx::Canvas::FORCE_RTL_DIRECTIONALITY) ||
            (base::IsRTL() &&
            (f&DT_RIGHT) && base::StringContainsStrongRTLChars(text)))
        {
            f |= DT_RTLREADING;
        }

        return f;
    }

}

namespace gfx
{

    CanvasGdiplus::CanvasGdiplus(int width, int height, bool is_opaque)
    {
        bool initialized = initialize(width, height, is_opaque);
        DCHECK(initialized);
    }

    CanvasGdiplus::CanvasGdiplus() {}

    CanvasGdiplus::~CanvasGdiplus() {}

    bool CanvasGdiplus::initialize(int width, int height, bool is_opaque)
    {
        if((width==0) || (height==0))
        {
            width = 1;
            height = 1;
        }

        Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(width, height);
        if(!bitmap)
        {
            return false;
        }

        mem_bitmap_ = Bitmap(bitmap);
        mem_graphics_.reset(new Gdiplus::Graphics(bitmap));

        if(is_opaque)
        {
            // 为了辅助查找bugs, 设置背景色为某种很容易被发现的颜色.
            Clear(Color(255, 0, 255, 128));
        }

        return true;
    }

    // static
    int CanvasGdiplus::DefaultCanvasTextAlignment()
    {
        if(!base::IsRTL())
        {
            return Canvas::TEXT_ALIGN_LEFT;
        }
        return Canvas::TEXT_ALIGN_RIGHT;
    }

    // static
    void CanvasGdiplus::InitializeDC(HDC context)
    {
        // Enables world transformation.
        // If the GM_ADVANCED graphics mode is set, GDI always draws arcs in the
        // counterclockwise direction in logical space. This is equivalent to the
        // statement that, in the GM_ADVANCED graphics mode, both arc control points
        // and arcs themselves fully respect the device context's world-to-device
        // transformation.
        BOOL res = SetGraphicsMode(context, GM_ADVANCED);
        DCHECK(res != 0);

        // Enables dithering.
        res = SetStretchBltMode(context, HALFTONE);
        DCHECK(res != 0);
        // As per SetStretchBltMode() documentation, SetBrushOrgEx() must be called
        // right after.
        res = SetBrushOrgEx(context, 0, 0, NULL);
        DCHECK(res != 0);

        // Sets up default orientation.
        res = SetArcDirection(context, AD_CLOCKWISE);
        DCHECK(res != 0);

        // Sets up default colors.
        res = SetBkColor(context, RGB(255, 255, 255));
        DCHECK(res != CLR_INVALID);
        res = SetTextColor(context, RGB(0, 0, 0));
        DCHECK(res != CLR_INVALID);
        res = SetDCBrushColor(context, RGB(255, 255, 255));
        DCHECK(res != CLR_INVALID);
        res = SetDCPenColor(context, RGB(0, 0, 0));
        DCHECK(res != CLR_INVALID);

        // Sets up default transparency.
        res = SetBkMode(context, OPAQUE);
        DCHECK(res != 0);
        res = SetROP2(context, R2_COPYPEN);
        DCHECK(res != 0);
    }

    // Checks each pixel immediately adjacent to the given pixel in the bitmap. If
    // any of them are not the halo color, returns true. This defines the halo of
    // pixels that will appear around the text. Note that we have to check each
    // pixel against both the halo color and transparent since DrawStringWithHalo
    // will modify the bitmap as it goes, and clears pixels shouldn't count as
    // changed.
    static bool pixelShouldGetHalo(const Bitmap& bitmap,
        int x, int y, const Color& halo_color)
    {
        if(x>0 &&
            bitmap.GetPixel(x-1, y).GetValue()!=halo_color.GetValue() &&
            bitmap.GetPixel(x-1, y).GetValue()!=0)
        {
            return true; // Touched pixel to the left.
        }
        if(x<bitmap.Width()-1 &&
            bitmap.GetPixel(x+1, y).GetValue()!=halo_color.GetValue() &&
            bitmap.GetPixel(x+1, y).GetValue()!=0)
        {
            return true; // Touched pixel to the right.
        }
        if(y>0 &&
            bitmap.GetPixel(x, y-1).GetValue()!=halo_color.GetValue() &&
            bitmap.GetPixel(x, y-1).GetValue()!=0)
        {
            return true; // Touched pixel above.
        }
        if(y<bitmap.Height()-1 &&
            bitmap.GetPixel(x, y+1).GetValue()!=halo_color.GetValue() &&
            bitmap.GetPixel(x, y+1).GetValue()!=0)
        {
            return true; // Touched pixel below.
        }
        return false;
    }

    void CanvasGdiplus::DrawStringWithHalo(const std::wstring& text,
        const Font& font,
        const Color& text_color,
        const Color& halo_color_in,
        int x, int y, int w, int h,
        int flags)
    {
        // Some callers will have semitransparent halo colors, which we don't handle
        // (since the resulting image can have 1-bit transparency only).
        Color halo_color = halo_color_in.GetValue() | 0xFF000000;

        // Create a temporary buffer filled with the halo color. It must leave room
        // for the 1-pixel border around the text.
        CanvasGdiplus text_canvas(w+2, h+2, true);
        text_canvas.DrawRectInt(halo_color, 0, 0, w+2, h+2);

        // Draw the text into the temporary buffer. This will have correct
        // ClearType since the background color is the same as the halo color.
        text_canvas.DrawStringInt(text, font, text_color, 1, 1, w, h, flags);

        Bitmap& text_bitmap = text_canvas.mem_bitmap_;
        Gdiplus::Rect rc_lock(0, 0, text_bitmap.Width(), text_bitmap.Height());
        Gdiplus::BitmapData lock_text_bitmap;
        text_bitmap.GetNativeBitmap()->LockBits(&rc_lock,
            Gdiplus::ImageLockModeRead,
            PixelFormat32bppARGB,
            &lock_text_bitmap);
        for(int cur_y=0; cur_y<h+2; cur_y++)
        {
            uint32* text_row = static_cast<uint32*>(lock_text_bitmap.Scan0) +
                cur_y*lock_text_bitmap.Width;
            for(int cur_x=0; cur_x<w+2; cur_x++)
            {
                if(text_row[cur_x] == halo_color.GetValue())
                {
                    // This pixel was not touched by the text routines. See if it borders
                    // a touched pixel in any of the 4 directions (not diagonally).
                    if(!pixelShouldGetHalo(text_bitmap, cur_x, cur_y, halo_color))
                    {
                        text_row[cur_x] = 0; // Make transparent.
                    }
                }
                else
                {
                    text_row[cur_x] |= 0xff << Gdiplus::Color::AlphaShift; // Make opaque.
                }
            }
        }
        text_bitmap.GetNativeBitmap()->UnlockBits(&lock_text_bitmap);

        // Draw the halo bitmap with blur.
        DrawBitmapInt(text_bitmap, x-1, y-1);
    }

    void CanvasGdiplus::Clear(const Color& color)
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        current->Clear(color.GetNativeColor());
    }

    Bitmap CanvasGdiplus::ExtractBitmap() const
    {
        DCHECK(!mem_bitmap_.IsNull());

        // 生成一个位图, 绘制画布内容到其中并返回.
        Gdiplus::Bitmap* bitmap = mem_bitmap_.GetNativeBitmap();
        Gdiplus::Rect rc_bitmap(0, 0, bitmap->GetWidth(), bitmap->GetHeight());
        return Bitmap(bitmap->Clone(rc_bitmap, bitmap->GetPixelFormat()));
    }

    void CanvasGdiplus::DrawToHDC(HDC hdc, int x, int y, const RECT* src_rect)
    {
        DCHECK(!mem_bitmap_.IsNull());

        RECT temp_rect;
        if(!src_rect)
        {
            temp_rect.left = 0;
            temp_rect.right = mem_bitmap_.Width();
            temp_rect.top = 0;
            temp_rect.bottom = mem_bitmap_.Height();
            src_rect = &temp_rect;
        }
        Gdiplus::Graphics g(hdc);
        g.DrawImage(mem_bitmap_.GetNativeBitmap(), x, y,
            temp_rect.left, temp_rect.top,
            temp_rect.right-temp_rect.left,
            temp_rect.bottom-temp_rect.top,
            Gdiplus::UnitPixel);
    }

    void CanvasGdiplus::Save()
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        states_.push(current->Save());
    }

    void CanvasGdiplus::Restore()
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        DCHECK(states_.size());

        current->Restore(states_.top());
        states_.pop();
    }

    void CanvasGdiplus::SaveLayer(uint8 alpha)
    {
        DCHECK(!mem_bitmap_.IsNull());

        SaveLayer(alpha, Rect(0, 0, mem_bitmap_.Width(),
            mem_bitmap_.Height()));
    }

    void CanvasGdiplus::SaveLayer(uint8 alpha, const Rect& layer_bounds)
    {
        DCHECK(!mem_bitmap_.IsNull());

        CanvasLayer* layer = new CanvasLayer();
        DCHECK(layer);
        layer->alpha = alpha;
        layer->bounds = layer_bounds;
        layer->mem_bitmap = Bitmap(new Gdiplus::Bitmap(
            mem_bitmap_.Width(), mem_bitmap_.Height()));
        layer->mem_graphics.reset(new Gdiplus::Graphics(
            layer->mem_bitmap.GetNativeBitmap()));
        layers_.push(layer);
    }

    void CanvasGdiplus::RestoreLayer()
    {
        DCHECK(layers_.size());
        
        CanvasLayer* layer = layers_.top();
        layers_.pop();
        DCHECK(layer);

        Gdiplus::Graphics* current = GetCurrentGraphics();
        Gdiplus::Bitmap* native_bitmap =
            layer->mem_bitmap.GetNativeBitmap();
        Gdiplus::ImageAttributes ia;
        Gdiplus::ColorMatrix cm =
        {
            1.0, 0.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 0.0, (static_cast<float>(layer->alpha))/255, 0.0,
            0.0, 0.0, 0.0, 0.0, 1.0
        };
        ia.SetColorMatrix(&cm);
        Gdiplus::Rect destRect(layer->bounds.x(), layer->bounds.y(),
            layer->bounds.width(), layer->bounds.height());
        current->DrawImage(native_bitmap, destRect,
            layer->bounds.x(), layer->bounds.y(),
            layer->bounds.width(), layer->bounds.height(),
            Gdiplus::UnitPixel, &ia);

        delete layer;
    }

    bool CanvasGdiplus::ClipRectInt(int x, int y, int w, int h)
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        return current->SetClip(Gdiplus::Rect(x, y, w, h),
            Gdiplus::CombineModeIntersect) == Gdiplus::Ok;
    }

    void CanvasGdiplus::TranslateInt(int x, int y)
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        current->TranslateTransform(static_cast<Gdiplus::REAL>(x),
            static_cast<Gdiplus::REAL>(y));
    }

    void CanvasGdiplus::ScaleInt(int x, int y)
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        current->ScaleTransform(static_cast<Gdiplus::REAL>(x),
            static_cast<Gdiplus::REAL>(y));
    }

    void CanvasGdiplus::FillRectInt(const Color& color,
        int x, int y, int w, int h)
    {
        Brush brush(color);
        FillRectInt(brush, x, y, w, h);
    }

    void CanvasGdiplus::FillRectInt(const Brush& brush,
        int x, int y, int w, int h)
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        current->FillRectangle(brush.GetNativeBrush(), x, y, w, h);
    }

    void CanvasGdiplus::DrawRectInt(const Color& color,
        int x, int y, int w, int h)
    {
        Gdiplus::Pen pen(color.GetNativeColor());

        Gdiplus::Graphics* current = GetCurrentGraphics();
        current->DrawRectangle(&pen, x, y, w, h);
    }

    void CanvasGdiplus::DrawLineInt(const Color& color,
        int x1, int y1, int x2, int y2)
    {
        Gdiplus::Pen pen(color.GetNativeColor());

        Gdiplus::Graphics* current = GetCurrentGraphics();
        current->DrawLine(&pen, x1, y1, x2, y2);
    }

    void CanvasGdiplus::DrawFocusRect(int x, int y, int width, int height)
    {
        // WLW TODO: fix it.
        //HDC dc = BeginPlatformPaint();
        //::DrawFocusRect(dc, &(Rect(x, y, width, height).ToRECT()));
        //EndPlatformPaint(dc);
        Gdiplus::Graphics* current = GetCurrentGraphics();
        Gdiplus::Pen pen(Gdiplus::Color::Gray);
        pen.SetDashStyle(Gdiplus::DashStyleDot);
        current->DrawRectangle(&pen, x, y, width-1, height-1);
    }

    void CanvasGdiplus::DrawBitmapInt(const Bitmap& bitmap, int x, int y)
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        if(!bitmap.IsNull())
        {
            current->DrawImage(bitmap.GetNativeBitmap(), x, y);
        }
    }

    void CanvasGdiplus::DrawBitmapInt(const Bitmap& bitmap,
        int src_x, int src_y, int src_w, int src_h,
        int dest_x, int dest_y, int dest_w, int dest_h)
    {
        Gdiplus::Graphics* current = GetCurrentGraphics();
        if(!bitmap.IsNull())
        {
            current->DrawImage(bitmap.GetNativeBitmap(),
                Gdiplus::Rect(dest_x, dest_y, dest_w, dest_h),
                src_x, src_y, src_w, src_h, Gdiplus::UnitPixel);
        }
    }

    void CanvasGdiplus::DrawStringInt(const std::wstring& text,
        const Font& font,
        const Color& color,
        int x, int y, int w, int h)
    {
        DrawStringInt(text, font, color, x, y, w, h,
            CanvasGdiplus::DefaultCanvasTextAlignment());
    }

    void CanvasGdiplus::DrawStringInt(const std::wstring& text,
        const Font& font,
        const Color& color,
        const Rect& display_rect)
    {
        DrawStringInt(text, font, color, display_rect.x(), display_rect.y(),
            display_rect.width(), display_rect.height());
    }

    void CanvasGdiplus::DrawStringInt(const std::wstring& text,
        const Font& font,
        const Color& color,
        int x, int y, int w, int h,
        int flags)
    {
        DrawStringInt(text, font.GetNativeFont(), color, x, y, w, h, flags);
    }

    void CanvasGdiplus::TileImageInt(const Bitmap& bitmap,
        int x, int y, int w, int h)
    {
        TileImageInt(bitmap, 0, 0, x, y, w, h);
    }

    void CanvasGdiplus::TileImageInt(const Bitmap& bitmap,
        int src_x, int src_y, int dest_x, int dest_y, int w, int h)
    {
        if(!IntersectsClipRectInt(dest_x, dest_y, w, h))
        {
            return;
        }

        Gdiplus::Graphics* current = GetCurrentGraphics();
        if(!bitmap.IsNull())
        {
            Gdiplus::Bitmap* native_bitmap = bitmap.GetNativeBitmap();
            Gdiplus::ImageAttributes ia;
            ia.SetWrapMode(Gdiplus::WrapModeTile);
            Gdiplus::Rect rc_dest(dest_x, dest_y, w, h);
            current->DrawImage(native_bitmap,
                rc_dest, src_x, src_y,
                native_bitmap->GetWidth()-src_x,
                native_bitmap->GetHeight()-src_y,
                Gdiplus::UnitPixel, &ia);
        }
    }

    HDC CanvasGdiplus::BeginPlatformPaint()
    {
        DCHECK(mem_graphics_.get());

        Gdiplus::Matrix matrix;
        mem_graphics_->GetTransform(&matrix);
        Gdiplus::REAL elem[6];
        matrix.GetElements(elem);

        Gdiplus::Region region;
        mem_graphics_->GetClip(&region);
        HRGN hrgn = region.GetHRGN(mem_graphics_.get());

        HDC hdc = mem_graphics_->GetHDC();
        InitializeDC(hdc);

        XFORM xf = { elem[0], elem[1], elem[2], elem[3], elem[4], elem[5] };
        SetWorldTransform(hdc, &xf);

        SelectClipRgn(hdc, hrgn);
        DeleteObject(hrgn);

        return hdc;
    }

    void CanvasGdiplus::EndPlatformPaint(HDC dc)
    {
        DCHECK(mem_graphics_.get());

        mem_graphics_->ReleaseHDC(dc);
    }

    CanvasGdiplus* CanvasGdiplus::AsCanvasGdiplus()
    {
        return this;
    }

    const CanvasGdiplus* CanvasGdiplus::AsCanvasGdiplus() const
    {
        return this;
    }

    Gdiplus::Graphics* CanvasGdiplus::GetCurrentGraphics() const
    {
        Gdiplus::Graphics* current = mem_graphics_.get();
        if(layers_.size())
        {
            CanvasLayer* layer = layers_.top();
            DCHECK(layer);
            current = layer->mem_graphics.get();
        }

        DCHECK(current);

        return current;
    }

    bool CanvasGdiplus::IntersectsClipRectInt(int x, int y, int w, int h)
    {
        DCHECK(mem_graphics_.get());

        Gdiplus::Rect rc_clip;
        mem_graphics_->GetClipBounds(&rc_clip);
        return !!rc_clip.Intersect(Gdiplus::Rect(x, y, w, h));
    }

    // static
    void CanvasGdiplus::SizeStringInt(const std::wstring& text,
        const Font& font,
        int* width, int* height,
        int flags)
    {
        // Clamp the max amount of text we'll measure to 2K.  When the string is
        // actually drawn, it will be clipped to whatever size box is provided, and
        // the time to do that doesn't depend on the length being clipped off.
        const int kMaxStringLength = 2048 - 1; // So the trailing \0 fits in 2K.
        std::wstring clamped_string(text.substr(0, kMaxStringLength));

        if(*width == 0)
        {
            // If multi-line + character break are on, the computed width will be one
            // character wide (useless).  Furthermore, if in this case the provided text
            // contains very long "words" (substrings without a word-breaking point),
            // DrawText() can run extremely slowly (e.g. several seconds).  So in this
            // case, we turn character breaking off to get a more accurate "desired"
            // width and avoid the slowdown.
            if(flags & (Canvas::MULTI_LINE|Canvas::CHARACTER_BREAK))
            {
                flags &= ~Canvas::CHARACTER_BREAK;
            }

            // Weird undocumented behavior: if the width is 0, DoDrawText() won't
            // calculate a size at all.  So set it to 1, which it will then change.
            if(!text.empty())
            {
                *width = 1;
            }
        }
        RECT r = { 0, 0, *width, *height };

        HDC dc = GetDC(NULL);
        HFONT old_font = static_cast<HFONT>(SelectObject(dc, font.GetNativeFont()));
        DoDrawText(dc, clamped_string, &r,
            ComputeFormatFlags(flags, clamped_string)|DT_CALCRECT);
        SelectObject(dc, old_font);
        ReleaseDC(NULL, dc);

        *width = r.right;
        *height = r.bottom;
    }

    void CanvasGdiplus::DrawStringInt(const std::wstring& text,
        HFONT font,
        const Color& color,
        int x, int y, int w, int h,
        int flags)
    {
        if(!IntersectsClipRectInt(x, y, w, h))
        {
            return;
        }

        // Clamp the max amount of text we'll draw to 32K.  There seem to be bugs in
        // DrawText() if you e.g. ask it to character-break a no-whitespace string of
        // length > 43680 (for which it draws nothing), and since we clamped to 2K in
        // SizeStringInt() we're unlikely to be able to display this much anyway.
        const int kMaxStringLength = 32768 - 1;  // So the trailing \0 fits in 32K.
        std::wstring clamped_string(text.substr(0, kMaxStringLength));

        RECT text_bounds = { x, y, x+w, y+h };
        HDC dc = BeginPlatformPaint();
        SetBkMode(dc, TRANSPARENT);
        HFONT old_font = (HFONT)SelectObject(dc, font);
        SetTextColor(dc, color.ToCOLORREF());

        int f = ComputeFormatFlags(flags, clamped_string);
        DoDrawText(dc, clamped_string, &text_bounds, f);

        // Restore the old font. This way we don't have to worry if the caller
        // deletes the font and the DC lives longer.
        SelectObject(dc, old_font);
        EndPlatformPaint(dc);
    }


    Canvas* Canvas::CreateCanvas()
    {
        return new CanvasGdiplus;
    }

    Canvas* Canvas::CreateCanvas(int width, int height, bool is_opaque)
    {
        return new CanvasGdiplus(width, height, is_opaque);
    }


    class CanvasPaintWin : public CanvasGdiplusPaint, public CanvasPaint
    {
    public:
        CanvasPaintWin(HWND view) : CanvasGdiplusPaint(view) {}

        virtual bool IsValid() const
        {
            return isEmpty();
        }

        virtual Rect GetInvalidRect() const
        {
            return Rect(paintStruct().rcPaint);
        }

        virtual Canvas* AsCanvas()
        {
            return this;
        }
    };

    CanvasPaint* CanvasPaint::CreateCanvasPaint(HWND view)
    {
        return new CanvasPaintWin(view);
    }

} //namespace gfx