
#ifndef __ref_counted_h__
#define __ref_counted_h__

#pragma once

#include "atomic_ref_count.h"

namespace base
{

    class RefCountedBase
    {
    public:
        static bool ImplementsThreadSafeReferenceCounting() { return false; }
        bool HasOneRef() const { return ref_count_ == 1; }

    protected:
        RefCountedBase();
        ~RefCountedBase();

        void AddRef() const;

        // ����ɾ���Լ�ʱ����true.
        bool Release() const;

    private:
        mutable int ref_count_;
#ifndef NDEBUG
        mutable bool in_dtor_;
#endif

        DISALLOW_COPY_AND_ASSIGN(RefCountedBase);
    };

    class RefCountedThreadSafeBase
    {
    public:
        static bool ImplementsThreadSafeReferenceCounting() { return true; }
        bool HasOneRef() const;

    protected:
        RefCountedThreadSafeBase();
        ~RefCountedThreadSafeBase();

        void AddRef() const;

        // ����ɾ���Լ�ʱ����true.
        bool Release() const;

    private:
        mutable AtomicRefCount ref_count_;
#ifndef NDEBUG
        mutable bool in_dtor_;
#endif

        DISALLOW_COPY_AND_ASSIGN(RefCountedThreadSafeBase);
    };

    // ���ü�����Ļ���. ������ʹ������������չ:
    //
    //     class MyFoo : public base::RefCounted<MyFoo> {
    //      ...
    //      private:
    //       friend class base::RefCounted<MyFoo>;
    //       ~MyFoo();
    //     };
    //
    // ��������ʼ����private��, ���ⲻС��ֱ�ӵ���delete����.
    template<class T>
    class RefCounted : public RefCountedBase
    {
    public:
        RefCounted() {}
        ~RefCounted() {}

        void AddRef() const
        {
            RefCountedBase::AddRef();
        }

        void Release() const
        {
            if(RefCountedBase::Release())
            {
                delete static_cast<const T*>(this);
            }
        }

    private:
        DISALLOW_COPY_AND_ASSIGN(RefCounted<T>);
    };

    template<class T, typename Traits> class RefCountedThreadSafe;

    // RefCountedThreadSafe<T>ȱʡ������. �����ü���Ϊ0ʱɾ������.
    // ���ؿ���ʵ���������߳�ɾ�������.
    template<typename T>
    struct DefaultRefCountedThreadSafeTraits
    {
        static void Destruct(T* x)
        {
            // ͨ��RefCountedThreadSafeɾ��, ��������ֻ��Ҫ����RefCountedThreadSafe
            // Ϊ��Ԫ, ����������ṹ��.
            RefCountedThreadSafe<T,
                DefaultRefCountedThreadSafeTraits>::DeleteInternal(x);
        }
    };

    // �̰߳�ȫ�ɱ��RefCounted<T>.
    //
    //     class MyFoo : public base::RefCountedThreadSafe<MyFoo> {
    //      ...
    //     };
    template<class T, typename Traits=DefaultRefCountedThreadSafeTraits<T> >
    class RefCountedThreadSafe : public RefCountedThreadSafeBase
    {
    public:
        RefCountedThreadSafe() {}
        ~RefCountedThreadSafe() {}

        void AddRef()
        {
            RefCountedThreadSafeBase::AddRef();
        }

        void Release()
        {
            if(RefCountedThreadSafeBase::Release())
            {
                Traits::Destruct(static_cast<T*>(this));
            }
        }

    private:
        friend struct DefaultRefCountedThreadSafeTraits<T>;
        static void DeleteInternal(T* x) { delete x; }

        DISALLOW_COPY_AND_ASSIGN(RefCountedThreadSafe);
    };

    // ��װһЩ��������ʵ��scoped_refptrs<>.
    template<typename T>
    class RefCountedData : public base::RefCounted< base::RefCountedData<T> >
    {
    public:
        RefCountedData() : data() {}
        RefCountedData(const T& in_value) : data(in_value) {}

        T data;
    };

} //namespace base

// ����ָ�����ڶ�������ü���. ʹ������಻��Ҫ�ֶ��ĵ���AddRef��Release, ����
// ���������ǵ���Release���µ��ڴ�й©. �÷�ʾ��:
//
//     class MyFoo : public RefCounted<MyFoo> {
//      ...
//     };
//
//     void some_function() {
//       scoped_refptr<MyFoo> foo = new MyFoo();
//       foo->Method(param);
//       // |foo| is released when this function returns
//     }
//
//     void some_other_function() {
//       scoped_refptr<MyFoo> foo = new MyFoo();
//       ...
//       foo = NULL;  // explicitly releases |foo|
//       ...
//       if(foo)
//         foo->Method(param);
//     }
//
// ���������չʾ��scoped_refptr<T>����Ϊ����һ��ָ��, ����scoped_refptr<T>
// ������Ի�������, ����:
//
//     {
//       scoped_refptr<MyFoo> a = new MyFoo();
//       scoped_refptr<MyFoo> b;
//
//       b.swap(a);
//       // now, |b| references the MyFoo object, and |a| references NULL.
//     }
//
// Ҫ��|a|��|b|����ͬһ��MyFoo����, ����ֱ��ʹ�ø�ֵ����:
//
//     {
//       scoped_refptr<MyFoo> a = new MyFoo();
//       scoped_refptr<MyFoo> b;
//
//       b = a;
//       // now, |a| and |b| each own a reference to the same MyFoo object.
//     }
template<class T>
class scoped_refptr
{
public:
    scoped_refptr() : ptr_(NULL) {}

    scoped_refptr(T* p) : ptr_(p)
    {
        if(ptr_)
        {
            ptr_->AddRef();
        }
    }

    scoped_refptr(const scoped_refptr<T>& r) : ptr_(r.ptr_)
    {
        if(ptr_)
        {
            ptr_->AddRef();
        }
    }

    template<typename U>
    scoped_refptr(const scoped_refptr<U>& r) : ptr_(r.get())
    {
        if(ptr_)
        {
            ptr_->AddRef();
        }
    }

    ~scoped_refptr()
    {
        if(ptr_)
        {
            ptr_->Release();
        }
    }

    T* get() const { return ptr_; }
    operator T*() const { return ptr_; }
    T* operator->() const { return ptr_; }

    // �ͷ�ָ��.
    // ���ض���ǰӵ�е�ָ��. ���ָ��ΪNULL, ����NULL.
    // ������ɺ�, ����ӵ��һ��NULLָ��, ����ӵ���κζ���.
    T* release()
    {
        T* retVal = ptr_;
        ptr_ = NULL;
        return retVal;
    }

    scoped_refptr<T>& operator=(T* p)
    {
        // �ȵ���AddRef, �������Ҹ�ֵҲ�ܹ���.
        if(p)
        {
            p->AddRef();
        }
        if(ptr_ )
        {
            ptr_ ->Release();
        }
        ptr_ = p;
        return *this;
    }

    scoped_refptr<T>& operator=(const scoped_refptr<T>& r)
    {
        return *this = r.ptr_;
    }

    template<typename U>
    scoped_refptr<T>& operator=(const scoped_refptr<U>& r)
    {
        return *this = r.get();
    }

    void swap(T** pp)
    {
        T* p = ptr_;
        ptr_ = *pp;
        *pp = p;
    }

    void swap(scoped_refptr<T>& r)
    {
        swap(&r.ptr_);
    }

protected:
    T* ptr_;
};

template<typename T>
scoped_refptr<T> make_scoped_refptr(T* t)
{
    return scoped_refptr<T>(t);
}

#endif //__ref_counted_h__