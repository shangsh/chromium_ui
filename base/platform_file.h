
#ifndef __platform_file_h__
#define __platform_file_h__

#pragma once

#include <windows.h>

#include "time.h"

namespace base
{

    class FilePath;

    typedef HANDLE PlatformFile;
    const PlatformFile kInvalidPlatformFileValue = INVALID_HANDLE_VALUE;

    enum PlatformFileFlags
    {
        PLATFORM_FILE_OPEN = 1,
        PLATFORM_FILE_CREATE = 2,
        PLATFORM_FILE_OPEN_ALWAYS = 4,          // ���ܻ��½��ļ�.
        PLATFORM_FILE_CREATE_ALWAYS = 8,        // ���ܻḲ�����ļ�.
        PLATFORM_FILE_READ = 16,
        PLATFORM_FILE_WRITE = 32,
        PLATFORM_FILE_EXCLUSIVE_READ = 64,      // EXCLUSIVE��Windows��SHARE�����෴.
        PLATFORM_FILE_EXCLUSIVE_WRITE = 128,
        PLATFORM_FILE_ASYNC = 256,
        PLATFORM_FILE_TEMPORARY = 512,          // ����Windows�Ͽ���.
        PLATFORM_FILE_HIDDEN = 1024,            // ����Windows�Ͽ���.
        PLATFORM_FILE_DELETE_ON_CLOSE = 2048,
        PLATFORM_FILE_TRUNCATE = 4096,
        PLATFORM_FILE_WRITE_ATTRIBUTES = 8192   // ����Windows�Ͽ���.
    };

    // �����ļ�ϵͳ���Ƶ��µĵ���ʧ�ܻ᷵��PLATFORM_FILE_ERROR_ACCESS_DENIED.
    // ���ڰ�ȫ���Ե��µĽ�ֹ�����᷵��PLATFORM_FILE_ERROR_SECURITY.
    enum PlatformFileError
    {
        PLATFORM_FILE_OK = 0,
        PLATFORM_FILE_ERROR_FAILED = -1,
        PLATFORM_FILE_ERROR_IN_USE = -2,
        PLATFORM_FILE_ERROR_EXISTS = -3,
        PLATFORM_FILE_ERROR_NOT_FOUND = -4,
        PLATFORM_FILE_ERROR_ACCESS_DENIED = -5,
        PLATFORM_FILE_ERROR_TOO_MANY_OPENED = -6,
        PLATFORM_FILE_ERROR_NO_MEMORY = -7,
        PLATFORM_FILE_ERROR_NO_SPACE = -8,
        PLATFORM_FILE_ERROR_NOT_A_DIRECTORY = -9,
        PLATFORM_FILE_ERROR_INVALID_OPERATION = -10,
        PLATFORM_FILE_ERROR_SECURITY = -11,
        PLATFORM_FILE_ERROR_ABORT = -12,
        PLATFORM_FILE_ERROR_NOT_A_FILE = -13,
        PLATFORM_FILE_ERROR_NOT_EMPTY = -14,
    };

    // ���ڱ����ļ�����Ϣ. ���Ҫ���ṹ������µ��ֶ�, ����ͬʱ����cpp�еĺ���.
    struct PlatformFileInfo
    {
        // �ļ��Ĵ�С(�ֽ�). ��is_directoryΪtrueʱδ����.
        int64 size;

        // �ļ���һ��Ŀ¼��Ϊtrue.
        bool is_directory;

        // �ļ�����޸�ʱ��.
        base::Time last_modified;

        // �ļ�������ʱ��.
        base::Time last_accessed;

        // �ļ�����ʱ��.
        base::Time creation_time;
    };

    // �������ߴ��ļ�. ���ʹ��PLATFORM_FILE_OPEN_ALWAYS, �Ҵ�����Ч��|created|,
    // �������ļ�ʱ����|created|Ϊtrue, ���ļ�ʱ����|created|Ϊfalse.
    // |error_code|����ΪNULL.
    PlatformFile CreatePlatformFile(const FilePath& name,
        int flags, bool* created, PlatformFileError* error_code);

    // �ر��ļ����. �ɹ�����|true|, ʧ�ܷ���|false|.
    bool ClosePlatformFile(PlatformFile file);

    // ��offsetƫ��λ�ÿ�ʼ��ȡһ���������ֽ�(��������EOF). ����ʵ�ʶ�ȡ���ֽ���,
    // ���󷵻�-1.
    int ReadPlatformFile(PlatformFile file, int64 offset, char* data, int size);

    // ��offsetƫ��λ�ô�дһ�黺�������ݵ��ļ�, �Ḳ��֮ǰ������. ����ʵ��д���
    // �ֽ���, ���󷵻�-1.
    int WritePlatformFile(PlatformFile file, int64 offset, const char* data,
        int size);

    // �����ļ��ĳ���. ���|length|���ڵ�ǰ�ļ�����, ���䲿�ֻ����0. �ļ�������
    // ����false.
    bool TruncatePlatformFile(PlatformFile file, int64 length);

    // �����ļ��Ļ��������ݵ�����.
    bool FlushPlatformFile(PlatformFile file);

    // �����ļ���������ʱ����޸�ʱ��.
    bool TouchPlatformFile(PlatformFile file, const Time& last_access_time,
        const Time& last_modified_time);

    // �����ļ��Ļ�����Ϣ.
    bool GetPlatformFileInfo(PlatformFile file, PlatformFileInfo* info);

    // PassPlatformFile���ڴ���PlatformFile������Ȩ��������, �౾���ӹ�
    // ����Ȩ.
    //
    // ʾ��:
    //
    //  void MaybeProcessFile(PassPlatformFile pass_file) {
    //    if(...) {
    //      PlatformFile file = pass_file.ReleaseValue();
    //      // Now, we are responsible for closing |file|.
    //    }
    //  }
    //
    //  void OpenAndMaybeProcessFile(const FilePath& path) {
    //    PlatformFile file = CreatePlatformFile(path, ...);
    //    MaybeProcessFile(PassPlatformFile(&file));
    //    if(file != kInvalidPlatformFileValue)
    //      ClosePlatformFile(file);
    //  }
    class PassPlatformFile
    {
    public:
        explicit PassPlatformFile(PlatformFile* value) : value_(value) {}

        // ���ض����д洢��PlatformFile, ֮������߻�ȡ����Ȩ, Ӧ�ø����ļ��Ĺر�.
        // �κκ����ĵ��ö������طǷ���PlatformFile.
        PlatformFile ReleaseValue()
        {
            PlatformFile temp = *value_;
            *value_ = kInvalidPlatformFileValue;
            return temp;
        }

    private:
        PlatformFile* value_;
    };

} //namespace base

#endif //__platform_file_h__