
#ifndef __multi_animation_h__
#define __multi_animation_h__

#pragma once

#include <vector>

#include "animation.h"
#include "tween.h"

// MultiAnimation����϶���, ����һЩ�Ӷ����Ķ���. ����parts����MultiAnimation,
// ����Start(), ����������ʱ���֪ͨ����. MultiAnimationһֱ����ֱ������Stop.
class MultiAnimation : public Animation
{
public:
    // ���ֶ����Ķ���. ÿ�����ֶ���������Щ���:
    //
    // time_ms: �ⲿ�ֵ�ʱ��.
    // start_time_ms: ���ڼ��㶯����ʼλ�õİٷֱ�.
    // end_time_ms: ���ڼ��㶯������λ�õİٷֱ�.
    //
    // ����������, |start_time_ms|=0, |end_time_ms|=|time_ms|. �����Ҫ�õ���ͬ
    // ��Ч��, ���Ե���start/end. ����ִ��һ��200ms������[0.25, 0.75]����, ʹ����
    // ����ֵ: 200��100��400.
    struct Part
    {
        Part() : time_ms(0), start_time_ms(0),
            end_time_ms(0), type(Tween::ZERO) {}
        Part(int time_ms, Tween::Type type) : time_ms(time_ms),
            start_time_ms(0), end_time_ms(time_ms), type(type) {}

        int time_ms;
        int start_time_ms;
        int end_time_ms;
        Tween::Type type;
    };

    typedef std::vector<Part> Parts;

    explicit MultiAnimation(const Parts& parts);
    virtual ~MultiAnimation();

    // ���ص�ǰֵ. MultiAnimation�ĵ�ǰֵ�ɵ�ǰ���ֶ����Ĳ�������ȷ��.
    virtual double GetCurrentValue() const { return current_value_; }

    // ���ص�ǰ���ֶ���������.
    size_t current_part_index() const { return current_part_index_; }

protected:
    virtual void Step(base::TimeTicks time_now);

private:
    // ���ظ���ʱ��Ĳ��ֶ���. |time_ms|�ᱻ����Ϊ���������ֶ�����ʱ��,
    // |part_index|��������ֶ���������.
    const Part& GetPart(int* time_ms, size_t* part_index);

    // ������������ɲ���.
    const Parts parts_;

    // ���в��ֶ�������ʱ��.
    const int cycle_time_ms_;

    // �����ĵ�ǰֵ.
    double current_value_;

    // ��ǰ���ֶ���������.
    size_t current_part_index_;

    DISALLOW_COPY_AND_ASSIGN(MultiAnimation);
};

#endif //__multi_animation_h__