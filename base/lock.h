
#ifndef __lock_h__
#define __lock_h__

#pragma once

#include "lock_impl.h"

namespace base
{

    // OS�ٽ�����װ. ֻ��debugģʽ��֧��AssertAcquired()����.
    class Lock
    {
    public:
#if defined(NDEBUG)
        Lock() : lock_() {}
        ~Lock() {}
        void Acquire() { lock_.Lock(); }
        void Release() { lock_.Unlock(); }

        // ���û������, ����������true. ����Ѿ��������߳�����, ��������false.
        // �Ѿ��������̲߳�Ҫ�ٵ���(���ܲ���Ԥ�ڲ����Ķ���ʧ��).
        bool Try() { return lock_.Try(); }

        // ��debugģʽ���ǿ�ʵ��.
        void AssertAcquired() const {}
#else //!NDEBUG
        Lock();
        ~Lock() {}

        // ע��: ����windows���ٽ���֧�ֵݹ����, �������ﲻ����, ����̵߳ڶ���
        // �������(�Ѿ�����)�ᴥ��DCHECK().
        void Acquire()
        {
            lock_.Lock();
            CheckUnheldAndMark();
        }
        void Release()
        {
            CheckHeldAndUnmark();
            lock_.Unlock();
        }

        bool Try()
        {
            bool rv = lock_.Try();
            if(rv)
            {
                CheckUnheldAndMark();
            }
            return rv;
        }

        void AssertAcquired() const;
#endif //NDEBUG

    private:
#if !defined(NDEBUG)
        // ����2������Ϊ�ݹ����ʱ�����������.
        void CheckHeldAndUnmark();
        void CheckUnheldAndMark();

        // ����˽�г�Ա��lock_����, �����Ҫע��ֻ���ڼ�����ʹ��.

        // owned_by_thread_����ȷ��owning_thread_id_�Ƿ�Ϸ�.
        // ��Ϊowning_thread_id_û�п�ֵ(null).
        bool owned_by_thread_;
        DWORD owning_thread_id_;
#endif //NDEBUG

        LockImpl lock_;

        DISALLOW_COPY_AND_ASSIGN(Lock);
    };

    // �Զ�����������, �����������Զ�����.
    class AutoLock
    {
    public:
        explicit AutoLock(Lock& lock) : lock_(lock)
        {
            lock_.Acquire();
        }

        ~AutoLock()
        {
            lock_.AssertAcquired();
            lock_.Release();
        }

    private:
        Lock& lock_;
        DISALLOW_COPY_AND_ASSIGN(AutoLock);
    };

    // �Զ�����������, ���캯���ж����Ѽ���������, ����ʱ���¼���.
    class AutoUnlock
    {
    public:
        explicit AutoUnlock(Lock& lock) : lock_(lock)
        {
            // ���Ե������Ѽ���.
            lock_.AssertAcquired();
            lock_.Release();
        }

        ~AutoUnlock()
        {
            lock_.Acquire();
        }

    private:
        Lock& lock_;
        DISALLOW_COPY_AND_ASSIGN(AutoUnlock);
    };

} //namespace base

#endif //__lock_h__