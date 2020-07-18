
#ifndef __timer_h__
#define __timer_h__

#pragma once

#include "base/logging.h"
#include "base/time.h"

#include "task.h"

// OneShotTimer��RepeatingTimer�ṩ�˷����ʱ��API. ���������Կ���,
// OneShotTimer�ڸ�����ʱ���ӳٹ���ʱ��ص�, RepeatingTimer�ᶨ�ڵ�
// �ص�.
// 
// OneShotTimer��RepeatingTimer�������뿪�������ʱ��ȡ��ʱ��, ����
// �����ױ�֤�����뿪������֮�󲻻��ٻص�. ����������ʱ��ʱ��, ֻ
// ��Ҫʵ����һ��OneShotTimer����RepeatingTimer��Ա����.
//
// RepeatingTimer�÷�ʾ��:
//
//     class MyClass {
//     public:
//       void StartDoingStuff() {
//         timer_.Start(TimeDelta::FromSeconds(1), this, &MyClass::DoStuff);
//       }
//       void StopDoingStuff() {
//         timer_.Stop();
//       }
//     private:
//       void DoStuff() {
//         // This method is called every second to do stuff.
//         ...
//       }
//       base::RepeatingTimer<MyClass> timer_;
//     };
//
// OneShotTimer��RepeatingTimer��֧��Reset����, ����Ժ����׵��ӳ�ʱ���¼�
// ����һ���ӳ�ʱ�䴥��. �����������������, 0.5���ʱ�����timer_��Reset,
// ��ôDoStuff�ĵ��û��ӳٵ���һ��. ���仰˵, Reset��ͬ���ȵ���Stop, Ȼ��
// ��һ���Ĳ�������Start.

class MessageLoop;

namespace base
{

    //-------------------------------------------------------------------------
    // BaseTimer_Helper��OneShotTimer��RepeatingTimer���ڲ�ʵ��. ��Ҫֱ��ʹ��.
    //
    // BaseTimer<T>ģ��ʵ��֮�乲��Ĵ���.
    class BaseTimer_Helper
    {
    public:
        // ֹͣʱ��.
        ~BaseTimer_Helper()
        {
            OrphanDelayedTask();
        }

        // ����ʱ���Ƿ���������.
        bool IsRunning() const
        {
            return delayed_task_ != NULL;
        }

        // ����ʱ�ӵ�ǰ���ӳ�ʱ��. ֻ����ʱ������ʱ����!
        TimeDelta GetCurrentDelay() const
        {
            DCHECK(IsRunning());
            return delayed_task_->delay_;
        }

    protected:
        BaseTimer_Helper() : delayed_task_(NULL) {}

        // ���Է���timer_�ĳ�Ա, �������ǿ��԰������ʱ�ӷ��뿪.
        class TimerTask : public Task
        {
        public:
            explicit TimerTask(TimeDelta delay) : timer_(NULL), delay_(delay) {}
            virtual ~TimerTask() {}
            BaseTimer_Helper* timer_;
            TimeDelta delay_;
        };

        // ��delayed_task_��ʱ�ӷ��뿪, ��������ִ��ʱʲô������.
        void OrphanDelayedTask();

        // ��ʼ��һ���µ��ӳ�����. ���delayed_task_��Ϊ��, ���Ȱ�����ʱ�ӷ���.
        void InitiateDelayedTask(TimerTask* timer_task);

        TimerTask* delayed_task_;

        DISALLOW_COPY_AND_ASSIGN(BaseTimer_Helper);
    };

    //-------------------------------------------------------------------------
    // BaseTimer��OneShotTimer��RepeatingTimer���ڲ�ʵ��. ��Ҫֱ��ʹ��.
    template<class Receiver, bool kIsRepeating>
    class BaseTimer : public BaseTimer_Helper
    {
    public:
        typedef void (Receiver::*ReceiverMethod)();

        // ����ʱ��. ��Ҫ��ʱ�����е�ʱ�����.
        void Start(TimeDelta delay, Receiver* receiver, ReceiverMethod method)
        {
            DCHECK(!IsRunning());
            InitiateDelayedTask(new TimerTask(delay, receiver, method));
        }

        // ֹͣʱ��. ���ʱ��û������, �����κ�����.
        void Stop()
        {
            OrphanDelayedTask();
        }

        // �����������е�ʱ�ӵĵȴ�ʱ��.
        void Reset()
        {
            DCHECK(IsRunning());
            InitiateDelayedTask(static_cast<TimerTask*>(delayed_task_)->Clone());
        }

