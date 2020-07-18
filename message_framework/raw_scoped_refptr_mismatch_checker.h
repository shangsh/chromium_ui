
#ifndef __raw_scoped_refptr_mismatch_checker_h__
#define __raw_scoped_refptr_mismatch_checker_h__

#pragma once

#include "base/ref_counted.h"
#include "base/tuple.h"

// ��������scoped_refptr<>���Ͳ�������һ��ԭʼָ��������Ƿǳ�Σ�յ�. ��������
// �����������, ������ִ��ǰ�����������ü���. ���������ϣ���ڴ��ݲ�����ʱ��
// ���������ü���, ������������һ��! ����: http://crbug.com/27191.
// ����һ�����Ա����������������ʱ����, ��ֹ��������ķ���.
template<class A, class B>
struct ExpectsScopedRefptrButGetsRawPtr
{
    enum { value = 0 };
};

template<class A, class B>
struct ExpectsScopedRefptrButGetsRawPtr<scoped_refptr<A>, B*>
{
    enum { value = 1 };
};

template <class Function, class Params>
struct FunctionUsesScopedRefptrCorrectly
{
    enum { value = 1 };
};

template<class A1, class A2>
struct FunctionUsesScopedRefptrCorrectly<void (*)(A1), Tuple1<A2> >
{
    enum { value = !ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value };
};

template<class A1, class B1, class A2, class B2>
struct FunctionUsesScopedRefptrCorrectly<void (*)(A1, B1), Tuple2<A2, B2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value) };
};

template<class A1, class B1, class C1, class A2, class B2, class C2>
struct FunctionUsesScopedRefptrCorrectly<void (*)(A1, B1, C1),
    Tuple3<A2, B2, C2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value) };
};

template<class A1, class B1, class C1, class D1,
class A2, class B2, class C2, class D2>
struct FunctionUsesScopedRefptrCorrectly<void (*)(A1, B1, C1, D1),
    Tuple4<A2, B2, C2, D2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value) };
};

template<class A1, class B1, class C1, class D1, class E1,
class A2, class B2, class C2, class D2, class E2>
struct FunctionUsesScopedRefptrCorrectly<void (*)(A1, B1, C1, D1, E1),
    Tuple5<A2, B2, C2, D2, E2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<E1, E2>::value) };
};

template<class A1, class B1, class C1, class D1, class E1, class F1,
class A2, class B2, class C2, class D2, class E2, class F2>
struct FunctionUsesScopedRefptrCorrectly<void (*)(A1, B1, C1, D1, E1, F1),
    Tuple6<A2, B2, C2, D2, E2, F2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<E1, E2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<F1, F2>::value) };
};

template<class A1, class B1, class C1, class D1, class E1, class F1, class G1,
class A2, class B2, class C2, class D2, class E2, class F2, class G2>
struct FunctionUsesScopedRefptrCorrectly<void (*)(A1, B1, C1, D1, E1, F1, G1),
    Tuple7<A2, B2, C2, D2, E2, F2, G2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<E1, E2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<F1, F2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<G1, G2>::value) };
};

template<class Method, class Params>
struct MethodUsesScopedRefptrCorrectly
{
    enum { value = 1 };
};

template<class T, class A1, class A2>
struct MethodUsesScopedRefptrCorrectly<void (T::*)(A1), Tuple1<A2> >
{
    enum { value = !ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value };
};

template<class T, class A1, class B1, class A2, class B2>
struct MethodUsesScopedRefptrCorrectly<void (T::*)(A1, B1), Tuple2<A2, B2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value) };
};

template<class T, class A1, class B1, class C1,
class A2, class B2, class C2>
struct MethodUsesScopedRefptrCorrectly<void (T::*)(A1, B1, C1),
    Tuple3<A2, B2, C2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value) };
};

template<class T, class A1, class B1, class C1, class D1,
class A2, class B2, class C2, class D2>
struct MethodUsesScopedRefptrCorrectly<void (T::*)(A1, B1, C1, D1),
    Tuple4<A2, B2, C2, D2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value) };
};

template<class T, class A1, class B1, class C1, class D1, class E1,
class A2, class B2, class C2, class D2, class E2>
struct MethodUsesScopedRefptrCorrectly<void (T::*)(A1, B1, C1, D1, E1),
    Tuple5<A2, B2, C2, D2, E2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<E1, E2>::value) };
};

template<class T, class A1, class B1, class C1, class D1, class E1, class F1,
class A2, class B2, class C2, class D2, class E2, class F2>
struct MethodUsesScopedRefptrCorrectly<void (T::*)(A1, B1, C1, D1, E1, F1),
    Tuple6<A2, B2, C2, D2, E2, F2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<E1, E2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<F1, F2>::value) };
};

template<class T,
class A1, class B1, class C1, class D1, class E1, class F1, class G1,
class A2, class B2, class C2, class D2, class E2, class F2, class G2>
struct MethodUsesScopedRefptrCorrectly<void (T::*)(A1, B1, C1, D1, E1, F1, G1),
    Tuple7<A2, B2, C2, D2, E2, F2, G2> >
{
    enum { value = !(ExpectsScopedRefptrButGetsRawPtr<A1, A2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<B1, B2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<C1, C2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<D1, D2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<E1, E2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<F1, F2>::value ||
        ExpectsScopedRefptrButGetsRawPtr<G1, G2>::value) };
};

#endif //__raw_scoped_refptr_mismatch_checker_h__