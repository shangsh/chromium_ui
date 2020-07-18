
#ifndef __task_h__
#define __task_h__

#pragma once

#include "base/weak_ptr.h"

#include "raw_scoped_refptr_mismatch_checker.h"

// Task��ʾ��ִ�е�����, ͨ�������������߳���ִ�д����������Ϣѭ���а���
// ����ִ��.
class Task
{
public:
    Task();
    virtual ~Task();

    // Tasks��Run()������������Զ�ɾ��.
    virtual void Run() = 0;
};

class CancelableTask : public Task
{
public:
    CancelableTask();
    virtual ~CancelableTask();

    // �������е�����֧��ȡ������.
    virtual void Cancel() = 0;
};

// ������Χ�Ĺ�����������ڰ�ȫ�Ĵ��������ü������󲢷���������Ϣѭ��.
// �೧��֤�������������೧��������֮����ִ��. һ�����, �೧��������Ϊ
// ��ĳ�Ա����, ��������������������ʱ�Զ�ȡ��.
//
// �÷�ʾ��:
//
//     class MyClass {
//     private:
//       // This factory will be used to schedule invocations of SomeMethod.
//       ScopedRunnableMethodFactory<MyClass> some_method_factory_;
//
//     public:
//       // It is safe to suppress warning 4355 here.
//       MyClass() : ALLOW_THIS_IN_INITIALIZER_LIST(some_method_factory_(this)) {}
//
//       void SomeMethod() {
//         // If this function might be called directly, you might want to revoke
//         // any outstanding runnable methods scheduled to call it.  If it's not
//         // referenced other than by the factory, this is unnecessary.
//         some_method_factory_.RevokeAll();
//         ...
//       }
//
//       void ScheduleSomeMethod() {
//         // If you'd like to only only have one pending task at a time, test for
//         // |empty| before manufacturing another task.
//         if(!some_method_factory_.empty())
//           return;
//
//         // The factories are not thread safe, so always invoke on
//         // |MessageLoop::current()|.
//         MessageLoop::current()->PostDelayedTask(
//           FROM_HERE,
//           some_method_factory_.NewRunnableMethod(&MyClass::SomeMethod),
//           kSomeMethodDelayMS);
//       }
//     };

// ScopedRunnableMethodFactory����һ��ָ������Ŀ�ִ�з���. ͨ����Ϊ������
// ������ĳ�Ա����, �������ɻص�����.
template<class T>
class ScopedRunnableMethodFactory
{
public:
    explicit ScopedRunnableMethodFactory(T* object) : weak_factory_(object) {}

    template<class Method>
    inline CancelableTask* NewRunnableMethod(Method method)
    {
        return new RunnableMethod<Method, Tuple0>(
            weak_factory_.GetWeakPtr(), method, MakeTuple());
    }

    template<class Method, class A>
    inline CancelableTask* NewRunnableMethod(Method method, const A& a)
    {
        return new RunnableMethod<Method, Tuple1<A> >(
            weak_factory_.GetWeakPtr(), method, MakeTuple(a));
    }

    template<class Method, class A, class B>
    inline CancelableTask* NewRunnableMethod(Method method, const A& a,
        const B& b)
    {
        return new RunnableMethod<Method, Tuple2<A, B> >(
            weak_factory_.GetWeakPtr(), method, MakeTuple(a, b));
    }

    template<class Method, class A, class B, class C>
    inline CancelableTask* NewRunnableMethod(Method method,
        const A& a, const B& b, const C& c)
    {
        return new RunnableMethod<Method, Tuple3<A, B, C> >(
            weak_factory_.GetWeakPtr(), method, MakeTuple(a, b, c));
    }

    template<class Method, class A, class B, class C, class D>
    inline CancelableTask* NewRunnableMethod(Method method,
        const A& a, const B& b, const C& c, const D& d)
    {
        return new RunnableMethod<Method, Tuple4<A, B, C, D> >(
            weak_factory_.GetWeakPtr(), method, MakeTuple(a, b, c, d));
    }

    template<class Method, class A, class B, class C, class D, class E>
    inline CancelableTask* NewRunnableMethod(Method method,
        const A& a, const B& b, const C& c, const D& d, const E& e)
    {
        return new RunnableMethod<Method, Tuple5<A, B, C, D, E> >(
            weak_factory_.GetWeakPtr(), method, MakeTuple(a, b, c, d, e));
    }

