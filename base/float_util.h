
#ifndef __float_util_h__
#define __float_util_h__

#pragma once

#include <float.h>

namespace base
{

    inline bool IsFinite(const double& number)
    {
        return _finite(number) != 0;
    }

} //namespace base

#endif //__float_util_h__