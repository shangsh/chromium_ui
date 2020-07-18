
#ifndef __gdiplus_initializer_h__
#define __gdiplus_initializer_h__

#pragma once

#include <algorithm>
using std::min;
using std::max;

#include <windows.h>
#include <gdiplus.h>

#include "base/basic_types.h"
#include "base/non_thread_safe.h"

namespace gfx
{

    class GdiplusInitializer : public NonThreadSafe
    {
    public:
        GdiplusInitializer();
        ~GdiplusInitializer();

        // �����ֶ����ý��г�ʼ��.
        bool Init();

        // �����ֶ����ý���������. ��������֮ǰ���뱣֤���е�GDI+��Դ
        // ȫ���ͷ�, �˺����Դ�ͷŻᵼ�±���.
        void UnInit();

    private:
        DISALLOW_COPY_AND_ASSIGN(GdiplusInitializer);

        ULONG_PTR gdiplus_token_;
    };

} //namespace gfx

#endif //__gdiplus_initializer_h__