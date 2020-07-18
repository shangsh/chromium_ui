
#ifndef __bitmap_operations_h__
#define __bitmap_operations_h__

#pragma once

#include "bitmap.h"

namespace gfx
{

    class BitmapOperations
    {
    public:
        // ���������õ�����ɫλͼ, ��СΪ32*32. ռλ��, ��ʾ����ʧ��,
        // ����ע��.
        static Bitmap CreateDebugBitmap();

        // ����ͼ���ںϳ�һ��ͼ��. alpha����ָ���ڶ���ͼ���͸����.
        // ͼ���ʽ������PixelFormat32bppARGB�Ҵ�Сһ��.
        static Bitmap CreateBlendedBitmap(const Bitmap& first_bitmap,
            const Bitmap& second_bitmap, double alpha);

        // ͨ����color��image�������һ����ť�ı���ͼ��, Ȼ����Ӧ��mask. ����һ��
        // ��ϲ�����װ, �ȼ�����ʹ��|color|���Ʊ���, Ȼ������������|image|, ���
        // ��������Ӧ��|mask|. ͼ���ʽ������PixelFormat32bppARGB.
        static Bitmap CreateButtonBackground(const Color& color,
            const Bitmap& image_bitmap, const Bitmap& mask_bitmap);

    private:
        BitmapOperations(); // ��������ʵ��.
    };

} //namespace gfx

#endif //__bitmap_operations_h__