
#ifndef __rand_util_h__
#define __rand_util_h__

#pragma once

#include "basic_types.h"

namespace base
{

    // ����[0, kuint64max]����������. �̰߳�ȫ.
    uint64 RandUint64();

    // ����[min, max]����������. �̰߳�ȫ.
    int RandInt(int min, int max);

    // ����[0, max)����������. �̰߳�ȫ.
    //
    // ������std::random_shuffle()��������:
    // ����|std::vector<int> myvector|, ����������
    //     std::random_shuffle(myvector.begin(), myvector.end(), base::RandGenerator);
    uint64 RandGenerator(uint64 max);

    // ����[0, 1)����������. �̰߳�ȫ.
    double RandDouble();

} //namespace base


#endif //__rand_util_h__