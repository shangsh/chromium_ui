
#ifndef __bitmap_h__
#define __bitmap_h__

#pragma once

#include "base/ref_counted.h"

#include "color.h"

namespace Gdiplus
{
    class Bitmap;
}

namespace gfx
{

    class PlatformBitmap;

    class Bitmap
    {
    public:
        // ����ȱʡλͼ.
        Bitmap();

        // ��¡λͼ����.
        Bitmap(const Bitmap& other);
        Bitmap& operator=(const Bitmap& other);

        // �ӱ���λͼ����Bitmap����.
        explicit Bitmap(Gdiplus::Bitmap* native_bitmap);

        // ��PlatformBitmap���󹹽�Bitmap. Bitmap����ӹ�PlatformBitmap���������Ȩ.
        explicit Bitmap(PlatformBitmap* platform_bitmap);

        ~Bitmap();

        // ���ر���λͼ����.
        Gdiplus::Bitmap* GetNativeBitmap() const;

        // ��ȡ�ײ�ƽ̨ʵ��. ���Ը�����Ҫǿת������ʵ������.
        PlatformBitmap* platform_bitmap() const { return platform_bitmap_.get(); }

        // ����λͼ�����Ƿ�Ϊ��.
        bool IsNull() const;

        // ��ȡλͼ�Ŀ��.
        int Width() const;

        // ��ȡλͼ�ĸ߶�.
        int Height() const;

        // ��ȡָ��λ��������ɫֵ.
        Color GetPixel(int x, int y) const;

    private:
        // ƽ̨λͼ��װʵ��.
        scoped_refptr<PlatformBitmap> platform_bitmap_;
    };

} //namespace gfx

#endif //__bitmap_h__