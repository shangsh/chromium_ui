
#ifndef __canvas_paint_win_h__
#define __canvas_paint_win_h__

#pragma once

namespace gfx
{

    // CanvasPaintT��Windowsƽ̨WM_PAINT��Ϣ����ĸ�����. ����init/destruction
    // �зֱ������BeginPaint/EndPaint, �ᴴ����ȷ��С��λͼ�ͻ���, ����������
    // ����ƽ��. λͼ������ʱ�Զ��Ļ��Ƶ���Ļ.
    //
    // �ڻ�ͼ֮ǰ�Ƚϵ���isEmptyȷ���Ƿ��ж�����Ҫ����. �������ʱ���ǿյ�, λͼ
    // �������������־���. ��ʱ, �����߲�Ӧ�û�ͼ.
    //
    // ���, ����Ҫ��ô��:
    //     case WM_PAINT: {
    //       gfx::PlatformCanvasPaint canvas(hwnd);
    //       if(!canvas.isEmpty()) {
    //         ... paint to the canvas ...
    //       }
    //       return 0;
    //     }
    template<class T>
    class CanvasPaintT : public T
    {
    public:
        // ���캯���ٶ������ǲ�͸����.
        explicit CanvasPaintT(HWND hwnd) : hwnd_(hwnd), paint_dc_(NULL),
            for_paint_(true)
        {
            memset(&ps_, 0, sizeof(ps_));
            initPaint(true);
        }

        CanvasPaintT(HWND hwnd, bool opaque) : hwnd_(hwnd), paint_dc_(NULL),
            for_paint_(true)
        {
            memset(&ps_, 0, sizeof(ps_));
            initPaint(opaque);
        }

        // Ϊdc�ϵĻ�ͼ������һ��CanvasPaintT. ������BeginPaint/EndPaint.
        CanvasPaintT(HDC dc, bool opaque, int x, int y, int w, int h)
            : hwnd_(NULL), paint_dc_(dc), for_paint_(false)
        {
            memset(&ps_, 0, sizeof(ps_));
            ps_.rcPaint.left = x;
            ps_.rcPaint.right = x + w;
            ps_.rcPaint.top = y;
            ps_.rcPaint.bottom = y + h;
            init(opaque);
        }

        virtual ~CanvasPaintT()
        {
            if(!isEmpty())
            {
                // �ύ��ͼ����Ļ.
                DrawToHDC(paint_dc_, ps_.rcPaint.left, ps_.rcPaint.top, NULL);
            }
            if(for_paint_)
            {
                EndPaint(hwnd_, &ps_);
            }
        }

        // �����Ч���ǿյķ���true. ������������ȷ���Ƿ��ж�����Ҫ����
        bool isEmpty() const
        {
            return ps_.rcPaint.right-ps_.rcPaint.left==0 ||
                ps_.rcPaint.bottom-ps_.rcPaint.top==0;
        }

        // ���ڷ���Windows�Ļ�ͼ����, �ر��Ƕ��ڻ�ȡ��ͼ���η�Χ:
        // paintstruct().rcPaint.
        const PAINTSTRUCT& paintStruct() const
        {
            return ps_;
        }

        // ���ػ�ͼDC.
        HDC paintDC() const
        {
            return paint_dc_;
        }

    protected:
        HWND hwnd_;
        HDC paint_dc_;
        PAINTSTRUCT ps_;

    private:
        void initPaint(bool opaque)
        {
            paint_dc_ = BeginPaint(hwnd_, &ps_);

            init(opaque);
        }

        void init(bool opaque)
        {
            // �޸�: ����ClearType, ���ǿ�����Ҫ�ѻ�ͼ��Χ����һ������, �����߽����
            // ��ȷ��(ClearType�ı�����������й�ϵ). ֻ�����Ƕ����ε����ص���Ļ.
            int width = ps_.rcPaint.right - ps_.rcPaint.left;
            int height = ps_.rcPaint.bottom - ps_.rcPaint.top;
            if(!initialize(width, height, opaque))
            {
                // �����������.
                *(char*)0 = 0;
            }

            // ����εĻ���ת������Ļ����.
            TranslateInt(-ps_.rcPaint.left, -ps_.rcPaint.top);
        }

        // true��ʾ��������ΪBeginPaint������.
        const bool for_paint_;

        // ��ֹ�����͸�ֵ���캯��.
        CanvasPaintT(const CanvasPaintT&);
        CanvasPaintT& operator=(const CanvasPaintT&);
    };

} //namespace gfx

#endif //__canvas_paint_win_h__