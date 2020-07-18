
#ifndef __callback_h__
#define __callback_h__

#pragma once

#include "base/tuple.h"

#include "raw_scoped_refptr_mismatch_checker.h"

// Callback��Task����, ��û�в�������, ʵ�������������ĺ���ָ��.
//
// Callbacks����ƺ�Tuples��ͬ����. ��һϵ�еĸ��������������ڶ�ʹ��������
// Tuple��ϸ��. �û�����ֻ��Ҫ��CallbackRunner����򽻵�, CallbackRunnerֻ
// ��Run����, ͨ��New*��������. �����û����ع����ĸ���ʵ���˻ص�, ֻ��Ҫ֪
// �����������Լ�����.
//
// ��Щʵ����ͨ��CallbackImpl��ɵ�, CallbackImpl������CallbackStorage, ����
// �Ĵ洢���Ͷ��û�͸��, �û�ֻ��Ҫ���ü���.
//
// ע��callbacksĿǰ����ȡ�����߷����Լ��Ĳ���, ���������ϲ���д����.
// callback�е�ָ���ڵ��ý���ǰ����������Ч��.
//
// ����Task, һ���ص�ִ�н���, �ص�ִ���߸���ɾ���ص�ָ��.
//
// �ͻ����÷�ʾ��:
//     void Object::DoStuff(int, string);
//     Callback2<int, string>::Type* callback =
//       NewCallback(obj, &Object::DoStuff);
//     callback->Run(5, string("hello"));
//     delete callback;
// ����ֱ��ʹ��Ԫ��:
//     CallbackRunner<Tuple2<int, string> >* callback =
//       NewCallback(obj, &Object::DoStuff);
//     callback->RunWithParams(MakeTuple(5, string("hello")));
//
// ��һ�����������з���ֵ�İ汾. ʾ��:
//     int Object::GetNextInt();
//     CallbackWithReturnValue<int>::Type* callback =
//       NewCallbackWithReturnValue(obj, &Object::GetNextInt);
//     int next_int = callback->Run();
//     delete callback;


// ����Callbacks�Ļ���, �洢���󷽷�ָ��.
template<class T, typename Method>
class CallbackStorage
{
public:
    CallbackStorage(T* obj, Method meth) : obj_(obj), meth_(meth) {}

protected:
    T* obj_;
    Method meth_;
};

// Interface that is exposed to the consumer, that does the actual calling
// of the method.
template<typename Params>
class CallbackRunner
{
public:
    typedef Params TupleType;

    virtual ~CallbackRunner() {}
    virtual void RunWithParams(const Params& params) = 0;

    // Convenience functions so callers don't have to deal with Tuples.
    inline void Run()
    {
        RunWithParams(Tuple0());
    }

    template<typename Arg1>
    inline void Run(const Arg1& a)
    {
        RunWithParams(Params(a));
    }

    template<typename Arg1, typename Arg2>
    inline void Run(const Arg1& a, const Arg2& b)
    {
        RunWithParams(Params(a, b));
    }

    template<typename Arg1, typename Arg2, typename Arg3>
    inline void Run(const Arg1& a, const Arg2& b, const Arg3& c)
    {
        RunWithParams(Params(a, b, c));
    }

    template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    inline void Run(const Arg1& a, const Arg2& b, const Arg3& c, const Arg4& d)
    {
        RunWithParams(Params(a, b, c, d));
    }

    template<typename Arg1, typename Arg2, typename Arg3,
        typename Arg4, typename Arg5>
        inline void Run(const Arg1& a, const Arg2& b, const Arg3& c,
        const Arg4& d, const Arg5& e)
    {
        RunWithParams(Params(a, b, c, d, e));
    }
};

template<class T, typename Method, typename Params>
class CallbackImpl : public CallbackStorage<T, Method>,
    public CallbackRunner<Params>
{
public:
    CallbackImpl(T* obj, Method meth) : CallbackStorage<T, Method>(obj, meth) {}
    virtual void RunWithParams(const Params& params)
    {
        // use "this->" to force C++ to look inside our templatized base class; see
        // Effective C++, 3rd Ed, item 43, p210 for details.
        DispatchToMethod(this->obj_, this->meth_, params);
    }
};

// 0-arg implementation
struct Callback0
{
    typedef CallbackRunner<Tuple0> Type;
};

template<class T>
typename Callback0::Type* NewCallback(T* object, void (T::*method)())
{
    return new CallbackImpl<T, void (T::*)(), Tuple0 >(object, method);
}

