
#ifndef __sys_info_h__
#define __sys_info_h__

#pragma once

#include <string>

#include "basic_types.h"

namespace base
{

    class FilePath;

    class SysInfo
    {
    public:
        // ���ر����߼�������/�˵�����.
        static int NumberOfProcessors();

        // ���ر��������ڴ�����.
        static int64 AmountOfPhysicalMemory();

        // ���ر��������ڴ�����(��λM).
        static int AmountOfPhysicalMemoryMB()
        {
            return static_cast<int>(AmountOfPhysicalMemory() / 1024 / 1024);
        }

        // ��ȡ|path|���ڴ��̵�ʣ��ռ��ֽ���, ʧ�ܷ���-1.
        static int64 AmountOfFreeDiskSpace(const FilePath& path);

        // ���ز���ϵͳ������.
        static std::string OperatingSystemName();

        // ���ز���ϵͳ�İ汾.
        static std::string OperatingSystemVersion();

        // ���ز���ϵͳ�汾����ֵ.
        static void OperatingSystemVersionNumbers(int32* major_version,
            int32* minor_version, int32* bugfix_version);

        // ����ϵͳ��CPU�ܹ�, ��ȷֵ�ڲ�ͬƽ̨���в��.
        static std::string CPUArchitecture();

        // ��������ʾ��ˮƽ��ֱ������.
        static void GetPrimaryDisplayDimensions(int* width, int* height);

        // ������ʾ������.
        static int DisplayCount();

        // ����VMϵͳ�������С�ڴ�����.
        static size_t VMAllocationGranularity();
    };

} //namespace base

#endif //__sys_info_h__