
#ifndef __weak_ptr_h__
#define __weak_ptr_h__

#include "ref_counted.h"

#pragma once

#include "logging.h"
#include "non_thread_safe.h"

// ������������ͬһ����, ϣ�����������������ڲ������ö������Ƶ�ʱ�����
// ʹ����ָ��. ����֮, ���ڲ������ü��������.
//
// ��ָ�������һ��ѡ������ù������ӵ���������ö���, �ڹ������������ʱ��
// �����������ö��󷽷��Զ�������. ���ö���������ʱ��Ҳ��Ҫ֪ͨ����������
// �������ö������Ƴ�. ���ַ�������, ����һЩ����.
//
// ʾ��:
//
//    class Controller : public SupportsWeakPtr<Controller> {
//      public:
//      void SpawnWorker() { Worker::StartNew(AsWeakPtr()); }
//      void WorkComplete(const Result& result) { ... }
//    };
//
//    class Worker {
//    public:
//      static void StartNew(const WeakPtr<Controller>& controller) {
//        Worker* worker = new Worker(controller);
//        // Kick off asynchronous processing...
//      }
//    private:
//      Worker(const WeakPtr<Controller>& controller)
//        : controller_(controller) {}
//      void DidCompleteAsynchronousProcessing(const Result& result) {
//        if(controller_)
//          controller_->WorkComplete(result);
//      }
//      WeakPtr<Controller> controller_;
//    };
//
// �����������, �û�����һ��Controller����, ���ö��SpawnWorker, Ȼ��������
// �������֮ǰ����Controller����. ����Worker��ֻ��ӵ��Controller����ָ��,
// Controller�����Ժ���ָ��Ľ����ò������Ұָ��.
//
// ����: ��ָ�벻���̰߳�ȫ��!!! WeakPtr����ֻ���ڴ������߳���ʹ��.

namespace base
{

    // ��Щ����WeakPtr��ʵ�ֲ���, ��Ҫֱ��ʹ��.
    class WeakReference
    {
    public:
        class Flag : public RefCounted<Flag>, public NonThreadSafe
        {
        public:
            Flag(Flag** handle);
            ~Flag();

            void AddRef() const;
            void Release() const;
            void Invalidate() { handle_ = NULL; }
            bool is_valid() const { return handle_ != NULL; }

            void DetachFromThread() { NonThreadSafe::DetachFromThread(); }

        private:
            Flag** handle_;
        };

        WeakReference();
        WeakReference(Flag* flag);
        ~WeakReference();

        bool is_valid() const;

    private:
        scoped_refptr<Flag> flag_;
    };

    class WeakReferenceOwner
    {
    public:
        WeakReferenceOwner();
        ~WeakReferenceOwner();

        WeakReference GetRef() const;

        bool HasRefs() const
        {
            return flag_ != NULL;
        }

        void Invalidate();

        // �������������������һ���߳���ʹ��.
        void DetachFromThread()
        {
            if(flag_) flag_->DetachFromThread();
        }

    private:
        mutable WeakReference::Flag* flag_;
    };

    // �򻯲�ͬ���͵�WeakPtr��������, �������ref_������public. WeakPtr<T>
    // ����ֱ�ӷ���WeakPtr<U>��˽�г�Ա, �ʻ�����ref_�ķ��ʿ�����Ϊ�ܱ�����.
    class WeakPtrBase
    {
    public:
        WeakPtrBase();
        ~WeakPtrBase();

    protected:
        WeakPtrBase(const WeakReference& ref);

        WeakReference ref_;
    };

    template<typename T> class SupportsWeakPtr;
    template<typename T> class WeakPtrFactory;

    // WeakPtr��ӵ��|T*|��������.
    //
    // ��ʹ����������һ���ָ��, ���ö����Ա����ǰʼ����Ҫ�ж��Ƿ�Ϊ��, �ײ�
    // ��������ٻᵼ��������ָ����Ч.
    //
    // ʾ��:
    //
    //     class Foo { ... };
    //     WeakPtr<Foo> foo;
    //     if(foo)
    //       foo->method();
    template<typename T>
    class WeakPtr : public WeakPtrBase
    {
    public:
        WeakPtr() : ptr_(NULL) {}

        // ���U��T, �����U��T������ת��.
        template<typename U>
        WeakPtr(const WeakPtr<U>& other) : WeakPtrBase(other), ptr_(other.get()) {}

        T* get() const { return ref_.is_valid() ? ptr_ : NULL; }
        operator T*() const { return get(); }

        T* operator*() const
        {
            DCHECK(get() != NULL);
            return *get();
        }
        T* operator->() const
        {
            DCHECK(get() != NULL);
            return get();
        }

        void reset()
        {
            ref_ = WeakReference();
            ptr_ = NULL;
        }

    private:
        friend class SupportsWeakPtr<T>;
        friend class WeakPtrFactory<T>;

        WeakPtr(const WeakReference& ref, T* ptr)
            : WeakPtrBase(ref), ptr_(ptr) {}

        // ָ��ֻ����ref_.is_valid()Ϊtrue��ʱ��Ϸ�, ����ֵ��δ�����(����NULL).
        T* ptr_;
    };

    // ������SupportsWeakPtr����, ���Ա�¶�Լ�����ָ��. ����Ҫ�����������Լ�����
    // ָ��ʱ, ����Դ�SupportsWeakPtr����, ����Ĺ��캯���в���Ҫ��ʼ����.
    template<class T>
    class SupportsWeakPtr
    {
    public:
        SupportsWeakPtr() {}

        WeakPtr<T> AsWeakPtr()
        {
            return WeakPtr<T>(weak_reference_owner_.GetRef(), static_cast<T*>(this));
        }

        // �������������������һ���߳���ʹ��.
        void DetachFromThread()
        {
            weak_reference_owner_.DetachFromThread();
        }

    private:
        WeakReferenceOwner weak_reference_owner_;
        DISALLOW_COPY_AND_ASSIGN(SupportsWeakPtr);
    };

    // �����ѡ�����WeakPtrFactory���ڿ����Ƿ�¶�Լ�����ָ��, ���ڲ�ʵ����Ҫ
    // ��ָ���ʱ��ǳ�����. ����Ժ�ԭʼ�������ʹ��, ���������WeakPtrFactory<bool>
    // ��һ��bool�ĵ�������.
    template<class T>
    class WeakPtrFactory
    {
    public:
        explicit WeakPtrFactory(T* ptr) : ptr_(ptr) {}

        WeakPtr<T> GetWeakPtr()
        {
            return WeakPtr<T>(weak_reference_owner_.GetRef(), ptr_);
        }

        // ����ʹ������ָ��Ƿ�.
        void InvalidateWeakPtrs()
        {
            weak_reference_owner_.Invalidate();
        }

        // �ж��Ƿ������ָ��.
        bool HasWeakPtrs() const
        {
            return weak_reference_owner_.HasRefs();
        }

    private:
        WeakReferenceOwner weak_reference_owner_;
        T* ptr_;

        DISALLOW_IMPLICIT_CONSTRUCTORS(WeakPtrFactory);
    };

} //namespace base

#endif //__weak_ptr_h__