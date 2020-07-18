
#ifndef __atomicops_h__
#define __atomicops_h__

// reference counts�����ԭ�Ӳ����μ�atomic_refcount.h.
// sequence numbers�����ԭ�Ӳ����μ�atomic_sequence_num.h.
//
// ��ģ�鱩¶�����̾���һ������. ������Ҫ��ȷ����, ����Ҫ��ԭ�ӻ�(atomicity)
// ���ڴ�����(memory ordering)��ϸ����; �ɶ��ԺͿ�ά���Զ��ϲ�. ���粻ʹ��
// ��������Ӱ����߱���ѡ���ʱ��ſ�����Щ����. ���ֻʹ������ʽ��ȫ˵��
// �ĺ���, ���ֻ��x86ƽ̨��ʹ��, �����ļܹ�ƽ̨�ϻᵼ�±���. �������ȷ��
// ��Щ, ������Ҫʹ��, ����ѡ��ʹ��Mutex.
//
// ֱ�Ӵ�ȡԭ�ӱ����Ǵ����. Ӧ��ʹ��NoBarrier�汾�Ĵ�ȡ����:
//     NoBarrier_Store()
//     NoBarrier_Load()
// ��ǰ�ı���������û����ǿ��Ҫ��, �������������.

#pragma once

#include <windows.h>

#include "basic_types.h"

namespace base
{

    typedef __w64 int32 Atomic32;
#ifdef ARCH_CPU_64_BITS
    // ��Ҫ��Atomic64��AtomicWord֮�������ʽת��, ����ζ��������64-bit������һ��.
    typedef intptr_t Atomic64;
#endif

    // AtomicWord��һ������ָ�볤��, ��Atomic32����Atomic64������CPU�ܹ�.
    typedef intptr_t AtomicWord;

    // ԭ�Ӳ���:
    //     result = *ptr;
    //     if(*ptr == old_value)
    //         *ptr = new_value;
    //     return result;
    //
    // ���"*ptr"=="old_value"���滻��"new_value", ����"*ptr"��ǰ��ֵ.
    // û���ڴ�����(memory barriers).
    Atomic32 NoBarrier_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value);

    // ԭ�Ӳ���: �洢new_value��*ptr, ��������ǰ��ֵ.
    // û���ڴ�����(memory barriers).
    Atomic32 NoBarrier_AtomicExchange(volatile Atomic32* ptr, Atomic32 new_value);

    // ԭ�Ӳ���: *ptrֵ����"increment", �������Ӻ����ֵ.
    // û���ڴ�����(memory barriers).
    Atomic32 NoBarrier_AtomicIncrement(volatile Atomic32* ptr, Atomic32 increment);

    Atomic32 Barrier_AtomicIncrement(volatile Atomic32* ptr, Atomic32 increment);

    // ���漸���ײ㺯�����ڿ���������(spinlocks)���ź���(mutexes)����������
    // (condition-variables)�����ϲ�ͬ���Ŀ���������. �����CompareAndSwap(),
    // ȡ������, ��ָ֤������ڴ�����. "Acquire"������֤������ڴ���ʲ���
    // ��ǰ; "Release"������֤ǰ����ڴ���ʲ����Ӻ�. "Barrier"��������
    // "Acquire"��"Release"����. MemoryBarrier()����"Barrier"���嵫��û�з���
    // �ڴ�.
    Atomic32 Acquire_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value);
    Atomic32 Release_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value);

    void MemoryBarrier();
    void NoBarrier_Store(volatile Atomic32* ptr, Atomic32 value);
    void Acquire_Store(volatile Atomic32* ptr, Atomic32 value);
    void Release_Store(volatile Atomic32* ptr, Atomic32 value);

    Atomic32 NoBarrier_Load(volatile const Atomic32* ptr);
    Atomic32 Acquire_Load(volatile const Atomic32* ptr);
    Atomic32 Release_Load(volatile const Atomic32* ptr);

    // 64λԭ�Ӳ���(����64λ����������Ч).
#ifdef ARCH_CPU_64_BITS
    Atomic64 NoBarrier_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value);
    Atomic64 NoBarrier_AtomicExchange(volatile Atomic64* ptr, Atomic64 new_value);
    Atomic64 NoBarrier_AtomicIncrement(volatile Atomic64* ptr, Atomic64 increment);
    Atomic64 Barrier_AtomicIncrement(volatile Atomic64* ptr, Atomic64 increment);

    Atomic64 Acquire_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value);
    Atomic64 Release_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value);
    void NoBarrier_Store(volatile Atomic64* ptr, Atomic64 value);
    void Acquire_Store(volatile Atomic64* ptr, Atomic64 value);
    void Release_Store(volatile Atomic64* ptr, Atomic64 value);
    Atomic64 NoBarrier_Load(volatile const Atomic64* ptr);
    Atomic64 Acquire_Load(volatile const Atomic64* ptr);
    Atomic64 Release_Load(volatile const Atomic64* ptr);
