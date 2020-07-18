
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
        // ����һ���յ�Canvas. ��������ʹ��ǰ�����ʼ��.
        CanvasGdiplus();

        CanvasGdiplus(int width, int height, bool is_opaque);

        virtual ~CanvasGdiplus();

        // �����ʹ�õ��������޲����Ĺ��캯��, ���ǵڶ�����ʼ������.
        bool initialize(int width, int height, bool is_opaque);

        // �������ṩ��������ı�����ĳߴ�. ���Ե�����͸�����Ӧ�ı�, ������Ҫ
        // ���ӸߺͿ�. ����֧�ֶ����ı�.
        static void SizeStringInt(const std::wstring& text,
            const Font& font,
            int* width, int* height,
            int flags);

        // ���ڱ���ϵͳ���Եķ�����, ����gfx::CanvasGdiplus�����ı�ʱʹ�õ�ȱʡ�ı�
        // ���뷽ʽ. gfx::Canvas::DrawStringInt��ûָ�����뷽ʽʱ����ñ�����.
        //
        // ����gfx::Canvas::TEXT_ALIGN_LEFT��gfx::Canvas::TEXT_ALIGN_RIGHT֮һ.
        static int DefaultCanvasTextAlignment();

        // ��ʼ��ȱʡ���豸���������ú���ɫ.
        static void InitializeDC(HDC context);

        // �ø�����ɫ���ƴ���1���ع�Ȧ���ı�. �������ClearType��һ����קͼ���
        // ͸��λͼ��. ��קͼ���͸����ֻ��1-bit, ��˲����κ�ģ����.
        void DrawStringWithHalo(const std::wstring& text,
            const Font& font,
            const Color& text_color,
            const Color& halo_color,
            int x, int y, int w, int h,
            int flags);

        // ������ɫ.
        void Clear(const Color& color);

        // ��ȡ�������ݳ�һ��λͼ.
        Bitmap ExtractBitmap() const;

        // ���Ƶ�DC��.
        void DrawToHDC(HDC hdc, int x, int y, const RECT* src_rect);

        // ��������Canvas:
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
        // ��ȡ��ǰ��ͼ��ͼ���豸.
        Gdiplus::Graphics* GetCurrentGraphics() const;

        // ���Ը����ľ����Ƿ��뵱ǰ�ü����ཻ.
        bool IntersectsClipRectInt(int x, int y, int w, int h);

        // ���ض���ɫ�������ڸ�����λ�û����ı�. �ı�ˮƽ���������, ��ֱ����
        // ���ж���, �������вü�. ����ı�̫��, ���ȡ����ĩβ���'...'.
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