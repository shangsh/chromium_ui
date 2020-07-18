
#ifndef __platform_bitmap_h__
#define __platform_bitmap_h__

#pragma once

#include "base/ref_counted.h"

namespace Gdiplus
{
    class Bitmap;
}

namespace gfx
{

    class Bitmap;

    class PlatformBitmap : public base::RefCounted<PlatformBitmap>
    {
    public:
        // ����ƽ̨��ص�PlatformBitmap.
        static PlatformBitmap* CreateFromNativeBitmap(
            Gdiplus::Bitmap* native_bitmap);

        // ���ر���λͼ����.
        virtual Gdiplus::Bitmap* GetNativeBitmap() const = 0;

        // ��ȡλͼ�Ŀ��.
        virtual int Width() const = 0;

        // ��ȡλͼ�ĸ߶�.
        virtual int Height() const = 0;

    protected:
        virtual ~PlatformBitmap() {}

    private:
        friend class base::RefCounted<PlatformBitmap>;
    };

} //namespace gfx

#endif //__platform_bitmap_h__