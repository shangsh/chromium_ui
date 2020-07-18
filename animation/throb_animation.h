
#ifndef __throb_animation_h__
#define __throb_animation_h__

#pragma once

#include "slide_animation.h"

// SlideAnimation��������, ʵ�ֳ�������. ���з�������Ϊ����SlideAnimation��:
// ���ɵ���һ״̬. StartThrobbing����ִ��һ������������/��ʾ�任.
//
// ThrobAnimation����������ʱ��: һ������ʵ��SlideAnimation��Ϊ, ��һ������
// ����.
class ThrobAnimation : public SlideAnimation
{
public:
    explicit ThrobAnimation(AnimationDelegate* target);
    virtual ~ThrobAnimation() {}

    // ��ʼ����. cycles_til_stopָ����������. ������ʾһֱ����.
    void StartThrobbing(int cycles_til_stop);

    // ���ö����еĻ�����������ʱ��.
    void SetThrobDuration(int duration) { throb_duration_ = duration; }

    virtual void Reset();
    virtual void Show();
    virtual void Hide();

    virtual void SetSlideDuration(int duration) { slide_duration_ = duration; }

    // ʣ��Ķ�������.
    void set_cycles_remaining(int value) { cycles_remaining_ = value; }
    int cycles_remaining() const { return cycles_remaining_; }

protected:
    // ����ʵ����������(�������ڶ���).
    virtual void Step(base::TimeTicks time_now);

private:
    // ����״̬, ��������ʵ��SlideAnimation��Ϊ.
    void ResetForSlide();

    // ������������ʱ��.
    int slide_duration_;

    // ����ʱ������������ʱ��.
    int throb_duration_;

    // ������ڶ���, ��ʾʣ��Ĵ���.
    int cycles_remaining_;

    // ���ڶ���?
    bool throbbing_;

    DISALLOW_COPY_AND_ASSIGN(ThrobAnimation);
};

#endif //__throb_animation_h__