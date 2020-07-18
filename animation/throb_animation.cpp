
#include "throb_animation.h"

#include <limits>

static const int kDefaultThrobDurationMS = 400;

ThrobAnimation::ThrobAnimation(AnimationDelegate* target)
: SlideAnimation(target),
slide_duration_(GetSlideDuration()),
throb_duration_(kDefaultThrobDurationMS),
cycles_remaining_(0),
throbbing_(false) {}

void ThrobAnimation::StartThrobbing(int cycles_til_stop)
{
    cycles_til_stop = cycles_til_stop>=0 ? cycles_til_stop
        : std::numeric_limits<int>::max();
    cycles_remaining_ = cycles_til_stop;
    throbbing_ = true;
    SlideAnimation::SetSlideDuration(throb_duration_);
    if(is_animating())
    {
        return; // �Ѿ�������, ��ǰѭ����ɺ�������ת.
    }

    if(IsShowing())
    {
        SlideAnimation::Hide();
    }
    else
    {
        SlideAnimation::Show();
    }
    cycles_remaining_ = cycles_til_stop;
}

void ThrobAnimation::Reset()
{
    ResetForSlide();
    SlideAnimation::Reset();
}

void ThrobAnimation::Show()
{
    ResetForSlide();
    SlideAnimation::Show();
}

void ThrobAnimation::Hide()
{
    ResetForSlide();
    SlideAnimation::Hide();
}

void ThrobAnimation::Step(base::TimeTicks time_now)
{
    LinearAnimation::Step(time_now);

    if(!is_animating() && throbbing_)
    {
        // ���������һȦ, ������ʼ��һȦֱ�����, ����ֹͣ.
        cycles_remaining_--;
        if(IsShowing())
        {
            // Ҫ��ֹͣʱ�����ص�, �������ﲻ���cycles_remaining_.
            SlideAnimation::Hide();
        }
        else if(cycles_remaining_ > 0)
        {
            SlideAnimation::Show();
        }
        else
        {
            // �������.
            throbbing_ = false;
        }
    }
}

void ThrobAnimation::ResetForSlide()
{
    SlideAnimation::SetSlideDuration(slide_duration_);
    cycles_remaining_ = 0;
    throbbing_ = false;
}