    void RevokeAll() { weak_factory_.InvalidateWeakPtrs(); }

    bool empty() const { return !weak_factory_.HasWeakPtrs(); }

protected:
    template<class Method, class Params>
    class RunnableMethod : public CancelableTask
    {
    public:
        RunnableMethod(const base::WeakPtr<T>& obj,
            Method meth, const Params& params)
            : obj_(obj), meth_(meth), params_(params)
        {
            COMPILE_ASSERT((MethodUsesScopedRefptrCorrectly<Method, Params>::value),
                badscopedrunnablemethodparams);
        }

        virtual void Run()
        {
            if(obj_)
            {
                DispatchToMethod(obj_.get(), meth_, params_);
            }
        }

        virtual void Cancel()
        {
            obj_.reset();
        }

    private:
        base::WeakPtr<T> obj_;
        Method meth_;
        Params params_;

        DISALLOW_COPY_AND_ASSIGN(RunnableMethod);
    };

private:
    base::WeakPtrFactory<T> weak_factory_;
};

// ɾ�������Task.
template<class T>
class DeleteTask : public CancelableTask
{
public:
    explicit DeleteTask(T* obj) : obj_(obj) {}
    virtual void Run()
    {
        delete obj_;
    }
    virtual void Cancel()
    {
        obj_ = NULL;
    }

private:
    T* obj_;
};

// ���ö���Release()������Task.
template<class T>
class ReleaseTask : public CancelableTask
{
public:
    explicit ReleaseTask(T* obj) : obj_(obj) {}
    virtual void Run()
    {
        if(obj_)
        {
            obj_->Release();
        }
    }
    virtual void Cancel()
    {
        obj_ = NULL;
    }

private:
    T* obj_;
};

// RunnableMethodTraits --------------------------------------------------------
//
// This traits-class is used by RunnableMethod to manage the lifetime of the
// callee object.  By default, it is assumed that the callee supports AddRef
// and Release methods.  A particular class can specialize this template to
// define other lifetime management.  For example, if the callee is known to
// live longer than the RunnableMethod object, then a RunnableMethodTraits
// struct could be defined with empty RetainCallee and ReleaseCallee methods.
//
// The DISABLE_RUNNABLE_METHOD_REFCOUNT macro is provided as a convenient way
// for declaring a RunnableMethodTraits that disables refcounting.

template<class T>
struct RunnableMethodTraits
{
    RunnableMethodTraits()
    {
#ifndef NDEBUG
        origin_thread_id_ = GetCurrentThreadId();
#endif
    }

    ~RunnableMethodTraits()
    {
#ifndef NDEBUG
        // If destroyed on a separate thread, then we had better have been using
        // thread-safe reference counting!
        if(origin_thread_id_ != GetCurrentThreadId())
        {
            DCHECK(T::ImplementsThreadSafeReferenceCounting());
        }
#endif
    }

    void RetainCallee(T* obj)
    {
#ifndef NDEBUG
        // Catch NewRunnableMethod being called in an object's constructor.  This
        // isn't safe since the method can be invoked before the constructor
        // completes, causing the object to be deleted.
        obj->AddRef();
        obj->Release();
#endif
        obj->AddRef();
    }

    void ReleaseCallee(T* obj)
    {
        obj->Release();
    }

private:
#ifndef NDEBUG
    DWORD origin_thread_id_;
#endif
};

// Convenience macro for declaring a RunnableMethodTraits that disables
// refcounting of a class.  This is useful if you know that the callee
// will outlive the RunnableMethod object and thus do not need the ref counts.
//
// The invocation of DISABLE_RUNNABLE_METHOD_REFCOUNT should be done at the
// global namespace scope.  Example:
//
//   namespace foo {
//   class Bar {
//     ...
//   };
//   }  // namespace foo
//
//   DISABLE_RUNNABLE_METHOD_REFCOUNT(foo::Bar)
//
// This is different from DISALLOW_COPY_AND_ASSIGN which is declared inside the
// class.
#define DISABLE_RUNNABLE_METHOD_REFCOUNT(TypeName) \
    template<> \
    struct RunnableMethodTraits<TypeName> { \
        void RetainCallee(TypeName* manager) {} \
        void ReleaseCallee(TypeName* manager) {} \
    }

