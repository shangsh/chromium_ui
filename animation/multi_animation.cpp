
#include "multi_animation.h"

// ȱʡʱ�Ӽ��, ����.
static const int kDefaultInterval = 20;

static int TotalTime(const MultiAnimation::Parts& parts)
{
    int time_ms = 0;
    for(size_t i=0; i<parts.size(); ++i)
    {
        DCHECK(parts[i].end_time_ms-parts[i].start_time_ms >= parts[i].time_ms);
        time_ms += parts[i].time_ms;
    }
    return time_ms;
}

MultiAnimation::MultiAnimation(const Parts& parts)
: Animation(base::TimeDelta::FromMilliseconds(kDefaultInterval)),
parts_(parts),
cycle_time_ms_(TotalTime(parts)),
current_value_(0),
current_part_index_(0)
{
    DCHECK(!parts_.empty());
}

MultiAnimation::~MultiAnimation() {}

void MultiAnimation::Step(base::TimeTicks time_now)
{
    double last_value = current_value_;
    size_t last_index = current_part_index_;

    int delta = static_cast<int>((time_now - start_time()).InMilliseconds() %
        cycle_time_ms_);
    const Part& part = GetPart(&delta, &current_part_index_);
    double percent = static_cast<double>(delta + part.start_time_ms) /
        static_cast<double>(part.end_time_ms);
    DCHECK(percent <= 1);
    current_value_ = Tween::CalculateValue(part.type, percent);

    if((current_value_!=last_value || current_part_index_!=last_index) &&
        delegate())
    {
        delegate()->AnimationProgressed(this);
    }
}

const MultiAnimation::Part& MultiAnimation::GetPart(int* time_ms,
                                                    size_t* part_index)
{
    DCHECK(*time_ms < cycle_time_ms_);

    for(size_t i=0; i<parts_.size(); ++i)
    {
        if(*time_ms < parts_[i].time_ms)
        {
            *part_index = i;
            return parts_[i];
        }

        *time_ms -= parts_[i].time_ms;
    }
    NOTREACHED();
    *time_ms = 0;
    *part_index = 0;
    return parts_[0];
}