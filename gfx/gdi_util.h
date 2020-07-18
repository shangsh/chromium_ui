
#ifndef __gdi_util_h__
#define __gdi_util_h__

#pragma once

#include <vector>
#include <windows.h>

#include "rect.h"

namespace gfx
{

    // ���ݸ�����λͼ�ߴ紴��һ��BITMAPINFOHEADER�ṹ��.
    void CreateBitmapHeader(int width, int height, BITMAPINFOHEADER* hdr);

    // // ���ݸ�����λͼ�ߴ��λ���һ��BITMAPINFOHEADER�ṹ��.
    void CreateBitmapHeaderWithColorDepth(int width, int height,
        int color_depth, BITMAPINFOHEADER* hdr);

    // ���ݸ�����λͼ�ߴ紴��һ��BITMAPV4HEADER�ṹ��. ֻ�е�����Ҫ͸����(
    // alphaͨ��)��ʱ����б�Ҫʹ��BMP V4. ��������AlphaMaskΪ0xff000000.
    void CreateBitmapV4Header(int width, int height, BITMAPV4HEADER* hdr);

    // ����һ���ڰ׵�BITMAPINFOHEADER.
    void CreateMonochromeBitmapHeader(int width, int height,
        BITMAPINFOHEADER* hdr);

    // ��hrgn�м�ȥcutouts.
    void SubtractRectanglesFromRegion(HRGN hrgn,
        const std::vector<gfx::Rect>& cutouts);

} //namespace gfx

#endif //__gdi_util_h__