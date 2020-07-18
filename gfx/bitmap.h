
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
        // 创建缺省位图.
        Bitmap();

        // 克隆位图对象.
        Bitmap(const Bitmap& other);
        Bitmap& operator=(const Bitmap& other);

        // 从本地位图创建Bitmap对象.
        explicit Bitmap(Gdiplus::Bitmap* native_bitmap);

        // 用PlatformBitmap对象构建Bitmap. Bitmap对象接管PlatformBitmap对象的所有权.
        explicit Bitmap(PlatformBitmap* platform_bitmap);

        ~Bitmap();

        // 返回本地位图对象.
        Gdiplus::Bitmap* GetNativeBitmap() const;

        // 获取底层平台实现. 可以根据需要强转到具体实现类型.
        PlatformBitmap* platform_bitmap() const { return platform_bitmap_.get(); }

        // 本地位图对象是否为空.
        bool IsNull() const;

        // 获取位图的宽度.
        int Width() const;

        // 获取位图的高度.
        int Height() const;

        // 获取指定位置像素颜色值.
        Color GetPixel(int x, int y) const;

    private:
        // 平台位图封装实现.
        scoped_refptr<PlatformBitmap> platform_bitmap_;
    };

} //namespace gfx

#endif //__bitmap_h__