
#include "path_service.h"

#include <windows.h>
#include <shlobj.h>

#include "file_path.h"
#include "windows_version.h"

namespace base
{

    // http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
    extern "C" IMAGE_DOS_HEADER __ImageBase;
    bool PathProvider(int key, FilePath* result)
    {
        // ��Ҫ����ֵ�ļ���. ֧�ֳ���MAX_PATH��·�������, ����ϵͳ������Ƶ�ʱ��
        // ����GetTempPath���ⶼ��֧��.
        wchar_t system_buffer[MAX_PATH] = { 0 };
        system_buffer[0] = 0;

        FilePath cur;
        switch(key)
        {
        case DIR_CURRENT:
            if(!FilePath::GetCurrentDirectory(cur))
            {
                return false;
            }
            break;
        case DIR_EXE:
            GetModuleFileNameW(NULL, system_buffer, MAX_PATH);
            cur = FilePath(system_buffer);
            cur.RemoveFileSpec();
            break;
        case DIR_MODULE:
            {
                // ������õ�ģ�鶼����Դ, ����dll��exe.
                HMODULE this_module = reinterpret_cast<HMODULE>(&__ImageBase);
                ::GetModuleFileNameW(this_module, system_buffer, MAX_PATH);
                cur = FilePath(system_buffer);
                cur.RemoveFileSpec();
                break;
            }
        case DIR_TEMP:
            if(!FilePath::GetTempDir(cur))
            {
                return false;
            }
            break;
        case FILE_EXE:
            GetModuleFileNameW(NULL, system_buffer, MAX_PATH);
            cur = FilePath(system_buffer);
            break;
        case FILE_MODULE:
            {
                // ������õ�ģ�鶼����Դ, ����dll��exe.
                HMODULE this_module = reinterpret_cast<HMODULE>(&__ImageBase);
                GetModuleFileNameW(this_module, system_buffer, MAX_PATH);
                cur = FilePath(system_buffer);
                break;
            }
        case DIR_WINDOWS:
            GetWindowsDirectoryW(system_buffer, MAX_PATH);
            cur = FilePath(system_buffer);
            break;
        case DIR_SYSTEM:
            GetSystemDirectoryW(system_buffer, MAX_PATH);
            cur = FilePath(system_buffer);
            break;
        case DIR_PROGRAM_FILES:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_PROGRAM_FILES, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        case DIR_IE_INTERNET_CACHE:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_INTERNET_CACHE, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        case DIR_COMMON_START_MENU:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        case DIR_START_MENU:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        case DIR_COMMON_APP_DATA:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        case DIR_APP_DATA:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        case DIR_PROFILE:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        case DIR_LOCAL_APP_DATA_LOW:
            if(GetWinVersion() < WINVERSION_VISTA)
            {
                return false;
            }
            // TODO: Ӧ��ʹ��SHGetKnownFolderPath�滻. Bug 1281128.
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            cur.RemoveFileSpec();
            cur.Append(L"LocalLow");
            break;
        case DIR_LOCAL_APP_DATA:
            if(FAILED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL,
                SHGFP_TYPE_CURRENT, system_buffer)))
            {
                return false;
            }
            cur = FilePath(system_buffer);
            break;
        default:
            return false;
        }

        *result = cur;
        return true;
    }

} //namespace base