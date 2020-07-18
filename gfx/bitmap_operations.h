
#ifndef __bitmap_operations_h__
#define __bitmap_operations_h__

#pragma once

#include "bitmap.h"

namespace gfx
{

    class BitmapOperations
    {
    public:
        // 创建调试用的亮红色位图, 大小为32*32. 占位用, 表示加载失败,
        // 引起注意.
        static Bitmap CreateDebugBitmap();

        // 两张图像融合成一张图像. alpha参数指定第二个图像的透明度.
        // 图像格式必须是PixelFormat32bppARGB且大小一样.
        static Bitmap CreateBlendedBitmap(const Bitmap& first_bitmap,
            const Bitmap& second_bitmap, double alpha);

        // 通过把color和image混合生产一个按钮的背景图像, 然后再应用mask. 它是一个
        // 组合操作封装, 等价于先使用|color|绘制背景, 然后在上面贴上|image|, 最后
        // 对输出结果应用|mask|. 图像格式必须是PixelFormat32bppARGB.
        static Bitmap CreateButtonBackground(const Color& color,
            const Bitmap& image_bitmap, const Bitmap& mask_bitmap);

    private:
        BitmapOperations(); // 不允许创建实例.
    };

} //namespace gfx

#endif //__bitmap_operations_h__