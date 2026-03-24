#ifndef __atomicops_mac_h__
#define __atomicops_mac_h__

#pragma once

#include <atomic>
#include <cstdint>
#include "basic_types.h"
#include "win_types.h"

namespace base
{

    typedef int32_t Atomic32;
    typedef intptr_t Atomic64;
    typedef intptr_t AtomicWord;

    // Atomic operations using C++11 atomics
    inline Atomic32 NoBarrier_CompareAndSwap(volatile Atomic32* ptr,
        Atomic32 old_value, Atomic32 new_value)
    {
        std::atomic<Atomic32>* atomic_ptr = reinterpret_cast<std::atomic<Atomic32>*>(
            const_cast<Atomic32*>(ptr));
        Atomic32 expected = old_value;
        std::atomic_compare_exchange_strong(atomic_ptr, &expected, new_value);
        return expected;
    }

    inline Atomic32 NoBarrier_AtomicExchange(volatile Atomic32* ptr, Atomic32 new_value)
    {
        std::atomic<Atomic32>* atomic_ptr = reinterpret_cast<std::atomic<Atomic32>*>(
            const_cast<Atomic32*>(ptr));
        return std::atomic_exchange(atomic_ptr, new_value);
    }

    inline Atomic32 NoBarrier_AtomicIncrement(volatile Atomic32* ptr, Atomic32 increment)
    {
        return Barrier_AtomicIncrement(ptr, increment);
    }

    inline Atomic32 Barrier_AtomicIncrement(volatile Atomic32* ptr, Atomic32 increment)
    {
        std::atomic<Atomic32>* atomic_ptr = reinterpret_cast<std::atomic<Atomic32>*>(
            const_cast<Atomic32*>(ptr));
        return std::atomic_fetch_add(atomic_ptr, increment) + increment;
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
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }

    inline void NoBarrier_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<Atomic32>*>(
            const_cast<Atomic32*>(ptr)), value, std::memory_order_relaxed);
    }

    inline void Acquire_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<Atomic32>*>(
            const_cast<Atomic32*>(ptr)), value, std::memory_order_acquire);
    }

    inline void Release_Store(volatile Atomic32* ptr, Atomic32 value)
    {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<Atomic32>*>(
            const_cast<Atomic32*>(ptr)), value, std::memory_order_release);
    }

    inline Atomic32 NoBarrier_Load(volatile const Atomic32* ptr)
    {
        return std::atomic_load_explicit(
            reinterpret_cast<const std::atomic<Atomic32>*>(ptr),
            std::memory_order_relaxed);
    }

    inline Atomic32 Acquire_Load(volatile const Atomic32* ptr)
    {
        return std::atomic_load_explicit(
            reinterpret_cast<const std::atomic<Atomic32>*>(ptr),
            std::memory_order_acquire);
    }

    inline Atomic32 Release_Load(volatile const Atomic32* ptr)
    {
        return std::atomic_load_explicit(
            reinterpret_cast<const std::atomic<Atomic32>*>(ptr),
            std::memory_order_release);
    }

    // 64-bit operations
    inline Atomic64 NoBarrier_CompareAndSwap(volatile Atomic64* ptr,
        Atomic64 old_value, Atomic64 new_value)
    {
        std::atomic<Atomic64>* atomic_ptr = reinterpret_cast<std::atomic<Atomic64>*>(
            const_cast<Atomic64*>(ptr));
        Atomic64 expected = old_value;
        std::atomic_compare_exchange_strong(atomic_ptr, &expected, new_value);
        return expected;
    }

    inline Atomic64 NoBarrier_AtomicExchange(volatile Atomic64* ptr, Atomic64 new_value)
    {
        std::atomic<Atomic64>* atomic_ptr = reinterpret_cast<std::atomic<Atomic64>*>(
            const_cast<Atomic64*>(ptr));
        return std::atomic_exchange(atomic_ptr, new_value);
    }

    inline Atomic64 Barrier_AtomicIncrement(volatile Atomic64* ptr, Atomic64 increment)
    {
        std::atomic<Atomic64>* atomic_ptr = reinterpret_cast<std::atomic<Atomic64>*>(
            const_cast<Atomic64*>(ptr));
        return std::atomic_fetch_add(atomic_ptr, increment) + increment;
    }

    inline Atomic64 NoBarrier_AtomicIncrement(volatile Atomic64* ptr, Atomic64 increment)
    {
        return Barrier_AtomicIncrement(ptr, increment);
    }

    inline void NoBarrier_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<Atomic64>*>(
            const_cast<Atomic64*>(ptr)), value, std::memory_order_relaxed);
    }

    inline void Acquire_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<Atomic64>*>(
            const_cast<Atomic64*>(ptr)), value, std::memory_order_acquire);
    }

    inline void Release_Store(volatile Atomic64* ptr, Atomic64 value)
    {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<Atomic64>*>(
            const_cast<Atomic64*>(ptr)), value, std::memory_order_release);
    }

    inline Atomic64 NoBarrier_Load(volatile const Atomic64* ptr)
    {
        return std::atomic_load_explicit(
            reinterpret_cast<const std::atomic<Atomic64>*>(ptr),
            std::memory_order_relaxed);
    }

    inline Atomic64 Acquire_Load(volatile const Atomic64* ptr)
    {
        return std::atomic_load_explicit(
            reinterpret_cast<const std::atomic<Atomic64>*>(ptr),
            std::memory_order_acquire);
    }

    inline Atomic64 Release_Load(volatile const Atomic64* ptr)
    {
        return std::atomic_load_explicit(
            reinterpret_cast<const std::atomic<Atomic64>*>(ptr),
            std::memory_order_release);
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

} //namespace base

#endif //__atomicops_mac_h__
