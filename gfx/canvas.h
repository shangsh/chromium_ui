
#ifndef __canvas_h__
#define __canvas_h__

#pragma once

#include <string>

#if PLATFORM_WINDOWS
#ifdef _WIN32
#include <windows.h>
#endif
#elif PLATFORM_MACOS
#include <CoreGraphics/CoreGraphics.h>
#endif

namespace gfx
{

    class Bitmap;
    class Brush;
    class CanvasGdiplus;
    class Color;
    class Font;
    class Point;
    class Rect;

#if PLATFORM_WINDOWS
    typedef HDC PlatformDC;
#elif PLATFORM_MACOS
    typedef CGContextRef PlatformDC;
#else
    typedef void* PlatformDC;
#endif

    class Canvas
    {
    public:
        // ָ��DrawStringInt������Ⱦ�ı��Ķ��뷽ʽ.
        enum
        {
            TEXT_ALIGN_LEFT = 1,
            TEXT_ALIGN_CENTER = 2,
            TEXT_ALIGN_RIGHT = 4,
            TEXT_VALIGN_TOP = 8,
            TEXT_VALIGN_MIDDLE = 16,
            TEXT_VALIGN_BOTTOM = 32,

            // ָ�������ı�.
            MULTI_LINE = 64,

            // ȱʡ�����DrawStringInt���ر���ǰ׺('&')�ַ�. Ҳ�����ַ���"&foo"��
            // ��Ⱦ��"&foo". ����Ⱦ���м���ǰ׺�ַ�����Դ�ı�ʱ, ǰ׺���Դ������»�
            // ��(SHOW_PREFIX)������ȫ����Ⱦ(HIDE_PREFIX).
            SHOW_PREFIX = 128,
            HIDE_PREFIX = 256,

            // ��ֹʡ�Ժ�.
            NO_ELLIPSIS = 512,

            // ָ�����ʿ��Ա������и�. ֻ����MULTI_LINEһ����.
            CHARACTER_BREAK = 1024,

            // ָʾDrawStringInt()ʹ��RTL��������Ⱦ�ı�. ����������û��Ҫ�������
            // ��־λ, ��Ϊ�ı��ķ�������Ϣ��������Unicode�ַ�����ʽǶ�����ַ�����.
            // �������������LTR, ���ǲ�����뷽�����ַ�, ��Ϊ��Щƽ̨(����û�а�װ
            // RTL�����Ӣ�İ�Windows XP)��֧����Щ�ַ�. ����, ��������LTRʱ, ���
            // ��־��������ȾRTL�����Ե��ı�.
            FORCE_RTL_DIRECTIONALITY = 2048,
        };

        virtual ~Canvas() {}

        // ����һ���յĻ���. ʹ��ǰ�����ʼ��.
        static Canvas* CreateCanvas();

        // �����ض���С�Ļ���.
        static Canvas* CreateCanvas(int width, int height, bool is_opaque);

        // ��ջ�ϱ���һ�ݻ�ͼ״̬�Ŀ���, ����Ե���Restore()֮ǰ, ���еĲ���
        // ������ݿ�����.
        virtual void Save() = 0;

        // �����ڵ���Save()��ָ���ͼ״̬. Restore()����Save()ʱ�ᷢ������.
        virtual void Restore() = 0;

        // ��Save()һ��, ֻ�ǻ�����һ��ͼ����, �ڵ���RestoreLayer()ʱ�뻭��
        // �ں�.
        // |layer_bounds|��ͼ���ڵ�ǰ����任�µķ�Χ.
        virtual void SaveLayer(uint8 alpha) = 0;
        virtual void SaveLayer(uint8 alpha, const Rect& layer_bounds) = 0;
        // �����ڵ���SaveLayer()��ָ���ͼ״̬. RestoreLayer()����SaveLayer()
        // ʱ�ᷢ������.
        virtual void RestoreLayer() = 0;

        // ������װ������������.
        // ����ü�Ϊ�շ���true.
        virtual bool ClipRectInt(int x, int y, int w, int h) = 0;

        // ������װ������������.
        virtual void TranslateInt(int x, int y) = 0;

        // ������װ������������.
        virtual void ScaleInt(int x, int y) = 0;

