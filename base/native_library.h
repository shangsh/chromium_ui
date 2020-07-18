
#ifndef __native_library_h__
#define __native_library_h__

#pragma once

#include <windows.h>

#include "string16.h"

namespace base
{

    class FilePath;

    typedef HMODULE NativeLibrary;

    // �Ӵ��̼��ر��ؿ�, ʹ�����UnloadNativeLibrary�ͷ�.
    NativeLibrary LoadNativeLibrary(const FilePath& library_path);

    // ж�ر��ؿ�.
    void UnloadNativeLibrary(NativeLibrary library);

    // ��ȡ����ָ��.
    void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
        const char* name);

    // ���ؿ�ƽ̨�ı��ؿ�ȫ��. ����:
    // "mylib"��Windows�Ϸ���"mylib.dll", ��Linux�Ϸ���"libmylib.so",
    // ��Mac�Ϸ���"mylib.dylib".
    string16 GetNativeLibraryName(const string16& name);

} //namespace base

#endif //__native_library_h__