
#ifndef __linear_animation_h__
#define __linear_animation_h__

#pragma once

#include "time.h"

#include "animation.h"

class AnimationDelegate;

// ����ʱ���޶��Ķ���. ��������ʱ��AnimateToState������.
class LinearAnimation : public Animation
{
public:
    // ��ʼ������ֵ, ���˳���ʱ��.
    //
    // ���������ʹ��������캯��, ���������SetDuration()����. ���һ��
    // �����, ����ʱ����ʱ���ڶ�������ɵ�����Start()֮����ܻ�ı�,
    // ������Ҫ��¶����ӿ�.
    LinearAnimation(int frame_rate, AnimationDelegate* delegate);

    // ��ʼ�����г�Ա.
    LinearAnimation(int duration, int frame_rate, AnimationDelegate* delegate);

    // ����ʹ�õĶ������ߵõ���ǰ״̬��ֵ. LinearAnimation���ṩ���Թ�ϵ, ������
    // ������������������ṩ�������͵�ֵ.
    virtual double GetCurrentValue() const;

    // ������ǰ����.
    void End();

    // �޸Ķ����ĳ���ʱ��. �����ö�������ʼ״̬.
    void SetDuration(int duration);

protected:
    // ����������ʱ�����. ���������������������״̬.
    virtual void AnimateToState(double state) = 0;

    // ��������Ҫǰ��ʱ, ��AnimationContainer����. Ӧ��ʹ��|time_now|������
    // Time::Now, �������Ա���ͬһʱ�䴥���Ķ�����һ��.
    virtual void Step(base::TimeTicks time_now);

    virtual void AnimationStopped();

    virtual bool ShouldSendCanceledFromStop();

private:
    base::TimeDelta duration_;

    // ��ǰ״̬, ��Χ��[0.0, 1.0].
    double state_;

    // ���Ϊtrue, ��ʾ��������. AnimationStopped���������ж϶����Ƿ��Ѿ�����.
    bool in_end_;

    DISALLOW_COPY_AND_ASSIGN(LinearAnimation);
};

#endif //__linear_animation_h__