        // ���ض���ɫ���ָ������.
        virtual void FillRectInt(const Color& color,
            int x, int y, int w, int h) = 0;

        // ���ض���ˢ���ָ������.
        virtual void FillRectInt(const Brush& brush,
            int x, int y, int w, int h) = 0;

        // ���ض������ø�����ɫ���ƾ��ο�.
        //
        // ע��: �����ֻ����Ҫ����һ����, ��ʹ��DrawLineInt.
        virtual void DrawRectInt(const Color& color,
            int x, int y, int w, int h) = 0;

        // ���ض���ɫ������.
        virtual void DrawLineInt(const Color& color,
            int x1, int y1, int x2, int y2) = 0;

        // ���ض�λ�û���λͼ. λͼ�����Ͻ���Ⱦ���Ǹ�λ��.
        virtual void DrawBitmapInt(const Bitmap& bitmap, int x, int y) = 0;

        // ���Ʋ���λͼ���ض�λ��. src������Ӧλͼ������, ���Ƶ�dest���궨���
        // ����.
        //
        // ���Դ�Ŀ���ߺ�Ŀ�ĵĲ�һ��, ͼ�񽫻�����.
        virtual void DrawBitmapInt(const Bitmap& bitmap,
            int src_x, int src_y, int src_w, int src_h,
            int dest_x, int dest_y, int dest_w, int dest_h) = 0;

        // ���ض���ɫ�������ڸ�����λ�û����ı�. �ı�ˮƽ���������, ��ֱ����
        // ���ж���, �������вü�. ����ı�̫��, ���ȡ����ĩβ����'...'.
        virtual void DrawStringInt(const std::wstring& text,
            const Font& font,
            const Color& color,
            int x, int y, int w, int h) = 0;
        virtual void DrawStringInt(const std::wstring& text,
            const Font& font,
            const Color& color,
            const Rect& display_rect) = 0;

        // ���ض���ɫ�������ڸ�����λ�û����ı�. ���һ������ָ���ı���Ⱦ�ķ�ʽ.
        // ������TEXT_ALIGN_CENTER��TEXT_ALIGN_RIGHT��TEXT_ALIGN_LEFT�е�һ��.
        virtual void DrawStringInt(const std::wstring& text,
            const Font& font,
            const Color& color,
            int x, int y, int w, int h,
            int flags) = 0;

        // ���ƴ��Ļ�ɫ����������ʾ����.
        virtual void DrawFocusRect(int x, int y, int width, int height) = 0;

        // ���ض�����ƽ��ͼ��.
        virtual void TileImageInt(const Bitmap& bitmap,
            int x, int y, int w, int h) = 0;
        virtual void TileImageInt(const Bitmap& bitmap,
            int src_x, int src_y,
            int dest_x, int dest_y, int w, int h) = 0;

        // ���ر��ػ�ͼ��������ƽ̨��صĻ�ͼ����. ������EndPlatformPaint()��Ե���.
        virtual PlatformDC BeginPlatformPaint() = 0;

        // ��������ʹ�ñ��ػ�ͼ��������ƽ̨��صĻ�ͼ, �豸��BeginPlatformPaint()
        // ���ص�.
        virtual void EndPlatformPaint(PlatformDC dc) = 0;

        // ��ȡ�ײ�CanvasGdiplus�ķ���.
        virtual CanvasGdiplus* AsCanvasGdiplus() { return NULL; }
        virtual const CanvasGdiplus* AsCanvasGdiplus() const { return NULL; }
    };

    class CanvasPaint
    {
    public:
        virtual ~CanvasPaint() {}

        // ����һ������, ����ʱ���Ƶ�|view|. �����Ĵ�С����|view|�Ŀͻ���.
        static CanvasPaint* CreateCanvasPaint(HWND view);

        // ���������һ����Ҫ�ػ����Ч�����򷵻�true.
        virtual bool IsValid() const = 0;

        // ������Ч����.
        virtual Rect GetInvalidRect() const = 0;

        // ���صײ�Ļ���.
        virtual Canvas* AsCanvas() = 0;
    };

} //namespace gfx

#endif //__canvas_h__