
#ifndef __windows_version_h__
#define __windows_version_h__

#pragma once

namespace base
{

    // 注意: 保持枚举顺序以便调用者可以"if(GetWinVersion() > WINVERSION_2000) ...".
    // 枚举值是可以修改的.
    enum WinVersion
    {
        WINVERSION_PRE_2000 = 0, // 不支持.
        WINVERSION_2000 = 1, // 不支持.
        WINVERSION_XP = 2,
        WINVERSION_SERVER_2003 = 3,
        WINVERSION_VISTA = 4,
        WINVERSION_2008 = 5,
        WINVERSION_WIN7 = 6,
    };

    // 返回当前运行的Windows版本.
    WinVersion GetWinVersion();

    // 返回安装的SP包的主要和次要版本号.
    void GetServicePackLevel(int* major, int* minor);

} //namespace base

#endif //__windows_version_h__