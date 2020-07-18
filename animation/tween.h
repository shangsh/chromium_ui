
#ifndef __tween_h__
#define __tween_h__

#pragma once

#include "base/basic_types.h"

namespace gfx
{
    class Rect;
}

class Tween
{
public:
    enum Type
    {
        LINEAR,        // Linear.
        EASE_OUT,      // Fast in, slow out (default).
        EASE_IN,       // Slow in, fast out.
        EASE_IN_OUT,   // Slow in and out, fast in the middle.
        FAST_IN_OUT,   // Fast in and out, slow in the middle.
        EASE_OUT_SNAP, // Fast in, slow out, snap to final value.
        ZERO,          // Returns a value of 0 always.
    };

    // ���ݲ������ͷ���state����ֵ. |state|��ֵ��0��1.
    static double CalculateValue(Type type, double state);

    // �����ȡstart��end֮���һ��ֵ.
    static double ValueBetween(double value, double start, double target);
    static int ValueBetween(double value, int start, int target);
    static gfx::Rect ValueBetween(double value,
        const gfx::Rect& start_bounds, const gfx::Rect& target_bounds);

private:
    Tween();
    ~Tween();

    DISALLOW_COPY_AND_ASSIGN(Tween);
};

#endif //__tween_h__