// RunnableMethod and RunnableFunction -----------------------------------------
//
// Runnable methods are a type of task that call a function on an object when
// they are run. We implement both an object and a set of NewRunnableMethod and
// NewRunnableFunction functions for convenience. These functions are
// overloaded and will infer the template types, simplifying calling code.
//
// The template definitions all use the following names:
// T                - the class type of the object you're supplying
//                    this is not needed for the Static version of the call
// Method/Function  - the signature of a pointer to the method or function you
//                    want to call
// Param            - the parameter(s) to the method, possibly packed as a Tuple
// A                - the first parameter (if any) to the method
// B                - the second parameter (if any) to the method
//
// Put these all together and you get an object that can call a method whose
// signature is:
//   R T::MyFunction([A[, B]])
//
// Usage:
// PostTask(FROM_HERE, NewRunnableMethod(object, &Object::method[, a[, b]])
// PostTask(FROM_HERE, NewRunnableFunction(&function[, a[, b]])

// RunnableMethod and NewRunnableMethod implementation -------------------------

template<class T, class Method, class Params>
class RunnableMethod : public CancelableTask
{
public:
    RunnableMethod(T* obj, Method meth, const Params& params)
        : obj_(obj), meth_(meth), params_(params)
    {
        traits_.RetainCallee(obj_);
        COMPILE_ASSERT((MethodUsesScopedRefptrCorrectly<Method, Params>::value),
            badrunnablemethodparams);
    }

    ~RunnableMethod()
    {
        ReleaseCallee();
    }

    virtual void Run()
    {
        if(obj_)
        {
            DispatchToMethod(obj_, meth_, params_);
        }
    }

    virtual void Cancel()
    {
        ReleaseCallee();
    }

private:
    void ReleaseCallee()
    {
        if(obj_)
        {
            traits_.ReleaseCallee(obj_);
            obj_ = NULL;
        }
    }

    T* obj_;
    Method meth_;
    Params params_;
    RunnableMethodTraits<T> traits_;
};

template<class T, class Method>
inline CancelableTask* NewRunnableMethod(T* object, Method method)
{
    return new RunnableMethod<T, Method, Tuple0>(object, method, MakeTuple());
}

template<class T, class Method, class A>
inline CancelableTask* NewRunnableMethod(T* object, Method method, const A& a)
{
    return new RunnableMethod<T, Method, Tuple1<A> >(object,
        method, MakeTuple(a));
}

template<class T, class Method, class A, class B>
inline CancelableTask* NewRunnableMethod(T* object, Method method,
                                         const A& a, const B& b)
{
    return new RunnableMethod<T, Method, Tuple2<A, B> >(object, method,
        MakeTuple(a, b));
}

template<class T, class Method, class A, class B, class C>
inline CancelableTask* NewRunnableMethod(T* object, Method method,
                                         const A& a, const B& b, const C& c)
{
    return new RunnableMethod<T, Method, Tuple3<A, B, C> >(object, method,
        MakeTuple(a, b, c));
}

template<class T, class Method, class A, class B, class C, class D>
inline CancelableTask* NewRunnableMethod(T* object, Method method,
                                         const A& a, const B& b,
                                         const C& c, const D& d)
{
    return new RunnableMethod<T, Method, Tuple4<A, B, C, D> >(object, method,
        MakeTuple(a, b,
        c, d));
}

template<class T, class Method, class A, class B, class C, class D, class E>
inline CancelableTask* NewRunnableMethod(T* object, Method method,
                                         const A& a, const B& b,
                                         const C& c, const D& d, const E& e)
{
    return new RunnableMethod<T,
        Method,
        Tuple5<A, B, C, D, E> >(object,
        method,
        MakeTuple(a, b, c, d, e));
}

template<class T, class Method, class A, class B, class C, class D, class E,
class F>
    inline CancelableTask* NewRunnableMethod(T* object, Method method,
    const A& a, const B& b,
    const C& c, const D& d, const E& e,
    const F& f)
{
    return new RunnableMethod<T,
        Method,
        Tuple6<A, B, C, D, E, F> >(object,
        method,
        MakeTuple(a, b, c, d, e,
        f));
}

template<class T, class Method, class A, class B, class C, class D, class E,
class F, class G>
    inline CancelableTask* NewRunnableMethod(T* object, Method method,
    const A& a, const B& b,
    const C& c, const D& d, const E& e,
    const F& f, const G& g)
{
    return new RunnableMethod<T,
        Method,
        Tuple7<A, B, C, D, E, F, G> >(object,
        method,
        MakeTuple(a, b, c, d,
        e, f, g));
}

#endif //__task_h__