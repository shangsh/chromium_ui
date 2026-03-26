
#ifndef __resource_util_h__
#define __resource_util_h__

#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

namespace base
{

    // 从dll中获取数据资源.
    bool GetDataResourceFromModule(HMODULE module, int resource_id,
        void** data, size_t* length);

} //namespace base

#endif //__resource_util_h__