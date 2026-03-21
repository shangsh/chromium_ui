#ifndef GFX_PLATFORM_CANVAS_WIN_H_
#define GFX_PLATFORM_CANVAS_WIN_H_

#pragma once

#include "gfx/canvas.h"
#include <windows.h>

namespace gfx
{

class Bitmap;
class Brush;
class CanvasGdiplus;
class Color;
class Font;
class Point;
class Rect;

class CanvasWin : public Canvas
{
public:
    CanvasWin();
    CanvasWin(int width, int height, bool is_opaque);
    virtual ~CanvasWin();

    // Canvas interface
    void Save() override;
    void Restore() override;
    void SaveLayer(uint8 alpha) override;
    void SaveLayer(uint8 alpha, const Rect& layer_bounds) override;
    void RestoreLayer() override;

    bool ClipRectInt(int x, int y, int w, int h) override;
    void TranslateInt(int x, int y) override;
    void ScaleInt(int x, int y) override;

    void FillRectInt(const Color& color, int x, int y, int w, int h) override;
    void FillRectInt(const Brush& brush, int x, int y, int w, int h) override;
    void DrawRectInt(const Color& color, int x, int y, int w, int h) override;
    void DrawLineInt(const Color& color, int x1, int y1, int x2, int y2) override;

    void DrawBitmapInt(const Bitmap& bitmap, int x, int y) override;
    void DrawBitmapInt(const Bitmap& bitmap,
        int src_x, int src_y, int src_w, int src_h,
        int dest_x, int dest_y, int dest_w, int dest_h) override;

    void DrawStringInt(const std::wstring& text,
        const Font& font, const Color& color,
        int x, int y, int w, int h) override;
    void DrawStringInt(const std::wstring& text,
        const Font& font, const Color& color,
        const Rect& display_rect) override;
    void DrawStringInt(const std::wstring& text,
        const Font& font, const Color& color,
        int x, int y, int w, int h, int flags) override;

    void DrawFocusRect(int x, int y, int width, int height) override;

    void TileImageInt(const Bitmap& bitmap, int x, int y, int w, int h) override;
    void TileImageInt(const Bitmap& bitmap,
        int src_x, int src_y,
        int dest_x, int dest_y, int w, int h) override;

    HDC BeginPlatformPaint() override;
    void EndPlatformPaint(HDC dc) override;

    CanvasGdiplus* AsCanvasGdiplus() override;
    const CanvasGdiplus* AsCanvasGdiplus() const override;

    // Access to internal HDC
    HDC GetHDC() const { return hdc_; }
    void ReleaseHDC(HDC hdc) { /* Windows handles this */ }

private:
    HDC hdc_;
    HGDIOBJ old_font_;
    int saved_state_count_;
};

// CanvasPaint for Windows
class CanvasPaintWin : public CanvasPaint
{
public:
    explicit CanvasPaintWin(HWND hwnd);
    virtual ~CanvasPaintWin();

    bool IsValid() const override;
    Rect GetInvalidRect() const override;
    Canvas* AsCanvas() override { return canvas_; }

private:
    CanvasWin* canvas_;
    HWND hwnd_;
    PAINTSTRUCT ps_;
};

} // namespace gfx

#endif // GFX_PLATFORM_CANVAS_WIN_H_
