
#include "animation.h"

#include "base/windows_version.h"

#include "gfx/rect.h"

#include "tween.h"

Animation::Animation(base::TimeDelta timer_interval)
: timer_interval_(timer_interval),
is_animating_(false), delegate_(NULL) {}

Animation::~Animation()
{
    // �����������в�Ҫ����֪ͨ. ���ܴ���ӵ���������, ��Ҳ����ɾ��.
    if(is_animating_)
    {
        container_->Stop(this);
    }
}

void Animation::Start()
{
    if(is_animating_)
    {
        return;
    }

    if(!container_.get())
    {
        container_ = new AnimationContainer();
    }

    is_animating_ = true;

    container_->Start(this);

    AnimationStarted();
}

void Animation::Stop()
{
    if(!is_animating_)
    {
        return;
    }

    is_animating_ = false;

    // ��֪ͨ����, ��Ϊ�����п��ܻ�ɾ���������.
    container_->Stop(this);

    AnimationStopped();

    if(delegate_)
    {
        if(ShouldSendCanceledFromStop())
        {
            delegate_->AnimationCanceled(this);
        }
        else
        {
            delegate_->AnimationEnded(this);
        }
    }
}

double Animation::CurrentValueBetween(double start, double target) const
{
    return Tween::ValueBetween(GetCurrentValue(), start, target);
}

int Animation::CurrentValueBetween(int start, int target) const
{
    return Tween::ValueBetween(GetCurrentValue(), start, target);

}

gfx::Rect Animation::CurrentValueBetween(const gfx::Rect& start_bounds,
                                         const gfx::Rect& target_bounds) const
{
    return Tween::ValueBetween(GetCurrentValue(), start_bounds, target_bounds);
}

void Animation::SetContainer(AnimationContainer* container)
{
    if(container == container_.get())
    {
        return;
    }

    if(is_animating_)
    {
        container_->Stop(this);
    }

    if(container)
    {
        container_ = container;
    }
    else
    {
        container_ = new AnimationContainer();
    }

    if(is_animating_)
    {
        container_->Start(this);
    }
}

// static
bool Animation::ShouldRenderRichAnimation()
{
    if(base::GetWinVersion() >= base::WINVERSION_VISTA)
    {
        BOOL result;
        // ��ȡ"�ر����в���Ҫ�Ķ���"ֵ.
#ifndef SPI_GETCLIENTAREAANIMATION
#define SPI_GETCLIENTAREAANIMATION 0x1042
#endif
        if(SystemParametersInfo(SPI_GETCLIENTAREAANIMATION, 0, &result, 0))
        {
            // ò��MSDN�ĵ�(2009����)��һ������:
            //   http://msdn.microsoft.com/en-us/library/ms724947(VS.85).aspx
            // �ĵ���˵��������ֹʱ����TRUE, ��ʵ����������ʱ����TRUE.
            return !!result;
        }
    }
    return !::GetSystemMetrics(SM_REMOTESESSION);
}

void Animation::SetStartTime(base::TimeTicks start_time)
{
    start_time_ = start_time;
}