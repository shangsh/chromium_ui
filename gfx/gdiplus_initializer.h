
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

        // 必须手动调用进行初始化.
        bool Init();

        // 必须手动调用进行清理工作. 函数调用之前必须保证所有的GDI+资源
        // 全部释放, 此后的资源释放会导致崩溃.
        void UnInit();

    private:
        DISALLOW_COPY_AND_ASSIGN(GdiplusInitializer);

        ULONG_PTR gdiplus_token_;
    };

} //namespace gfx

#endif //__gdiplus_initializer_h__