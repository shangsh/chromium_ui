
#ifndef __animation_container_h__
#define __animation_container_h__

#pragma once

#include <set>

#include "message_framework/timer.h"

// AnimationContainer��Animationʹ��, ����ײ��ʱ��. ÿ��Animation������
// �ڲ�����һ��AnimationContainer. ����ͨ��Animation::SetContainer����һ��
// Animations��ͬһ��AnimationContainer, ͬһ���Animations��֤ͬʱ���º�
// ����.
//
// AnimationContainerʹ�������ü���. ÿ��Animation�������ڲ���
// AnimationContainer����.
class AnimationContainer : public base::RefCounted<AnimationContainer>
{
public:
    // ���������animationsÿ�θ��¶���֪ͨ�۲���.
    class Observer
    {
    public:
        // ���������ʱ�Ӵ���ʱ, ������animations������ɺ���ñ�����.
        virtual void AnimationContainerProgressed(
            AnimationContainer* container) = 0;

        // �����������animationsΪ��ʱ����.
        virtual void AnimationContainerEmpty(AnimationContainer* container) = 0;

    protected:
        virtual ~Observer() {}
    };

    // AnimationContainer������Ԫ�ؽӿ�, ��Animationʵ��.
    class Element
    {
    public:
        // ����animation������ʱ��. ��AnimationContainer::Start����.
        virtual void SetStartTime(base::TimeTicks start_time) = 0;

        // ��animation����ʱ����.
        virtual void Step(base::TimeTicks time_now) = 0;

        // ����animation��ʱ����. ���һ��Ԫ����Ҫ�ı����ֵ, ��Ҫ��
        // ����Stop, Ȼ��Start.
        virtual base::TimeDelta GetTimerInterval() const = 0;

    protected:
        virtual ~Element() {}
    };

    AnimationContainer();

    // Animation��Ҫ������ʱ�����. ����б�Ҫ����ʱ��.
    // ע��: Animation���Զ�����, ��Ҫֱ�ӵ���.
    void Start(Element* animation);

    // Animation��Ҫֹͣ��ʱ�����. ���û������animations����, ֹͣʱ��.
    // ע��: Animation���Զ�����, ��Ҫֱ�ӵ���.
    void Stop(Element* animation);

    void set_observer(Observer* observer) { observer_ = observer; }

    // ���һ��ִ��animation��ʱ��.
    base::TimeTicks last_tick_time() const { return last_tick_time_; }

    // �Ƿ���ʱ��������?
    bool is_running() const { return !elements_.empty(); }

private:
    friend class base::RefCounted<AnimationContainer>;

    typedef std::set<Element*> Elements;

    ~AnimationContainer();

    // Timer�ص�����.
    void Run();

    // ����min_timer_interval_������ʱ��.
    void SetMinTimerInterval(base::TimeDelta delta);

    // ��������ʱ�ӵ���Сʱ����.
    base::TimeDelta GetMinInterval();

    // ֵ�����ֿ���:
    // . ���ֻ��һ��animation������ʱ�ӻ�û������, ��ʾ���animation��ʱ��.
    // . ���һ��ִ��animation��ʱ��(::Run������).
    base::TimeTicks last_tick_time_;

    // �����Ԫ��(animations)����.
    Elements elements_;

    // ʱ�����е���Сʱ����.
    base::TimeDelta min_timer_interval_;

    base::RepeatingTimer<AnimationContainer> timer_;

    Observer* observer_;

    DISALLOW_COPY_AND_ASSIGN(AnimationContainer);
};

#endif //__animation_container_h__