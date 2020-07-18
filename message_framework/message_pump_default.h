
#ifndef __message_pump_default_h__
#define __message_pump_default_h__

#pragma once

#include "base/time.h"
#include "base/waitable_event.h"

#include "message_pump.h"

namespace base
{

    class MessagePumpDefault : public MessagePump
    {
    public:
        MessagePumpDefault();
        ~MessagePumpDefault() {}

        virtual void Run(Delegate* delegate);
        virtual void Quit();
        virtual void ScheduleWork();
        virtual void ScheduleDelayedWork(const Time& delayed_work_time);

    private:
        // ��־λ����Ϊfalse��ʾRunѭ����Ҫ����.
        bool keep_running_;

        // ����ֱ�������鷢��.
        WaitableEvent event_;

        // ��Ҫ����DoDelayedWork��ʱ���.
        Time delayed_work_time_;

        DISALLOW_COPY_AND_ASSIGN(MessagePumpDefault);
    };

} //namespace base

#endif //__message_pump_default_h__