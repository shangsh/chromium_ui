#include "gfx/platform/canvas_win.h"
#include "gfx/bitmap.h"
#include "gfx/brush.h"
#include "gfx/canvas_gdiplus.h"
#include "gfx/color.h"
#include "gfx/font.h"
#include "gfx/rect.h"

namespace gfx
{

CanvasWin::CanvasWin()
    : hdc_(nullptr), old_font_(nullptr), saved_state_count_(0)
{
}

CanvasWin::CanvasWin(int width, int height, bool is_opaque)
    : hdc_(nullptr), old_font_(nullptr), saved_state_count_(0)
{
    HDC screen_dc = GetDC(nullptr);
    hdc_ = CreateCompatibleDC(screen_dc);
    ReleaseDC(nullptr, screen_dc);

    HBITMAP bitmap = CreateCompatibleBitmap(hdc_, width, height);
    SelectObject(hdc_, bitmap);
    DeleteObject(bitmap);

    if (!is_opaque)
    {
        SetBkMode(hdc_, TRANSPARENT);
    }
}

CanvasWin::~CanvasWin()
{
    if (hdc_)
    {
        DeleteDC(hdc_);
    }
}

void CanvasWin::Save()
{
    SaveDC(hdc_);
    saved_state_count_++;
}

void CanvasWin::Restore()
{
    if (saved_state_count_ > 0)
    {
        RestoreDC(hdc_, -1);
        saved_state_count_--;
    }
}

void CanvasWin::SaveLayer(uint8 alpha)
{
    SaveDC(hdc_);
    saved_state_count_++;
}

void CanvasWin::SaveLayer(uint8 alpha, const Rect& layer_bounds)
{
    SaveLayer(alpha);
}

void CanvasWin::RestoreLayer()
{
    Restore();
}

bool CanvasWin::ClipRectInt(int x, int y, int w, int h)
{
    return static_cast<bool>(IntersectClipRect(hdc_, x, y, x + w, y + h));
}

void CanvasWin::TranslateInt(int x, int y)
{
    OffsetWindowOrgEx(hdc_, x, y, nullptr);
}

void CanvasWin::ScaleInt(int x, int y)
{
    // Windows GDI doesn't support scaling directly
}

void CanvasWin::FillRectInt(const Color& color, int x, int y, int w, int h)
{
    HBRUSH brush = CreateSolidBrush(color.ToCOLORREF());
    FillRect(hdc_, &RECT{ x, y, x + w, y + h }, brush);
    DeleteObject(brush);
}

void CanvasWin::FillRectInt(const Brush& brush, int x, int y, int w, int h)
{
    // Use brush implementation
}

void CanvasWin::DrawRectInt(const Color& color, int x, int y, int w, int h)
{
    HPEN pen = CreatePen(PS_SOLID, 1, color.ToCOLORREF());
    HPEN old_pen = (HPEN)SelectObject(hdc_, pen);
    HBRUSH old_brush = (HBRUSH)SelectObject(hdc_, GetStockObject(NULL_BRUSH));
    Rectangle(hdc_, x, y, x + w, y + h);
    SelectObject(hdc_, old_pen);
    SelectObject(hdc_, old_brush);
    DeleteObject(pen);
}

void CanvasWin::DrawLineInt(const Color& color, int x1, int y1, int x2, int y2)
{
    HPEN pen = CreatePen(PS_SOLID, 1, color.ToCOLORREF());
    HPEN old_pen = (HPEN)SelectObject(hdc_, pen);
    MoveToEx(hdc_, x1, y1, nullptr);
    LineTo(hdc_, x2, y2);
    SelectObject(hdc_, old_pen);
    DeleteObject(pen);
}

void CanvasWin::DrawBitmapInt(const Bitmap& bitmap, int x, int y)
{
    // Implementation for bitmap drawing
}

void CanvasWin::DrawBitmapInt(const Bitmap& bitmap,
    int src_x, int src_y, int src_w, int src_h,
    int dest_x, int dest_y, int dest_w, int dest_h)
{
    // Implementation for scaled bitmap drawing
}

void CanvasWin::DrawStringInt(const std::wstring& text,
    const Font& font, const Color& color,
    int x, int y, int w, int h)
{
    DrawStringInt(text, font, color, x, y, w, h, TEXT_ALIGN_LEFT | TEXT_VALIGN_TOP);
}

void CanvasWin::DrawStringInt(const std::wstring& text,
    const Font& font, const Color& color,
    const Rect& display_rect)
{
    DrawStringInt(text, font, color,
        display_rect.x(), display_rect.y(),
        display_rect.width(), display_rect.height());
}

void CanvasWin::DrawStringInt(const std::wstring& text,
    const Font& font, const Color& color,
    int x, int y, int w, int h, int flags)
{
    HFONT hfont = font.GetNativeFont();
    HFONT old_font = (HFONT)SelectObject(hdc_, hfont);
    SetTextColor(hdc_, color.ToCOLORREF());

    UINT format = 0;
    if (flags & TEXT_ALIGN_CENTER)
        format |= DT_CENTER;
    else if (flags & TEXT_ALIGN_RIGHT)
        format |= DT_RIGHT;
    else
        format |= DT_LEFT;

    if (flags & TEXT_VALIGN_MIDDLE)
        format |= DT_VCENTER;
    else if (flags & TEXT_VALIGN_BOTTOM)
        format |= DT_BOTTOM;
    else
        format |= DT_TOP;

    if (flags & MULTI_LINE)
        format |= DT_WORDBREAK;
    else
        format |= DT_SINGLELINE;

    if (flags & SHOW_PREFIX)
        format |= DT_SHOWPREFIX;
    else
        format |= DT_HIDEPREFIX;

    if (!(flags & NO_ELLIPSIS))
        format |= DT_END_ELLIPSIS;

    RECT rect = { x, y, x + w, y + h };
    DrawText(hdc_, text.c_str(), -1, &rect, format);

    SelectObject(hdc_, old_font);
    DeleteObject(hfont);
}

void CanvasWin::DrawFocusRect(int x, int y, int width, int height)
{
    ::DrawFocusRect(hdc_, &RECT{ x, y, x + width, y + height });
}

void CanvasWin::TileImageInt(const Bitmap& bitmap, int x, int y, int w, int h)
{
    // Tile image implementation
}

void CanvasWin::TileImageInt(const Bitmap& bitmap,
    int src_x, int src_y,
    int dest_x, int dest_y, int w, int h)
{
    // Tile image with source
}

PlatformDC CanvasWin::BeginPlatformPaint()
{
    return hdc_;
}

void CanvasWin::EndPlatformPaint(PlatformDC dc)
{
    // No-op for memory DC
}

CanvasGdiplus* CanvasWin::AsCanvasGdiplus()
{
    return nullptr;
}

const CanvasGdiplus* CanvasWin::AsCanvasGdiplus() const
{
    return nullptr;
}

// CanvasPaintWin implementation
CanvasPaintWin::CanvasPaintWin(HWND hwnd)
    : canvas_(nullptr), hwnd_(hwnd)
{
    HDC dc = BeginPaint(hwnd_, &ps_);
    if (dc)
    {
        canvas_ = new CanvasWin();
        // Would need to attach to the DC
    }
}

CanvasPaintWin::~CanvasPaintWin()
{
    if (canvas_)
    {
        EndPaint(hwnd_, &ps_);
        delete canvas_;
    }
}

bool CanvasPaintWin::IsValid() const
{
    return canvas_ != nullptr;
}

Rect CanvasPaintWin::GetInvalidRect() const
{
    return Rect(ps_.rcPaint.left, ps_.rcPaint.top,
        ps_.rcPaint.right - ps_.rcPaint.left,
        ps_.rcPaint.bottom - ps_.rcPaint.top);
}

} // namespace gfx
