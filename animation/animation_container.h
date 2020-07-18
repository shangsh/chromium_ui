
#ifndef __animation_container_h__
#define __animation_container_h__

#pragma once

#include <set>

#include "message_framework/timer.h"

// AnimationContainer被Animation使用, 管理底层的时钟. 每个Animation都会在
// 内部创建一个AnimationContainer. 可以通过Animation::SetContainer设置一组
// Animations到同一个AnimationContainer, 同一组的Animations保证同时更新和
// 启动.
//
// AnimationContainer使用了引用计数. 每个Animation都被它内部的
// AnimationContainer所有.
class AnimationContainer : public base::RefCounted<AnimationContainer>
{
public:
    // 容器管理的animations每次更新都会通知观察者.
    class Observer
    {
    public:
        // 容器管理的时钟触发时, 在所有animations更新完成后调用本函数.
        virtual void AnimationContainerProgressed(
            AnimationContainer* container) = 0;

        // 当容器管理的animations为空时调用.
        virtual void AnimationContainerEmpty(AnimationContainer* container) = 0;

    protected:
        virtual ~Observer() {}
    };

    // AnimationContainer容器的元素接口, 由Animation实现.
    class Element
    {
    public:
        // 设置animation的启动时间. 由AnimationContainer::Start调用.
        virtual void SetStartTime(base::TimeTicks start_time) = 0;

        // 当animation步进时调用.
        virtual void Step(base::TimeTicks time_now) = 0;

        // 返回animation的时间间隔. 如果一个元素想要改变这个值, 需要先
        // 调用Stop, 然后Start.
        virtual base::TimeDelta GetTimerInterval() const = 0;

    protected:
        virtual ~Element() {}
    };

    AnimationContainer();

    // Animation需要启动的时候调用. 如果有必要启动时钟.
    // 注意: Animation会自动调用, 不要直接调用.
    void Start(Element* animation);

    // Animation需要停止的时候调用. 如果没有其它animations运行, 停止时钟.
    // 注意: Animation会自动调用, 不要直接调用.
    void Stop(Element* animation);

    void set_observer(Observer* observer) { observer_ = observer; }

    // 最后一次执行animation的时间.
    base::TimeTicks last_tick_time() const { return last_tick_time_; }

    // 是否还有时钟在运行?
    bool is_running() const { return !elements_.empty(); }

private:
    friend class base::RefCounted<AnimationContainer>;

    typedef std::set<Element*> Elements;

    ~AnimationContainer();

    // Timer回调方法.
    void Run();

    // 设置min_timer_interval_并重启时钟.
    void SetMinTimerInterval(base::TimeDelta delta);

    // 返回所有时钟的最小时间间隔.
    base::TimeDelta GetMinInterval();

    // 值有两种可能:
    // . 如果只有一个animation启动且时钟还没被触发, 表示添加animation的时间.
    // . 最后一次执行animation的时间(::Run被调用).
    base::TimeTicks last_tick_time_;

    // 管理的元素(animations)集合.
    Elements elements_;

    // 时钟运行的最小时间间隔.
    base::TimeDelta min_timer_interval_;

    base::RepeatingTimer<AnimationContainer> timer_;

    Observer* observer_;

    DISALLOW_COPY_AND_ASSIGN(AnimationContainer);
};

#endif //__animation_container_h__