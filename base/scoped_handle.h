
#ifndef __scoped_handle_h__
#define __scoped_handle_h__

#pragma once

#include <windows.h>

#include "logging.h"

namespace base
{

    // ���õĹ�����, �������ǹر�.
    // ��Ľӿں�ScopedStdioHandleһ��, ����һ��IsValid()����, ��ΪWindows
    // �ϵķǷ����������NULL����INVALID_HANDLE_VALUE(-1).
    //
    // ʾ��:
    //     ScopedHandle hfile(CreateFile(...));
    //     if(!hfile.Get())
    //       ...process error
    //     ReadFile(hfile.Get(), ...);
    //
    // ת�ƾ��������Ȩ:
    //     secret_handle_ = hfile.Take();
    //
    // ��ʽ�Ĺرվ��:
    //     hfile.Close();
    class ScopedHandle
    {
    public:
        ScopedHandle() : handle_(NULL) {}

        explicit ScopedHandle(HANDLE h) : handle_(NULL)
        {
            Set(h);
        }

        ~ScopedHandle()
        {
            Close();
        }

        // �÷������Դ�������INVALID_HANDLE_VALUE�ıȽ�, ��Ϊ������NULL
        // ��ʾ����.
        bool IsValid() const
        {
            return handle_ != NULL;
        }

        void Set(HANDLE new_handle)
        {
            Close();

            // Windows�ķǷ������ʾ�����ǲ�һ�µ�, ����ͳһʹ��NULL.
            if(new_handle != INVALID_HANDLE_VALUE)
            {
                handle_ = new_handle;
            }
        }

        HANDLE Get()
        {
            return handle_;
        }

        operator HANDLE() { return handle_; }

        HANDLE Take()
        {
            // ת������Ȩ.
            HANDLE h = handle_;
            handle_ = NULL;
            return h;
        }

        void Close()
        {
            if(handle_)
            {
                if(!CloseHandle(handle_))
                {
                    NOTREACHED();
                }
                handle_ = NULL;
            }
        }

    private:
        HANDLE handle_;
        DISALLOW_COPY_AND_ASSIGN(ScopedHandle);
    };

    template<class T>
    class ScopedGDIObject
    {
    public:
        ScopedGDIObject() : object_(NULL) {}
        explicit ScopedGDIObject(T object) : object_(object) {}

        ~ScopedGDIObject()
        {
            Close();
        }

        T Get()
        {
            return object_;
        }

        T Get() const
        {
            return object_;
        }

        void Set(T object)
        {
            if(object_ && object!=object_)
            {
                Close();
            }
            object_ = object;
        }

        ScopedGDIObject& operator=(T object)
        {
            Set(object);
            return *this;
        }

        T release()
        {
            T object = object_;
            object_ = NULL;
            return object;
        }

        operator T() { return object_; }

    private:
        void Close()
        {
            if(object_)
            {
                DeleteObject(object_);
            }
        }

        T object_;
        DISALLOW_COPY_AND_ASSIGN(ScopedGDIObject);
    };

    // ������HICON, ��Ϊ����HICON��Ҫ����DestroyIcon()������DeleteObject().
    template<>
    void ScopedGDIObject<HICON>::Close()
    {
        if(object_)
        {
            DestroyIcon(object_);
        }
    }

    // ���峣�õ�����.
    typedef ScopedGDIObject<HBITMAP> ScopedBitmap;
    typedef ScopedGDIObject<HRGN> ScopedRegion;
    typedef ScopedGDIObject<HFONT> ScopedHFONT;
    typedef ScopedGDIObject<HICON> ScopedHICON;

    // ֻ�����ڴ�CreateCompatibleDC���ص�HDCs. ��GetDC���ص�HDC, Ӧ��
    // ʹ��ReleaseDC.
    class ScopedHDC
    {
    public:
        ScopedHDC() : hdc_(NULL) {}
        explicit ScopedHDC(HDC h) : hdc_(h) {}

        ~ScopedHDC()
        {
            Close();
        }

        HDC Get()
        {
            return hdc_;
        }

        void Set(HDC h)
        {
            Close();
            hdc_ = h;
        }

        operator HDC() { return hdc_; }

    private:
        void Close()
        {
#ifdef NOGDI
            assert(false);
#else
            if(hdc_)
            {
                DeleteDC(hdc_);
            }
#endif //NOGDI
        }

        HDC hdc_;
        DISALLOW_COPY_AND_ASSIGN(ScopedHDC);
    };

    template<class T>
    class ScopedHGlobal
    {
    public:
        explicit ScopedHGlobal(HGLOBAL glob) : glob_(glob)
        {
            data_ = static_cast<T*>(GlobalLock(glob_));
        }
        ~ScopedHGlobal()
        {
            GlobalUnlock(glob_);
        }

        T* get() { return data_; }

        size_t Size() const { return GlobalSize(glob_); }

        T* operator->() const 
        {
            assert(data_ != 0);
            return data_;
        }

        T* release()
        {
            T* data = data_;
            data_ = NULL;
            return data;
        }

    private:
        HGLOBAL glob_;
        T* data_;

        DISALLOW_COPY_AND_ASSIGN(ScopedHGlobal);
    };

    class ScopedStdioHandle
    {
    public:
        ScopedStdioHandle() : handle_(NULL) {}

        explicit ScopedStdioHandle(FILE* handle)
            : handle_(handle) {}

        ~ScopedStdioHandle()
        {
            Close();
        }

        void Close()
        {
            if(handle_)
            {
                fclose(handle_);
                handle_ = NULL;
            }
        }

        FILE* get() const { return handle_; }

        FILE* Take()
        {
            FILE* temp = handle_;
            handle_ = NULL;
            return temp;
        }

        void Set(FILE* newhandle)
        {
            Close();
            handle_ = newhandle;
        }

    private:
        FILE* handle_;

        DISALLOW_COPY_AND_ASSIGN(ScopedStdioHandle);
    };

} //namespace base

#endif //__scoped_handle_h__