// 1-arg implementation
template<typename Arg1>
struct Callback1
{
    typedef CallbackRunner<Tuple1<Arg1> > Type;
};

template<class T, typename Arg1>
typename Callback1<Arg1>::Type* NewCallback(T* object, void (T::*method)(Arg1))
{
    return new CallbackImpl<T, void (T::*)(Arg1), Tuple1<Arg1> >(object, method);
}

// 2-arg implementation
template<typename Arg1, typename Arg2>
struct Callback2
{
    typedef CallbackRunner<Tuple2<Arg1, Arg2> > Type;
};

template<class T, typename Arg1, typename Arg2>
typename Callback2<Arg1, Arg2>::Type* NewCallback(
    T* object, void (T::*method)(Arg1, Arg2))
{
    return new CallbackImpl<T, void (T::*)(Arg1, Arg2),
        Tuple2<Arg1, Arg2> >(object, method);
}

// 3-arg implementation
template<typename Arg1, typename Arg2, typename Arg3>
struct Callback3
{
    typedef CallbackRunner<Tuple3<Arg1, Arg2, Arg3> > Type;
};

template<class T, typename Arg1, typename Arg2, typename Arg3>
typename Callback3<Arg1, Arg2, Arg3>::Type* NewCallback(
    T* object, void (T::*method)(Arg1, Arg2, Arg3))
{
    return new CallbackImpl<T,  void (T::*)(Arg1, Arg2, Arg3),
        Tuple3<Arg1, Arg2, Arg3> >(object, method);
}

// 4-arg implementation
template<typename Arg1, typename Arg2, typename Arg3, typename Arg4>
struct Callback4
{
    typedef CallbackRunner<Tuple4<Arg1, Arg2, Arg3, Arg4> > Type;
};

template<class T, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
typename Callback4<Arg1, Arg2, Arg3, Arg4>::Type* NewCallback(
    T* object, void (T::*method)(Arg1, Arg2, Arg3, Arg4))
{
    return new CallbackImpl<T, void (T::*)(Arg1, Arg2, Arg3, Arg4),
        Tuple4<Arg1, Arg2, Arg3, Arg4> >(object, method);
}

// 5-arg implementation
template<typename Arg1, typename Arg2, typename Arg3,
typename Arg4, typename Arg5>
struct Callback5
{
    typedef CallbackRunner<Tuple5<Arg1, Arg2, Arg3, Arg4, Arg5> > Type;
};

template<class T, typename Arg1, typename Arg2,
typename Arg3, typename Arg4, typename Arg5>
typename Callback5<Arg1, Arg2, Arg3, Arg4, Arg5>::Type* NewCallback(
    T* object, void (T::*method)(Arg1, Arg2, Arg3, Arg4, Arg5))
{
    return new CallbackImpl<T, void (T::*)(Arg1, Arg2, Arg3, Arg4, Arg5),
        Tuple5<Arg1, Arg2, Arg3, Arg4, Arg5> >(object, method);
}

// An UnboundMethod is a wrapper for a method where the actual object is
// provided at Run dispatch time.
template<class T, class Method, class Params>
class UnboundMethod
{
public:
    UnboundMethod(Method m, const Params& p) : m_(m), p_(p)
    {
        COMPILE_ASSERT((MethodUsesScopedRefptrCorrectly<Method, Params>::value),
            badunboundmethodparams);
    }
    void Run(T* obj) const
    {
        DispatchToMethod(obj, m_, p_);
    }

private:
    Method m_;
    Params p_;
};

// �޲���������ֵ�汾.
template<typename ReturnValue>
struct CallbackWithReturnValue
{
    class Type
    {
    public:
        virtual ~Type() {}
        virtual ReturnValue Run() = 0;
    };
};

template<class T, typename Method, typename ReturnValue>
class CallbackWithReturnValueImpl : public CallbackStorage<T, Method>,
    public CallbackWithReturnValue<ReturnValue>::Type
{
public:
    CallbackWithReturnValueImpl(T* obj, Method meth)
        : CallbackStorage<T, Method>(obj, meth) {}

    virtual ReturnValue Run()
    {
        return (this->obj_->*(this->meth_))();
    }

protected:
    virtual ~CallbackWithReturnValueImpl() {}
};

template<class T, typename ReturnValue>
typename CallbackWithReturnValue<ReturnValue>::Type*
NewCallbackWithReturnValue(T* object, ReturnValue (T::*method)())
{
    return new CallbackWithReturnValueImpl<T, ReturnValue (T::*)(),
        ReturnValue>(object, method);
}

#endif //__callback_h__