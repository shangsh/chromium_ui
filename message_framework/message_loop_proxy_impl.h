
#ifndef __message_loop_proxy_impl_h__
#define __message_loop_proxy_impl_h__

#pragma once

#include "message_loop.h"
#include "message_loop_proxy.h"

namespace base
{

    // MessageLoopProxy��һ������ʵ��, ����MessageLoop, ��ͨ��MessageLoop��
    // DestructionObserver�ӿڸ���������������. Ŀǰֻ���ڵ�ǰ�̴߳���һ��
    // MessageLoopProxyImpl����.
    class MessageLoopProxyImpl : public MessageLoopProxy,
        public MessageLoop::DestructionObserver
    {
    public:
        ~MessageLoopProxyImpl();

        virtual bool PostTask(Task* task);
        virtual bool PostDelayedTask(Task* task, int64 delay_ms);
        virtual bool PostNonNestableTask(Task* task);
        virtual bool PostNonNestableDelayedTask(Task* task, int64 delay_ms);
        virtual bool BelongsToCurrentThread();

        void WillDestroyCurrentMessageLoop();

    protected:
        // ����OnDestruct, �������Ŀ����Ϣѭ��������, ���ǿ���ɾ�����еĶ���.
        virtual void OnDestruct();

    private:
        MessageLoopProxyImpl();
        bool PostTaskHelper(Task* task, int64 delay_ms, bool nestable);

        friend class MessageLoopProxy;

        // target_message_loop_���ʱ�����.
        Lock message_loop_lock_;
        MessageLoop* target_message_loop_;

        DISALLOW_COPY_AND_ASSIGN(MessageLoopProxyImpl);
    };

} //namespace base

#endif //__message_loop_proxy_impl_h__