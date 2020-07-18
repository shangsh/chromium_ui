
#ifndef __lock_impl_h__
#define __lock_impl_h__

#pragma once

#include <windows.h>

#include "basic_types.h"

namespace base
{

    // LockImpl��ʵ����������ϵͳ������������.
    // һ������²�Ҫֱ��ʹ��LockImpl, ��ʹ��Lock.
    class LockImpl
    {
    public:
        typedef CRITICAL_SECTION OSLockType;

        LockImpl();
        ~LockImpl();

        // �����ǰû����, ����������true, �����������ش���.
        bool Try();
        // ����, ��������ֱ�������ɹ�.
        void Lock();
        // ����. ֻ���������ߵ���: Try()�ɹ����ػ���Lock()�ɹ���.
        void Unlock();

    private:
        OSLockType os_lock_;

        DISALLOW_COPY_AND_ASSIGN(LockImpl);
    };

} //namespace base

#endif //__lock_impl_h__