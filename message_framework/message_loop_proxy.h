
#ifndef __message_loop_proxy_h__
#define __message_loop_proxy_h__

#pragma once

#include "base/ref_counted.h"

#include "task.h"

namespace base
{

    struct MessageLoopProxyTraits;

    // MessageLoopProxyΪ��Ϣѭ����Post*�����ṩһ���̰߳�ȫ�����ü����ӿ�, �����
    // ��Ŀ����Ϣѭ���������������Ҫ��. ����ͨ��Thread::message_loop_proxy()��ȡ
    // MessageLoopProxy.
    class MessageLoopProxy
        : public base::RefCountedThreadSafe<MessageLoopProxy,
        MessageLoopProxyTraits>
    {
    public:
        // ��Щ������message_loop.h�е�һ��, ���ǲ���֤ÿ�����񶼿���Ͷ�ݵ�MessageLoop
        // (��������ڵĻ��ǿ��Ա�֤��), ���û��Ͷ�ݳɹ���ɾ������.
        // ����̴߳���������Ͷ�ݳɹ�, ��������true. ע�⼴ʹ����Ͷ�ݳɹ�, Ҳ���ܱ�֤
        // ��ִ��, ��ΪĿ���̵߳���Ϣ�����п����Ѿ�����һ��Quit��Ϣ��.
        virtual bool PostTask(Task* task) = 0;
        virtual bool PostDelayedTask(Task* task, int64 delay_ms) = 0;
        virtual bool PostNonNestableTask(Task* task) = 0;
        virtual bool PostNonNestableDelayedTask(Task* task, int64 delay_ms) = 0;
        // �������ߵ�ǰ�Ƿ���proxy������߳���ִ��.
        virtual bool BelongsToCurrentThread() = 0;

        template<class T>
        bool DeleteSoon(T* object)
        {
            return PostNonNestableTask(new DeleteTask<T>(object));
        }
        template<class T>
        bool ReleaseSoon(T* object)
        {
            return PostNonNestableTask(new ReleaseTask<T>(object));
        }

        // Ϊ��ǰ�̴߳���MessageLoopProxy����Ĺ�������.
        static scoped_refptr<MessageLoopProxy> CreateForCurrentThread();

    protected:
        friend struct MessageLoopProxyTraits;

        MessageLoopProxy();
        virtual ~MessageLoopProxy();

        // ��proxy������ɾ����ʱ�����. ��������������������, ʵ���ض��̵߳�����.
        virtual void OnDestruct();
    };

    struct MessageLoopProxyTraits
    {
        static void Destruct(MessageLoopProxy* proxy)
        {
            proxy->OnDestruct();
        }
    };

} //namespace base

#endif //__message_loop_proxy_h__