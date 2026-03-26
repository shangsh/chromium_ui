#ifndef __atomicops_h__
#define __atomicops_h__

#pragma once

#ifdef _WIN32
#include <windows.h>
#include "basic_types.h"
#else
#include <atomic>
#include <cstdint>
#include "basic_types.h"
#endif

namespace base
{

#ifdef _WIN32

    typedef LONG Atomic32;
    typedef LONG64 Atomic64;
    typedef LONG_PTR AtomicWord;

    // Windows implementations

#else // !_WIN32

    typedef int32_t Atomic32;
    typedef int64_t Atomic64;
    typedef intptr_t AtomicWord;

    // Simple stub implementations that work with volatile
    // These avoid the volatile/atomic conversion issues
    
    inline Atomic32 NoBarrier_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value)
    {
        return __sync_val_compare_and_swap(ptr, old_value, new_value);
    }

    inline Atomic32 NoBarrier_AtomicExchange(volatile Atomic32* ptr, Atomic32 new_value)
    {
        Atomic32 old = *ptr;
        while (!__sync_bool_compare_and_swap(ptr, old, new_value)) {
            old = *ptr;
        }
        return old;
    }

    inline Atomic32 NoBarrier_AtomicIncrement(volatile Atomic32* ptr, Atomic32 increment)
    {
        return __sync_add_and_fetch(ptr, increment);
    }

    inline Atomic32 Barrier_AtomicIncrement(volatile Atomic32* ptr, Atomic32 increment)
    {
        return __sync_add_and_fetch(ptr, increment);
    }

    inline Atomic32 Acquire_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value)
    {
        return __sync_val_compare_and_swap(ptr, old_value, new_value);
    }

    inline Atomic32 Release_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value)
    {
        return __sync_val_compare_and_swap(ptr, old_value, new_value);
    }

    inline void MemoryBarrier()
    {
        __sync_synchronize();
    }

    inline void NoBarrier_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        *ptr = value;
    }

    inline void Acquire_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        __sync_synchronize();
        *ptr = value;
    }

    inline void Release_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        *ptr = value;
        __sync_synchronize();
    }

    inline Atomic32 NoBarrier_Load(volatile const Atomic32* ptr)
    {
        return *ptr;
    }

    inline Atomic32 Acquire_Load(volatile const Atomic32* ptr)
    {
        Atomic32 val = *ptr;
        __sync_synchronize();
        return val;
    }

    inline Atomic32 Release_Load(volatile const Atomic32* ptr)
    {
        __sync_synchronize();
        return *ptr;
    }

    // 64-bit operations
    inline Atomic64 NoBarrier_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value)
    {
        return __sync_val_compare_and_swap(ptr, old_value, new_value);
    }

    inline Atomic64 NoBarrier_AtomicExchange(volatile Atomic64* ptr, Atomic64 new_value)
    {
        Atomic64 old = *ptr;
        while (!__sync_bool_compare_and_swap(ptr, old, new_value)) {
            old = *ptr;
        }
        return old;
    }

    inline Atomic64 Barrier_AtomicIncrement(volatile Atomic64* ptr, Atomic64 increment)
    {
        return __sync_add_and_fetch(ptr, increment);
    }

    inline Atomic64 NoBarrier_AtomicIncrement(volatile Atomic64* ptr, Atomic64 increment)
    {
        return __sync_add_and_fetch(ptr, increment);
    }

    inline void NoBarrier_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        *ptr = value;
    }

    inline void Acquire_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        __sync_synchronize();
        *ptr = value;
    }

    inline void Release_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        *ptr = value;
        __sync_synchronize();
    }

    inline Atomic64 NoBarrier_Load(volatile const Atomic64* ptr)
    {
        return *ptr;
    }

    inline Atomic64 Acquire_Load(volatile const Atomic64* ptr)
    {
        Atomic64 val = *ptr;
        __sync_synchronize();
        return val;
    }

    inline Atomic64 Release_Load(volatile const Atomic64* ptr)
    {
        __sync_synchronize();
        return *ptr;
    }

    inline Atomic64 Acquire_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value)
    {
        return __sync_val_compare_and_swap(ptr, old_value, new_value);
    }

    inline Atomic64 Release_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value)
    {
        return __sync_val_compare_and_swap(ptr, old_value, new_value);
    }

#endif // _WIN32

} //namespace base

#endif //__atomicops_h__
