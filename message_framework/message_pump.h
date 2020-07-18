
#ifndef __message_pump_h__
#define __message_pump_h__

#pragma once

#include "base/ref_counted.h"

namespace base
{

    class Time;

    class MessagePump : public RefCountedThreadSafe<MessagePump>
    {
    public:
        // ��μ�Run������ע��, ���˽�Delegate���෽������α�ʹ�õ�.
        class Delegate
        {
        public:
            virtual ~Delegate() {}

            // ��Run�����е���, ������ӦScheduleWork����, ���û�п�ִ�е���������
            // ����DoDelayedWork. ����true��ʾִ������ɹ�.
            virtual bool DoWork() = 0;

            // ��Run�����е���, ������ӦScheduleDelayedWork�ĵ���, ���û���ӳ�����,
            // ��Ϣ�û�����ֱ�����µ��¼���Ҫ����. ����true��ʾִ���ӳ�����ɹ�, ��ʱ
            // �������DoIdleWork(). ����|next_delayed_work_time|����ֵ����ȷ����һ��
            // ����DoDelayedWork��ʱ��. ���|next_delayed_work_time|Ϊ��(Time::is_null),
            // ��ʾ�ӳ��������Ϊ��, �����ٰ��Ÿ÷����ĵ���.
            virtual bool DoDelayedWork(Time* next_delayed_work_time) = 0;

            // ��Run��������Ϣ������֮ǰ����. ����true���������������.
            virtual bool DoIdleWork() = 0;
        };

        MessagePump();
        virtual ~MessagePump();

        // ����Run����������Ϣ�õ�ִ��ѭ��.
        //
        // ��Run������, ��Ϣ�������ԵĴ�������Ϣ�����ô�����. ��Ϣ�û�С�ĵ�
        // �������ص��ͱ�����Ϣ, ��֤�������͵��¼����ܹ���ȷ��ִ��.
        //
        // �������͵�ִ��ѭ��:
        //
        //     for(;;) {
        //       bool did_work = DoInternalWork();
        //       if(should_quit_)
        //         break;
        //
        //       did_work |= delegate_->DoWork();
        //       if(should_quit_)
        //         break;
        //
        //       did_work |= delegate_->DoDelayedWork();
        //       if(should_quit_)
        //         break;
        //
        //       if(did_work)
        //         continue;
        //
        //       did_work = delegate_->DoIdleWork();
        //       if(should_quit_)
        //         break;
        //
        //       if(did_work)
        //         continue;
        //
        //       WaitForWork();
        //     }
        //
        // ����, DoInternalWork����Ϣ�õ�˽�з���, �����ɷ�UI��Ϣ����֪ͨIO���.
        // WaitForWork˽�з��������߳�, ֱ�����µ��¼���Ҫ����.
        //
        // DoInternalWork��DoWork��DoDelayedWork������ѭ���в��ϵ�ִ��, ȷ������
        // ���͵�������ж��л��ᱻִ��. ������˵, ���������������Ϣ�÷ǳ���Ҫ.
        //
        // ֵ��ע�����ÿ�ε����ⲿ����, ѭ���������Ƿ���Ҫ�˳�. Quit����������
        // �ñ�־λ. һ���������˳���־λ, ���ٴ����κ�����.
        //
        // ע��: �ⲿ���������еĴ����ٴε���Run��������ǳ�С��. ������Ϣ��Ҳ����
        // ��Ҫ�����ڱ����еı�����Ϣ��(���� Windows�ϵ�MessageBox API�ᷢ��UI��
        // Ϣ!). ��ȷ�ؽ�,Ƕ�׵���ѭ�������Ͽ���������Ϣ�ÿ���, �������ṩ�ⲿ����
        // ����(DoWork��DoDelayedWork). DoWork�����ȡʱ��Ƭ, ���Ƿ���false(û����
        // �������).
        virtual void Run(Delegate* delegate) = 0;

        // ������ֹ���ڲ��ִ��ѭ��. ֻ���ڵ���Run�������߳��е���.
        virtual void Quit() = 0;

        // ���Ⱦ����ִ��һ��DoWork�ص�, ����ѵ��ȹ���ʲô������. ��������������
        // �̵߳���. һ������ScheduleWork����, DoWork�ڷ���falseǰ�ض���ִ��.
        virtual void ScheduleWork() = 0;

        // ������ĳһʱ��ִ��һ��DoDelayedWork�ص�, ȡ�����еȴ���DoDelayedWork�ص�.
        // ֻ���ڵ���Run�������߳���ʹ��.
        virtual void ScheduleDelayedWork(const Time& delayed_work_time) = 0;
    };

} //namespace base

#endif //__message_pump_h__