    private:
        typedef BaseTimer<Receiver, kIsRepeating> SelfType;

        class TimerTask : public BaseTimer_Helper::TimerTask
        {
        public:
            TimerTask(TimeDelta delay, Receiver* receiver, ReceiverMethod method)
                : BaseTimer_Helper::TimerTask(delay),
                receiver_(receiver), method_(method) {}

            virtual ~TimerTask()
            {
                // �������������п�������ΪMessageLoop�Ѿ�������. ���������, ��
                // Ҫ���������ٵ���������һ��Timer��Ұָ��.
                ClearBaseTimer();
            }

            virtual void Run()
            {
                if(!timer_) // ���������, timer_Ϊ��.
                {
                    return;
                }
                if(kIsRepeating)
                {
                    ResetBaseTimer();
                }
                else
                {
                    ClearBaseTimer();
                }
                DispatchToMethod(receiver_, method_, Tuple0());
            }

            TimerTask* Clone() const
            {
                return new TimerTask(delay_, receiver_, method_);
            }

        private:
            // ֪ͨ����ʱ���Ѿ�ֹͣ.
            void ClearBaseTimer()
            {
                if(timer_)
                {
                    SelfType* self = static_cast<SelfType*>(timer_);
                    // ʱ���п����Ѿ�������, ����һ���ɵ�����. ����, ���Timerָ��
                    // ��һ����ͬ������, ���Զ϶�Timer�Ѿ���������������.
                    if(self->delayed_task_ == this)
                    {
                        self->delayed_task_ = NULL;
                    }
                    // ��ʱTimer�е�delayed_task_����ָ������. ��Ҫ�Լ�����timer_,
                    // ��ΪTimer����������������û��.
                    timer_ = NULL;
                }
            }

            // ֪ͨ������������ʱ��.
            void ResetBaseTimer()
            {
                DCHECK(timer_);
                DCHECK(kIsRepeating);
                SelfType* self = static_cast<SelfType*>(timer_);
                self->Reset();
            }

            Receiver* receiver_;
            ReceiverMethod method_;
        };
    };

    //-------------------------------------------------------------------------
    // �򵥵�һ�ε���ʱ��.
    template<class Receiver>
    class OneShotTimer : public BaseTimer<Receiver, false> {};

    //-------------------------------------------------------------------------
    // �򵥵��ظ�����ʱ��.
    template<class Receiver>
    class RepeatingTimer : public BaseTimer<Receiver, true> {};

    //-------------------------------------------------------------------------
    // �ӳ�ʱ�����Ƶ���İ�ť. ������, �������Reset, �Ż���MessageLoop���߳���ִ
    // �и����ķ���.
    //
    // ���󴴽���, ֻ�е���Reset�Ż�����ʱ��. ���ϴε���Reset, ����|delay|���ִ��
    // �ص�. �ص���ɺ�, ֻ���ٴε���Reset������������ʱ��.
    //
    // ɾ������, ��ʱ��ȡ��, ���е�ʱ�Ӳ��ᱻִ��.
    template<class Receiver>
    class DelayTimer
    {
    public:
        typedef void (Receiver::*ReceiverMethod)();

        DelayTimer(TimeDelta delay, Receiver* receiver, ReceiverMethod method)
            : receiver_(receiver), method_(method), delay_(delay) {}

        void Reset()
        {
            DelayFor(delay_);
        }

    private:
        void DelayFor(TimeDelta delay)
        {
            trigger_time_ = Time::Now() + delay;

            // ����Ѿ���һ��ʱ��, ���ӳ�ʱ���delay�ȵ���, ʲô��������.
            if(timer_.IsRunning() && timer_.GetCurrentDelay()<=delay)
            {
                return;
            }

            // ʱ��û�����л����ӳٵ��ں���, ��������.
            timer_.Stop();
            timer_.Start(delay, this, &DelayTimer<Receiver>::Check);
        }

        void Check()
        {
            if(trigger_time_.is_null())
            {
                return;
            }

            // ����ȴ���ʱ�䲻��, �ٵ�һ��.
            const Time now = Time::Now();
            if(now < trigger_time_)
            {
                DelayFor(trigger_time_-now);
                return;
            }

            (receiver_->*method_)();
        }

        const Receiver* receiver_;
        const ReceiverMethod method_;
        const TimeDelta delay_;

        OneShotTimer<DelayTimer<Receiver> > timer_;
        Time trigger_time_;
    };

} //namespace base

#endif //__timer_h__