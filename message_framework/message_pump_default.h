
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
        // 标志位设置为false表示Run循环需要返回.
        bool keep_running_;

        // 休眠直到有事情发生.
        WaitableEvent event_;

        // 需要调用DoDelayedWork的时间点.
        Time delayed_work_time_;

        DISALLOW_COPY_AND_ASSIGN(MessagePumpDefault);
    };

} //namespace base

#endif //__message_pump_default_h__