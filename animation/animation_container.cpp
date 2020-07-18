
#include "animation_container.h"

using base::TimeDelta;
using base::TimeTicks;

AnimationContainer::AnimationContainer()
: last_tick_time_(TimeTicks::Now()), observer_(NULL) {}

AnimationContainer::~AnimationContainer()
{
    // animationsӵ���������, ɾ��ǰ��ֹͣ. ���elements_��Ϊ��, ˵����
    // ������.
    DCHECK(elements_.empty());
}

void AnimationContainer::Start(Element* element)
{
    DCHECK(elements_.count(element) == 0); // ֻ�е�Ԫ��û���е�ʱ�������.

    if(elements_.empty())
    {
        last_tick_time_ = TimeTicks::Now();
        SetMinTimerInterval(element->GetTimerInterval());
    }
    else if(element->GetTimerInterval() < min_timer_interval_)
    {
        SetMinTimerInterval(element->GetTimerInterval());
    }

    element->SetStartTime(last_tick_time_);
    elements_.insert(element);
}

void AnimationContainer::Stop(Element* element)
{
    DCHECK(elements_.count(element) > 0); // Ԫ�ر��������е�.

    elements_.erase(element);

    if(elements_.empty())
    {
        timer_.Stop();
        if(observer_)
        {
            observer_->AnimationContainerEmpty(this);
        }
    }
    else
    {
        TimeDelta min_timer_interval = GetMinInterval();
        if(min_timer_interval > min_timer_interval_)
        {
            SetMinTimerInterval(min_timer_interval);
        }
    }
}

void AnimationContainer::Run()
{
    // �ڸ�������Ԫ�غ�֪ͨ�۲���. �������Ԫ���ڸ���ʱ����ɾ��, ��ô���ǵ����ü���
    // �����Ϊ0, ��֪ͨ�۲��ߺ�Ҳ�ᱻɾ��. �������һ�����ñ�֤��������Ԫ�غ����
    // ���ǺϷ���.
    scoped_refptr<AnimationContainer> this_ref(this);

    TimeTicks current_time = TimeTicks::Now();

    last_tick_time_ = current_time;

    // ����һ�����ڱ���, �����ڵ���Stepʱ�Ƴ��κ�Ԫ�ض�����������.
    Elements elements = elements_;

    for(Elements::const_iterator i=elements.begin(); i!=elements.end(); ++i)
    {
        // ȷ��Ԫ���ǺϷ���.
        if(elements_.find(*i) != elements_.end())
        {
            (*i)->Step(current_time);
        }
    }

    if(observer_)
    {
        observer_->AnimationContainerProgressed(this);
    }
}

void AnimationContainer::SetMinTimerInterval(base::TimeDelta delta)
{
    // �����Ǻ͵�ǰԪ�ص�ʱ���, ���ʹ��Animation/AnimationContainer��˵������
    // ��.
    timer_.Stop();
    min_timer_interval_ = delta;
    timer_.Start(min_timer_interval_, this, &AnimationContainer::Run);
}

TimeDelta AnimationContainer::GetMinInterval()
{
    DCHECK(!elements_.empty());

    TimeDelta min;
    Elements::const_iterator i = elements_.begin();
    min = (*i)->GetTimerInterval();
    for(++i; i!=elements_.end(); ++i)
    {
        if((*i)->GetTimerInterval() < min)
        {
            min = (*i)->GetTimerInterval();
        }
    }
    return min;
}