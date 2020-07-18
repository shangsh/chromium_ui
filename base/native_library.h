
#ifndef __native_library_h__
#define __native_library_h__

#pragma once

#include <windows.h>

#include "string16.h"

namespace base
{

    class FilePath;

    typedef HMODULE NativeLibrary;

    // 从磁盘加载本地库, 使用完后UnloadNativeLibrary释放.
    NativeLibrary LoadNativeLibrary(const FilePath& library_path);

    // 卸载本地库.
    void UnloadNativeLibrary(NativeLibrary library);

    // 获取函数指针.
    void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
        const char* name);

    // 返回跨平台的本地库全名. 例如:
    // "mylib"在Windows上返回"mylib.dll", 在Linux上返回"libmylib.so",
    // 在Mac上返回"mylib.dylib".
    string16 GetNativeLibraryName(const string16& name);

} //namespace base

#endif //__native_library_h__