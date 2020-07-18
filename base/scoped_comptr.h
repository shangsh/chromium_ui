
#ifndef __scoped_comptr_h__
#define __scoped_comptr_h__

#pragma once

#include <unknwn.h>

#include "logging.h"
#include "ref_counted.h"

namespace base
{

    // ģ��COM�ӿڵ�����ָ��. ʹ��scoped_refptr��Ϊ�����ṩ���ü�������,
    // �����IUnknown��һЩ�ض�����.
    template<class Interface, const IID* interface_id=&__uuidof(Interface)>
    class ScopedComPtr : public scoped_refptr<Interface>
    {
    public:
        // ����ģ��, ��ֹʹ����͸��ScopedComPtr����ýӿ�ָ���AddRef��(��)
        // Release.
        class BlockIUnknownMethods : public Interface
        {
        private:
            STDMETHOD(QueryInterface)(REFIID iid, void** object) = 0;
            STDMETHOD_(ULONG, AddRef)() = 0;
            STDMETHOD_(ULONG, Release)() = 0;
        };

        typedef scoped_refptr<Interface> ParentClass;

        ScopedComPtr() {}

        explicit ScopedComPtr(Interface* p) : ParentClass(p) {}

        ScopedComPtr(const ScopedComPtr<Interface, interface_id>& p)
            : ParentClass(p) {}

        ~ScopedComPtr()
        {
            // ����ָ����Ĵ�С����ָ�뱣��һ��.
            COMPILE_ASSERT(sizeof(ScopedComPtr<Interface, interface_id>) ==
                sizeof(Interface*), ScopedComPtrSize);
        }

        // ��ʽ�����ڲ������Release()����. ΪScopedComPtrʵ���ṩ���ù���.
        // ������IUnknown::Release�ȼ�, ��Ҫ��scoped_ptr::release()����.
        void Release()
        {
            if(ptr_ != NULL)
            {
                ptr_->Release();
                ptr_ = NULL;
            }
        }

        // ʹ�ڲ������뱾������벢������ָ��.
        Interface* Detach()
        {
            Interface* p = ptr_;
            ptr_ = NULL;
            return p;
        }

        // ����һ���Ѿ���addref�Ľӿ�ָ��.
        void Attach(Interface* p)
        {
            DCHECK(ptr_ == NULL);
            ptr_ = p;
        }

        // ���ؽӿ�ָ��ĵ�ַ.
        // ���ڽ����������(�ӹ�����Ȩ). ����ʹ��DCHECKs��֤��ǰֵ�Ƿ�ΪNULL.
        // �÷�: Foo(p.Receive());
        Interface** Receive()
        {
            DCHECK(ptr_==NULL) << "Object leak. Pointer must be NULL";
            return &ptr_;
        }

        template<class Query>
        HRESULT QueryInterface(Query** p)
        {
            DCHECK(p != NULL);
            DCHECK(ptr_ != NULL);
            // IUnknown�Ѿ���һ��ģ��汾��QueryInterface, ����iid������
            // ��������ʽ��. ����ֻ�������DCHECKs.
            return ptr_->QueryInterface(p);
        }

        // ��IIDû�����͹���ʱQI.
        HRESULT QueryInterface(const IID& iid, void** obj)
        {
            DCHECK(obj != NULL);
            DCHECK(ptr_ != NULL);
            return ptr_->QueryInterface(iid, obj);
        }

        // ��|other|��ѯ�ڲ�����ӿ�, ������other->QueryInterface�����Ĵ�����.
        HRESULT QueryFrom(IUnknown* object)
        {
            DCHECK(object != NULL);
            return object->QueryInterface(Receive());
        }

        // CoCreateInstance��װ.
        HRESULT CreateInstance(const CLSID& clsid, IUnknown* outer=NULL,
            DWORD context=CLSCTX_ALL)
        {
            DCHECK(ptr_ == NULL);
            HRESULT hr = ::CoCreateInstance(clsid, outer, context,
                *interface_id, reinterpret_cast<void**>(&ptr_));
            return hr;
        }

        // ��鱾������|other|�Ƿ���ͬ.
        bool IsSameObject(IUnknown* other)
        {
            if(!other && !ptr_)
            {
                return true;
            }

            if(!other || !ptr_)
            {
                return false;
            }

            ScopedComPtr<IUnknown> my_identity;
            QueryInterface(my_identity.Receive());

            ScopedComPtr<IUnknown> other_identity;
            other->QueryInterface(other_identity.Receive());

            return static_cast<IUnknown*>(my_identity) ==
                static_cast<IUnknown*>(other_identity);
        }

        // �ṩ�ӿڵ�ֱ�ӷ���. ����ʹ����һ�����õļ������Ʒ���IUknown��
        // ����, ���ⷢ����������Ĵ�������:
        //     ScopedComPtr<IUnknown> p(Foo());
        //     p->Release();
        //     ... ������������ִ��ʱ, ���ٴε���Release().
        // ���ܵ�����QueryInterface��ӵ�DCHECKs. �����㾲̬��ǿתScopedComPtr��
        // ����װ�Ľӿ�, �㻹�ǿ���ͨ���ӿ�ָ�������Щ����, ����һ�㲻Ҫ������.
        BlockIUnknownMethods* operator->() const
        {
            DCHECK(ptr_ != NULL);
            return reinterpret_cast<BlockIUnknownMethods*>(ptr_);
        }

        // ʹ�ø����operator=().
        using scoped_refptr<Interface>::operator=;

        static const IID& iid()
        {
            return *interface_id;
        }
    };

} //namespace base

#endif //__scoped_comptr_h__