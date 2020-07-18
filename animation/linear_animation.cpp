
#include "linear_animation.h"

using base::Time;
using base::TimeDelta;

static TimeDelta CalculateInterval(int frame_rate)
{
    int timer_interval = 1000000 / frame_rate;
    if(timer_interval < 10000)
    {
        timer_interval = 10000;
    }
    return TimeDelta::FromMicroseconds(timer_interval);
}

LinearAnimation::LinearAnimation(int frame_rate,
                                 AnimationDelegate* delegate)
                                 : Animation(CalculateInterval(frame_rate)),
                                 state_(0.0),
                                 in_end_(false)
{
    set_delegate(delegate);
}

LinearAnimation::LinearAnimation(int duration,
                                 int frame_rate,
                                 AnimationDelegate* delegate)
                                 : Animation(CalculateInterval(frame_rate)),
                                 duration_(TimeDelta::FromMilliseconds(duration)),
                                 state_(0.0),
                                 in_end_(false)
{
    set_delegate(delegate);
    SetDuration(duration);
}

double LinearAnimation::GetCurrentValue() const
{
    // ȱʡ�����Թ�ϵ, �������������е���.
    return state_;
}

void LinearAnimation::End()
{
    if(!is_animating())
    {
        return;
    }

    // ע��: �����������AutoReset, ��ΪStop���ܻ�ɾ������(ͨ������).
    in_end_ = true;
    Stop();
}

void LinearAnimation::SetDuration(int duration)
{
    duration_ = TimeDelta::FromMilliseconds(duration);
    if(duration_ < timer_interval())
    {
        duration_ = timer_interval();
    }
    if(is_animating())
    {
        SetStartTime(container()->last_tick_time());
    }
}

void LinearAnimation::Step(base::TimeTicks time_now)
{
    TimeDelta elapsed_time = time_now - start_time();
    state_ = static_cast<double>(elapsed_time.InMicroseconds()) /
        static_cast<double>(duration_.InMicroseconds());
    if(state_ >= 1.0)
    {
        state_ = 1.0;
    }

    AnimateToState(state_);

    if(delegate())
    {
        delegate()->AnimationProgressed(this);
    }

    if(state_ == 1.0)
    {
        Stop();
    }
}

void LinearAnimation::AnimationStopped()
{
    if(!in_end_)
    {
        return;
    }

    in_end_ = false;
    // ����state_������״̬, ȷ�����ô����ʱ�򲻻ᱻȡ��.
    state_ = 1;
    AnimateToState(1.0);
}

bool LinearAnimation::ShouldSendCanceledFromStop()
{
    return state_ != 1;
}