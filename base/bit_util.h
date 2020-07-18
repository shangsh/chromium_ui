
#ifndef __bit_util_h__
#define __bit_util_h__

#pragma once

#include "basic_types.h"
#include "logging.h"

namespace base
{

    // ��������i����2^i<=n<2^(i+1)
    inline int Log2Floor(uint32 n)
    {
        if(n == 0)
        {
            return -1;
        }
        int log = 0;
        uint32 value = n;
        for(int i=4; i>=0; --i)
        {
            int shift = (1 << i);
            uint32 x = value >> shift;
            if(x != 0)
            {
                value = x;
                log += shift;
            }
        }
        DCHECK_EQ(value, 1u);
        return log;
    }

    // ��������i����2^(i-1)<n<=2^i
    inline int Log2Ceiling(uint32 n)
    {
        if(n == 0)
        {
            return -1;
        }
        else
        {
            // ��n==0ʱLog2Floor����-1, ��������Ĵ������n==1ʱ����.
            return 1 + Log2Floor(n-1);
        }
    }

} //namespace base

#endif //__bit_util_h__