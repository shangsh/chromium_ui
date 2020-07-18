
#ifndef __windows_version_h__
#define __windows_version_h__

#pragma once

namespace base
{

    // ע��: ����ö��˳���Ա�����߿���"if(GetWinVersion() > WINVERSION_2000) ...".
    // ö��ֵ�ǿ����޸ĵ�.
    enum WinVersion
    {
        WINVERSION_PRE_2000 = 0, // ��֧��.
        WINVERSION_2000 = 1, // ��֧��.
        WINVERSION_XP = 2,
        WINVERSION_SERVER_2003 = 3,
        WINVERSION_VISTA = 4,
        WINVERSION_2008 = 5,
        WINVERSION_WIN7 = 6,
    };

    // ���ص�ǰ���е�Windows�汾.
    WinVersion GetWinVersion();

    // ���ذ�װ��SP������Ҫ�ʹ�Ҫ�汾��.
    void GetServicePackLevel(int* major, int* minor);

} //namespace base

#endif //__windows_version_h__