
#ifndef __file_util_h__
#define __file_util_h__

#pragma once

#include <stack>
#include <string>
#include <vector>

#include "file_path.h"
#include "platform_file.h"
#include "time.h"

namespace base
{

    //-----------------------------------------------------------------------------
    // ǣ�����ļ�ϵͳ���ʺ��޸ĵĺ���:

    // ����pathĿ¼����ʱ��|comparison_time|���߸��������ļ�����.
    // ����".."����".", Ŀ¼���ݹ����.
    int CountFilesCreatedAfter(const FilePath& path,
        const base::Time& comparison_time);

    // ����|root_path|��ȫ���ļ������ֽ���. ���|root_path|�����ڷ���0.
    //
    // ������ʵ��ʹ����FileEnumerator��, �����κ�ƽ̨�¶�����ܿ�.
    int64 ComputeDirectorySize(const FilePath& root_path);

    // ����|directory|��(���ݹ�)ƥ��ģʽ|pattern|��ȫ���ļ������ֽ���.
    // ���|directory|�����ڷ���0.
    //
    // ������ʵ��ʹ����FileEnumerator��, �����κ�ƽ̨�¶�����ܿ�.
    int64 ComputeFilesSize(const FilePath& directory,
        const std::wstring& pattern);

    // ɾ��·��, �������ļ�����Ŀ¼. �����Ŀ¼, ��recursiveΪtrueʱ,
    // ��ɾ��Ŀ¼�������ݰ�����Ŀ¼, �����Ƴ�Ŀ¼(��Ŀ¼).
    // �ɹ�����true, ���򷵻�false.
    //
    // ����: ʹ��recursive==true�ȼ���"rm -rf", ��ҪС��.
    bool Delete(const FilePath& path, bool recursive);

    // ���Ȳ���ϵͳ���´�������ʱ��ɾ���ļ���Ŀ¼.
    // ע��:
    // 1) ��ɾ�����ļ�/Ŀ¼Ӧ������ʱĿ¼.
    // 2) ��ɾ����Ŀ¼����Ϊ��.
    bool DeleteAfterReboot(const FilePath& path);


    // �����ļ����Լ�·���ϲ����ڵ�Ŀ¼.
    // in:
    //      path                ȫ·��, ���������·��
    //      sa                  ��ȫ����
    //
    // returns:
    //      ERROR_SUCCESS (0)   �ɹ�
    //      ERROR_              ʧ��(ע��ERROR_FILE_EXISTS��ERROR_ALREADY_EXISTS)
    int EnsureDirectory(const FilePath& path, SECURITY_ATTRIBUTES* sa);
    int EnsureDirectoryForFile(const FilePath& path, SECURITY_ATTRIBUTES* sa);

    // �����ļ�·������Ϣ.
    bool GetFileInfo(const FilePath& file_path, base::PlatformFileInfo* info);

    // ��װfopen����. �ɹ����طǿյ�FILE*.
    FILE* OpenFile(const FilePath& filename, const char* mode);

    // �ر�OpenFile�򿪵��ļ�. �ɹ�����true.
    bool CloseFile(FILE* file);

    // ��ȡ���̵�ǰ����Ŀ¼.
    bool GetCurrentDirectory(FilePath* path);

    // ���ý��̵�ǰ����Ŀ¼.
    bool SetCurrentDirectory(const FilePath& path);

    // ö��·���������ļ�, ����֤����.
    // ����������������ʽ, ��Ҫ�����߳���ʹ��.
    class FileEnumerator
    {
    public:
        typedef WIN32_FIND_DATA FindInfo;

        enum FILE_TYPE
        {
            FILES                 = 1 << 0,
            DIRECTORIES           = 1 << 1,
            FILES_DIRS            = FILES | DIRECTORIES,
        };

