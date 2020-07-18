
#ifndef __scoped_native_library_h__
#define __scoped_native_library_h__

#pragma once

#include "native_library.h"

class FilePath;

namespace base
{

    // ��װ�ֲ�NativeLibrary����, �Զ�����ж�ر��ض�̬��.
    class ScopedNativeLibrary
    {
    public:
        explicit ScopedNativeLibrary(const FilePath& library_path)
        {
            library_ = LoadNativeLibrary(library_path);
        }

        ~ScopedNativeLibrary()
        {
            if(library_)
            {
                UnloadNativeLibrary(library_);
            }
        }

        void* GetFunctionPointer(const char* function_name)
        {
            if(!library_)
            {
                return NULL;
            }
            return GetFunctionPointerFromNativeLibrary(library_, function_name);
        }

    private:
        NativeLibrary library_;

        DISALLOW_COPY_AND_ASSIGN(ScopedNativeLibrary);
    };

} //namespace base

#endif //__scoped_native_library_h__