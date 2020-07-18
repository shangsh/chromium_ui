
#include "lazy_instance.h"

#include "at_exit.h"

namespace base
{

    bool LazyInstanceHelper::NeedsInstance()
    {
        // ���Դ���ʵ��, ����ǵ�һ�ε���, state��_��EMPTY��ΪCREATING.
        // �����ʾ��������.
        if(base::Acquire_CompareAndSwap(&state_,
            STATE_EMPTY, STATE_CREATING) == STATE_EMPTY)
        {
            // �����߱��봴��ʵ��.
            return true;
        }
        else
        {
            // Ҫô���ڴ���Ҫô�Ѿ��������. ѭ���ȴ�.
            while(base::NoBarrier_Load(&state_) != STATE_CREATED)
            {
                ::Sleep(0);
            }
        }

        // �����߳��Ѿ�������ʵ��.
        return false;
    }

    void LazyInstanceHelper::CompleteInstance(void* instance, void (*dtor)(void*))
    {
        // ʵ��������, ��CREATING��ΪCREATED.
        base::Release_Store(&state_, STATE_CREATED);

        // ȷ��ʵ�����Ķ����ڳ����˳�ʱ������.
        base::AtExitManager::RegisterCallback(dtor, instance);
    }

} //namespace base