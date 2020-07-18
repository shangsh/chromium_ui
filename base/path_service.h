
#ifndef __path_service_h__
#define __path_service_h__

#pragma once

namespace base
{

    class FilePath;

    enum
    {
        PATH_START = 0,

        DIR_CURRENT,    // ��ǰĿ¼.
        DIR_EXE,        // ����FILE_EXE��Ŀ¼.
        DIR_MODULE,     // ����FILE_MODULE��Ŀ¼.
        DIR_TEMP,       // ��ʱĿ¼.
        FILE_EXE,       // ��ִ�г����ȫ·��.
        FILE_MODULE,    // ��ǰģ���ȫ·��.
        PATH_END
    };

    enum
    {
        PATH_WIN_START = 100,

        DIR_WINDOWS,                // WindowsĿ¼ c:\windows
        DIR_SYSTEM,                 // ϵͳĿ¼ c:\windows\system32
        DIR_PROGRAM_FILES,          // ����Ŀ¼ c:\program files

        DIR_IE_INTERNET_CACHE,      // Internet��ʱ�ļ�Ŀ¼.
        DIR_COMMON_START_MENU,      // ��ʼ�˵�Ŀ¼ C:\Documents and Settings\All Users\
                                    // Start Menu\Programs
        DIR_START_MENU,             // ��ʼ�˵�Ŀ¼ C:\Documents and Settings\<user>\
                                    // Start Menu\Programs
        DIR_COMMON_APP_DATA,        // Ӧ������Ŀ¼ All Users\Application Data
        DIR_APP_DATA,               // Ӧ������Ŀ¼ <user>\Application Data
        DIR_PROFILE,                // �û�Ŀ¼ C:\Documents and settings\<user>
        DIR_LOCAL_APP_DATA_LOW,     // �������Լ���Ӧ������Ŀ¼
        DIR_LOCAL_APP_DATA,         // �ֲ�Ӧ������Ŀ¼ Local Settings\Application Data
        PATH_WIN_END
    };

    bool PathProvider(int key, FilePath* result);

} //namespace base

#endif //__path_service_h__