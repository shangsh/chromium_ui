
#include "slide_animation.h"

#include <math.h>

// ÿ���ӵ�֡��.
static const int kDefaultFramerateHz = 50;

// ����ȱʡ�ĳ���ʱ��.
static const int kDefaultDurationMs = 120;

SlideAnimation::SlideAnimation(AnimationDelegate* target)
: LinearAnimation(kDefaultFramerateHz, target),
target_(target),
tween_type_(Tween::EASE_OUT),
showing_(false),
value_start_(0),
value_end_(0),
value_current_(0),
slide_duration_(kDefaultDurationMs) {}

SlideAnimation::~SlideAnimation() {}

void SlideAnimation::Reset()
{
    Reset(0);
}

void SlideAnimation::Reset(double value)
{
    Stop();
    showing_ = static_cast<bool>(value == 1);
    value_current_ = value;
}

void SlideAnimation::Show()
{
    // ���������ʾ(������ȫ��ʾ), ʲô������.
    if(showing_)
    {
        return;
    }

    showing_ = true;
    value_start_ = value_current_;
    value_end_ = 1.0;

    // ȷ������һЩ����.
    if(slide_duration_ == 0)
    {
        AnimateToState(1.0); // ����������β.
        return;
    }
    else if(value_current_ == value_end_) 
    {
        return;
    }

    // ��ǰ���ڷ����Ķ����ᱻ����.
    SetDuration(static_cast<int>(slide_duration_ * (1 - value_current_)));
    Start();
}

void SlideAnimation::Hide()
{
    // �����������(������ȫ����), ʲô������.
    if(!showing_)
    {
        return;
    }

    showing_ = false;
    value_start_ = value_current_;
    value_end_ = 0.0;

    // ȷ������һЩ����.
    if(slide_duration_ == 0)
    {
        AnimateToState(0.0); // ����������β(��������µ��ڿ�ͷ).
        return;
    }
    else if(value_current_ == value_end_)
    {
        return;
    }

    // ��ǰ���ڷ����Ķ����ᱻ����.
    SetDuration(static_cast<int>(slide_duration_ * value_current_));
    Start();
}

void SlideAnimation::AnimateToState(double state)
{
    if(state > 1.0)
    {
        state = 1.0;
    }

    state = Tween::CalculateValue(tween_type_, state);

    value_current_ = value_start_ + (value_end_ - value_start_) * state;

    // ʵ������.
    if(tween_type_==Tween::EASE_OUT_SNAP &&
        fabs(value_current_-value_end_)<=0.06)
    {
        value_current_ = value_end_;
    }

    // ����Խ��ֵ(��ǰֵ���ܳ���1.0, ������Ȧ�κδ������).
    if((value_end_>=value_start_ && value_current_>value_end_) ||
        (value_end_<value_start_ && value_current_<value_end_))
    {
        value_current_ = value_end_;
    }
}