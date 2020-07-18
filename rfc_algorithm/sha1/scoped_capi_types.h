
#ifndef __scoped_capi_types_h__
#define __scoped_capi_types_h__

#pragma once

#include <windows.h>
#include <wincrypt.h>

#include "base/logging.h"

namespace base
{

    // CryptoAPI���ٺ�����������, ��CryptDestroyHash���ֵ��������ͷź���.
    template<typename CAPIHandle, BOOL (WINAPI *Destroyer)(CAPIHandle)>
    struct CAPIDestroyer
    {
        void operator()(CAPIHandle handle) const
        {
            if(handle)
            {
                BOOL ok = Destroyer(handle);
                DCHECK(ok);
            }
        }
    };

    // CryptoAPI�ر�/�ͷź�����������, ���еڶ���DWORD���͵Ĳ�����־λ.
    // ����CertCloseStore����CryptReleaseContext���ֺ���.
    template<typename CAPIHandle, BOOL (WINAPI *Destroyer)(CAPIHandle, DWORD),
        DWORD flags>
    struct CAPIDestroyerWithFlags
    {
        void operator()(CAPIHandle handle) const
        {
            if(handle)
            {
                BOOL ok = Destroyer(handle, flags);
                DCHECK(ok);
            }
        }
    };

    // ΪCryptoAPI�ļ��ܽ��ܺ�֤����֤����ṩscoped_ptr֧��. ���ھ������Ϊ
    // �������Ͷ�����ָ������, ���е�scoped�����scoped_ptr_malloc�޷�����.
    // �����ϵ�ͬ��scoped_ptr.
    template <class CAPIHandle, typename FreeProc>
    class ScopedCAPIHandle
    {
    public:
        explicit ScopedCAPIHandle(CAPIHandle handle=NULL) : handle_(handle) {}

        ~ScopedCAPIHandle()
        {
            free_(handle_);
        }

        void reset(CAPIHandle handle=NULL)
        {
            if(handle_ != handle)
            {
                free_(handle_);
                handle_ = handle;
            }
        }

        operator CAPIHandle() const { return handle_; }
        CAPIHandle get() const { return handle_; }

        CAPIHandle* receive()
        {
            CHECK(handle_ == NULL);
            return &handle_;
        }

        bool operator==(CAPIHandle handle) const
        {
            return handle_ == handle;
        }

        bool operator!=(CAPIHandle handle) const
        {
            return handle_ != handle;
        }

        void swap(ScopedCAPIHandle& b)
        {
            CAPIHandle tmp = b.handle_;
            b.handle_ = handle_;
            handle_ = tmp;
        }

        CAPIHandle release()
        {
            CAPIHandle tmp = handle_;
            handle_ = NULL;
            return tmp;
        }

    private:
        CAPIHandle handle_;
        static const FreeProc free_;

        DISALLOW_COPY_AND_ASSIGN(ScopedCAPIHandle);
    };

    template<class CH, typename FP>
    const FP ScopedCAPIHandle<CH, FP>::free_ = FP();

    template<class CH, typename FP>
    inline bool operator==(CH h, const ScopedCAPIHandle<CH, FP>& b)
    {
        return h == b.get();
    }

    template<class CH, typename FP>
    inline bool operator!=(CH h, const ScopedCAPIHandle<CH, FP>& b)
    {
        return h != b.get();
    }

    typedef ScopedCAPIHandle<HCRYPTPROV, CAPIDestroyerWithFlags<HCRYPTPROV,
        CryptReleaseContext, 0> > ScopedHCRYPTPROV;

    typedef ScopedCAPIHandle<HCRYPTKEY, CAPIDestroyer<HCRYPTKEY,
        CryptDestroyKey> > ScopedHCRYPTKEY;

    typedef ScopedCAPIHandle<HCRYPTHASH, CAPIDestroyer<HCRYPTHASH,
        CryptDestroyHash> > ScopedHCRYPTHASH;

} //namespace base

#endif //__scoped_capi_types_h__