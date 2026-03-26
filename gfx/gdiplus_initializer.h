
#ifndef __gdiplus_initializer_h__
#define __gdiplus_initializer_h__

#pragma once

#include <algorithm>
using std::min;
using std::max;

#ifdef _WIN32
#include <windows.h>
#endif
#ifdef _WIN32
#include <gdiplus.h>
#endif

#include "base/basic_types.h"
#include "base/non_thread_safe.h"

namespace gfx
{

#ifdef _WIN32
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
#else
    // Stub for non-Windows
    class GdiplusInitializer
    {
    public:
        GdiplusInitializer() {}
        ~GdiplusInitializer() {}
        bool Init() { return true; }
        void UnInit() {}
    };
#endif // _WIN32

} //namespace gfx

#endif //__gdiplus_initializer_h__