        // |root_path|�Ǳ�������ʼĿ¼, ���ܲ��Է�б�߽�β.
        //
        // ���|recursive|��true, ��ݹ������Ŀ¼. ���ù�����ȱ�����ʽ, ����
        // ��ǰĿ¼���ļ�������Ŀ¼���ļ�����.
        //
        // |file_type|ָ����ƥ���ļ�����Ŀ¼�������߶�ƥ��.
        //
        // |pattern|�ǿ�ѡ���ļ�ƥ��ģʽ, ʵ��shell�Ĳ�������, ����"*.txt"����
        // "Foo???.doc". ����ĳЩƥ��ģʽ���ǿ�ƽ̨��, ��Ϊ�ײ���õ���OS���
        // ����. һ����˵, Windows��ƥ������Ҫ��������ƽ̨, ���Ȳ���. ���û
        // ָ��, ƥ�������ļ�.
        // ע��: ƥ��ģʽ���޶�root_pathĿ¼��Ч, �ݹ����Ŀ¼�²�������.
        FileEnumerator(const FilePath& root_path,
            bool recursive,
            FileEnumerator::FILE_TYPE file_type);
        FileEnumerator(const FilePath& root_path,
            bool recursive,
            FileEnumerator::FILE_TYPE file_type,
            const std::wstring& pattern);
        ~FileEnumerator();

        // ���û����һ��·�����ؿ��ַ���.
        FilePath Next();

        // д�ļ���Ϣ��|info|.
        void GetFindInfo(FindInfo* info);

        // ���FindInfo�ǲ���Ŀ¼.
        static bool IsDirectory(const FindInfo& info);

        static FilePath GetFilename(const FindInfo& find_info);

    private:
        // ���ö�ٿ�����������·������true.
        bool ShouldSkip(const FilePath& path);

        // ��find_data_�Ϸ�ʱΪtrue.
        bool has_find_data_;
        WIN32_FIND_DATA find_data_;
        HANDLE find_handle_;

        FilePath root_path_;
        bool recursive_;
        FILE_TYPE file_type_;
        std::wstring pattern_; // ������ƥ��ʱ�ַ���Ϊ��.

        // ��¼������Ȳ����л���Ҫ��������Ŀ¼.
        std::stack<FilePath> pending_paths_;

        DISALLOW_COPY_AND_ASSIGN(FileEnumerator);
    };

    class MemoryMappedFile
    {
    public:
        // ȱʡ���캯��, ���г�Ա������Ϊ�Ƿ�/��ֵ.
        MemoryMappedFile();
        ~MemoryMappedFile();

        // ��һ�����ڵ��ļ���ӳ�䵽�ڴ���. ����Ȩ����Ϊֻ��. ��������Ѿ�ָ��һ��
        // �Ϸ����ڴ�ӳ���ļ�, ���û�ʧ�ܲ�����false. ����޷����ļ����ļ�������
        // �����ڴ�ӳ��ʧ��, ��������false. �Ժ���ܻ�����ָ������Ȩ��.
        bool Initialize(const FilePath& file_name);
        // ������һ��, ֻ���ļ��������Ѵ򿪵�. MemoryMappedFile��ӹ�|file|������Ȩ,
        // ����֮���ر�.
        bool Initialize(PlatformFile file);

        const uint8* data() const { return data_; }
        size_t length() const { return length_; }

        // file_��ָ��һ���򿪵��ڴ�ӳ���ļ��ĺϷ������?
        bool IsValid();

    private:
        // ��ָ���ļ�, ���ݸ�MapFileToMemoryInternal().
        bool MapFileToMemory(const FilePath& file_name);

        // ӳ���ļ����ڴ�, ����data_Ϊ�ڴ��ַ. ����ɹ�����true, �κ�ʧ�����ζ���
        // ����false. Initialize()�ĸ�������.
        bool MapFileToMemoryInternal();

        // �ر����д򿪵ľ��. �����Ժ���ܻ��ɹ�����.
        void CloseHandles();

        base::PlatformFile file_;
        HANDLE file_mapping_;
        uint8* data_;
        size_t length_;

        DISALLOW_COPY_AND_ASSIGN(MemoryMappedFile);
    };

} //namespace base

#endif //__file_util_h__