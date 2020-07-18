
#ifndef __canvas_gdiplus_h__
#define __canvas_gdiplus_h__

#pragma once

#include <windows.h>

#include <stack>

#include "base/basic_types.h"
#include "base/scoped_ptr.h"

#include "bitmap.h"
#include "canvas.h"
#include "canvas_paint_win.h"
#include "rect.h"

namespace gfx
{

    class CanvasGdiplus : public Canvas
    {
    public:
        // 创建一个空的Canvas. 调用者在使用前必须初始化.
        CanvasGdiplus();

        CanvasGdiplus(int width, int height, bool is_opaque);

        virtual ~CanvasGdiplus();

        // 如果你使用的是上面无参数的构造函数, 这是第二步初始化工作.
        bool initialize(int width, int height, bool is_opaque);

        // 计算用提供字体绘制文本所需的尺寸. 尝试调整宽和高来适应文本, 根据需要
        // 增加高和宽. 方法支持多行文本.
        static void SizeStringInt(const std::wstring& text,
            const Font& font,
            int* width, int* height,
            int flags);

        // 基于本地系统语言的方向性, 返回gfx::CanvasGdiplus绘制文本时使用的缺省文本
        // 对齐方式. gfx::Canvas::DrawStringInt在没指定对齐方式时会调用本函数.
        //
        // 返回gfx::Canvas::TEXT_ALIGN_LEFT、gfx::Canvas::TEXT_ALIGN_RIGHT之一.
        static int DefaultCanvasTextAlignment();

        // 初始化缺省的设备环境的设置和颜色.
        static void InitializeDC(HDC context);

        // 用给定颜色绘制带有1像素光圈的文本. 允许绘制ClearType到一个拖拽图像的
        // 透明位图上. 拖拽图像的透明度只有1-bit, 因此不做任何模糊化.
        void DrawStringWithHalo(const std::wstring& text,
            const Font& font,
            const Color& text_color,
            const Color& halo_color,
            int x, int y, int w, int h,
            int flags);

        // 清理背景色.
        void Clear(const Color& color);

        // 提取画布内容成一个位图.
        Bitmap ExtractBitmap() const;

        // 绘制到DC上.
        void DrawToHDC(HDC hdc, int x, int y, const RECT* src_rect);

        // 覆盖重载Canvas:
        virtual void Save();
        virtual void Restore();
        virtual void SaveLayer(uint8 alpha);
        virtual void SaveLayer(uint8 alpha, const Rect& layer_bounds);
        virtual void RestoreLayer();
        virtual bool ClipRectInt(int x, int y, int w, int h);
        virtual void TranslateInt(int x, int y);
        virtual void ScaleInt(int x, int y);
        virtual void FillRectInt(const Color& color, int x, int y, int w, int h);
        virtual void FillRectInt(const Brush& brush, int x, int y, int w, int h);
        virtual void DrawRectInt(const Color& color, int x, int y, int w, int h);
        virtual void DrawLineInt(const Color& color, int x1, int y1, int x2, int y2);
        virtual void DrawBitmapInt(const Bitmap& bitmap, int x, int y);
        virtual void DrawBitmapInt(const Bitmap& bitmap,
            int src_x, int src_y, int src_w, int src_h,
            int dest_x, int dest_y, int dest_w, int dest_h);
        virtual void DrawStringInt(const std::wstring& text,
            const Font& font,
            const Color& color,
            int x, int y, int w, int h);
        virtual void DrawStringInt(const std::wstring& text,
            const Font& font,
            const Color& color,
            const Rect& display_rect);
        virtual void DrawStringInt(const std::wstring& text,
            const Font& font,
            const Color& color,
            int x, int y, int w, int h,
            int flags);
        virtual void DrawFocusRect(int x, int y, int width, int height);
        virtual void TileImageInt(const Bitmap& bitmap,
            int x, int y, int w, int h);
        virtual void TileImageInt(const Bitmap& bitmap,
            int src_x, int src_y,
            int dest_x, int dest_y, int w, int h);
        virtual HDC BeginPlatformPaint();
        virtual void EndPlatformPaint(HDC dc);
        virtual CanvasGdiplus* AsCanvasGdiplus();
        virtual const CanvasGdiplus* AsCanvasGdiplus() const;

    private:
        // 获取当前绘图的图形设备.
        Gdiplus::Graphics* GetCurrentGraphics() const;

        // 测试给定的矩形是否与当前裁剪区相交.
        bool IntersectsClipRectInt(int x, int y, int w, int h);

        // 用特定颜色、字体在给定的位置绘制文本. 文本水平方向左对齐, 垂直方向
        // 居中对齐, 在区域中裁剪. 如果文本太大, 会截取并在末尾添加'...'.
        void DrawStringInt(const std::wstring& text,
            HFONT font,
            const Color& color,
            int x, int y, int w, int h,
            int flags);

        scoped_ptr<Gdiplus::Graphics> mem_graphics_;
        std::stack<Gdiplus::GraphicsState> states_;
        struct CanvasLayer
        {
            scoped_ptr<Gdiplus::Graphics> mem_graphics;
            Bitmap mem_bitmap;
            Rect bounds;
            uint8 alpha;
        };
        std::stack<CanvasLayer*> layers_;
        Bitmap mem_bitmap_;

        DISALLOW_COPY_AND_ASSIGN(CanvasGdiplus);
    };

    typedef CanvasPaintT<CanvasGdiplus> CanvasGdiplusPaint;

} //namespace gfx

#endif //__canvas_gdiplus_h__