#endif //ARCH_CPU_64_BITS


    inline Atomic32 NoBarrier_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value)
    {
        LONG result = InterlockedCompareExchange(
            reinterpret_cast<volatile LONG*>(ptr),
            static_cast<LONG>(new_value),
            static_cast<LONG>(old_value));
        return static_cast<Atomic32>(result);
    }

    inline Atomic32 NoBarrier_AtomicExchange(volatile Atomic32* ptr,
        Atomic32 new_value)
    {
        LONG result = InterlockedExchange(
            reinterpret_cast<volatile LONG*>(ptr),
            static_cast<LONG>(new_value));
        return static_cast<Atomic32>(result);
    }

    inline Atomic32 NoBarrier_AtomicIncrement(volatile Atomic32* ptr,
        Atomic32 increment)
    {
        return Barrier_AtomicIncrement(ptr, increment);
    }

    inline Atomic32 Barrier_AtomicIncrement(volatile Atomic32* ptr,
        Atomic32 increment)
    {
        return InterlockedExchangeAdd(reinterpret_cast<volatile LONG*>(ptr),
            static_cast<LONG>(increment))+increment;
    }

    inline Atomic32 Acquire_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value)
    {
        return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
    }

    inline Atomic32 Release_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value)
    {
        return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
    }

    inline void MemoryBarrier()
    {
        ::MemoryBarrier();
    }

    inline void NoBarrier_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        *ptr = value;
    }

    inline void Acquire_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        NoBarrier_AtomicExchange(ptr, value);
    }

    inline void Release_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        *ptr = value;
    }

    inline Atomic32 NoBarrier_Load(volatile const Atomic32* ptr)
    {
        return *ptr;
    }

    inline Atomic32 Acquire_Load(volatile const Atomic32* ptr)
    {
        Atomic32 value = *ptr;
        return value;
    }

    inline Atomic32 Release_Load(volatile const Atomic32* ptr)
    {
        MemoryBarrier();
        return *ptr;
    }

#if defined(_WIN64)
    // 64λƽ̨�µ�64λ�ͼ�����
    COMPILE_ASSERT(sizeof(Atomic64)==sizeof(PVOID), atomic_word_is_atomic);

    inline Atomic64 NoBarrier_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value)
    {
        PVOID result = InterlockedCompareExchangePointer(
            reinterpret_cast<volatile PVOID*>(ptr),
            reinterpret_cast<PVOID>(new_value), reinterpret_cast<PVOID>(old_value));
        return reinterpret_cast<Atomic64>(result);
    }

    inline Atomic64 NoBarrier_AtomicExchange(volatile Atomic64* ptr,
        Atomic64 new_value)
    {
        PVOID result = InterlockedExchangePointer(
            reinterpret_cast<volatile PVOID*>(ptr),
            reinterpret_cast<PVOID>(new_value));
        return reinterpret_cast<Atomic64>(result);
    }

    inline Atomic64 Barrier_AtomicIncrement(volatile Atomic64* ptr,
        Atomic64 increment)
    {
        return InterlockedExchangeAdd64(
            reinterpret_cast<volatile LONGLONG*>(ptr),
            static_cast<LONGLONG>(increment)) + increment;
    }

    inline Atomic64 NoBarrier_AtomicIncrement(volatile Atomic64* ptr,
        Atomic64 increment)
    {
        return Barrier_AtomicIncrement(ptr, increment);
    }

    inline void NoBarrier_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        *ptr = value;
    }

    inline void Acquire_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        NoBarrier_AtomicExchange(ptr, value);
    }

    inline void Release_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        *ptr = value;
    }

    inline Atomic64 NoBarrier_Load(volatile const Atomic64* ptr)
    {
        return *ptr;
    }

    inline Atomic64 Acquire_Load(volatile const Atomic64* ptr)
    {
        Atomic64 value = *ptr;
        return value;
    }

    inline Atomic64 Release_Load(volatile const Atomic64* ptr)
    {
        MemoryBarrier();
        return *ptr;
    }

    inline Atomic64 Acquire_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value)
    {
        return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
    }

    inline Atomic64 Release_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value)
    {
        return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
    }
#endif //defined(_WIN64)

} //namespace base

#endif //__atomicops_h__