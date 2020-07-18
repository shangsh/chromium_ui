
#include "windows_version.h"

#include <windows.h>

#include "logging.h"

namespace base
{

    WinVersion GetWinVersion()
    {
        static bool checked_version = false;
        static WinVersion win_version = WINVERSION_PRE_2000;
        if(!checked_version)
        {
            OSVERSIONINFOEX version_info;
            version_info.dwOSVersionInfoSize = sizeof(version_info);
            GetVersionEx(reinterpret_cast<OSVERSIONINFO*>(&version_info));
            if(version_info.dwMajorVersion == 5)
            {
                switch(version_info.dwMinorVersion)
                {
                case 0:
                    win_version = WINVERSION_2000;
                    break;
                case 1:
                    win_version = WINVERSION_XP;
                    break;
                case 2:
                default:
                    win_version = WINVERSION_SERVER_2003;
                    break;
                }
            }
            else if(version_info.dwMajorVersion == 6)
            {
                if(version_info.wProductType != VER_NT_WORKSTATION)
                {
                    // 2008��6.0, 2008 R2��6.1.
                    win_version = WINVERSION_2008;
                }
                else
                {
                    if(version_info.dwMinorVersion == 0)
                    {
                        win_version = WINVERSION_VISTA;
                    }
                    else
                    {
                        win_version = WINVERSION_WIN7;
                    }
                }
            }
            else if(version_info.dwMajorVersion > 6)
            {
                win_version = WINVERSION_WIN7;
            }
            checked_version = true;
        }
        return win_version;
    }

    void GetServicePackLevel(int* major, int* minor)
    {
        DCHECK(major && minor);
        static bool checked_version = false;
        static int service_pack_major = -1;
        static int service_pack_minor = -1;
        if(!checked_version)
        {
            OSVERSIONINFOEX version_info = { 0 };
            version_info.dwOSVersionInfoSize = sizeof(version_info);
            GetVersionEx(reinterpret_cast<OSVERSIONINFOW*>(&version_info));
            service_pack_major = version_info.wServicePackMajor;
            service_pack_minor = version_info.wServicePackMinor;
            checked_version = true;
        }

        *major = service_pack_major;
        *minor = service_pack_minor;
    }

} //namespace base