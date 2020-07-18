
#include "lock_impl.h"

namespace base
{

    LockImpl::LockImpl()
    {
        // �ڶ�����������������, ���ڶ�����������Ա����߳��л��������.
        // MSDN˵: �ڶ��������ڶദ���������, һ���߳��������Ѽ�����
        //         critical sectionʱ�����ѭ��, ����ʱ��Ż����sleep.
        InitializeCriticalSectionAndSpinCount(&os_lock_, 2000);
    }

    LockImpl::~LockImpl()
    {
        DeleteCriticalSection(&os_lock_);
    }

    bool LockImpl::Try()
    {
        if(TryEnterCriticalSection(&os_lock_) != FALSE)
        {
            return true;
        }
        return false;
    }

    void LockImpl::Lock()
    {
        EnterCriticalSection(&os_lock_);
    }

    void LockImpl::Unlock()
    {
        LeaveCriticalSection(&os_lock_);
    }

} //namespace base