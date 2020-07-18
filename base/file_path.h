
#ifndef __file_path_h__
#define __file_path_h__

#pragma once

#include <windows.h>

#include <string>
#include <vector>

class Pickle;

namespace base
{

    class FilePath
    {
    public:
        // ����·����ʾ"��ǰĿ¼".
        static const wchar_t* kCurrentDirectory;
        // ����·����ʾ"��Ŀ¼".
        static const wchar_t* kParentDirectory;

        FilePath();
        FilePath(const FilePath& that);
        // path��NULL��β���ַ���.
        explicit FilePath(const std::wstring& path);
        ~FilePath();
        FilePath& operator=(const FilePath& that);

        bool operator==(const FilePath& that) const;
        bool operator!=(const FilePath& that) const;
        bool operator<(const FilePath& that) const;

        const wchar_t* value() const { return path_; }
        bool Empty() const { return wcslen(path_) == 0; }
        bool IsAbsolute() const;

        // ���·��.
        void Clear() { path_[0] = '\0'; }
        // ��β����ӷ�б��, ����Ѿ����������. ��ӳɹ�����true.
        bool AddBackslash();
        // �Ƴ�β���ķ�б��.
        void RemoveBackslash();
        // �����չ��(����".exe"). ��ӳɹ�����true.
        bool AddExtension(const wchar_t* ext);
        // ���·��.
        bool Append(const wchar_t* append);
        // ��·�����й淶��, �����µ�·��. ����:
        // "A:\name_1\.\name_2\..\name_3" -> "A:\name_1\name_3"
        FilePath Canonicalize() const;
        // ����·����������ɲ���.
        void GetComponents(std::vector<std::wstring>* components) const;
        // ���·��������չ�����Ƴ�.
        void RemoveExtension();
        // ���·��������չ�����滻, ������ӵ�����. �ɹ�����true.
        bool RenameExtension(const wchar_t* ext);
        // �Ƴ�ĩβ���ļ����ͷ�б��, �����򷵻�true, ���򷵻�false.
        bool RemoveFileSpec();
        // ȥ��·��, ֻʣ�ļ���.
        void StripPath();
        // ֻ�����̷�(����"C:\").
        bool StripToRoot();

        // ����·������չ��, �ɹ�����"."��λ��, ���򷵻�ĩβNULL�ַ��ĵ�ַ.
        const wchar_t* FindExtension() const;
        // ����·�����ļ���, �ɹ������ļ�����ʼλ��, ���򷵻�·����ʼ��ַ.
        const wchar_t* FindFileName() const;

        // ·����Ӧ���ļ�����Ŀ¼�Ƿ����
        bool IsExists() const;
        // ·���Ƿ�ΪĿ¼
        bool IsDirectory() const;
        // ·���Ƿ�Ϊ��Ŀ¼
        bool IsDirectoryEmpty() const;
        // �Ƿ���"."(��ǰ)Ŀ¼
        bool IsDotDirectory() const;
        // �Ƿ���".."(�ϼ�)Ŀ¼
        bool IsDotDotDirectory() const;

        // ��ȡϵͳ��ʱĿ¼
        static bool GetTempDir(FilePath& path);
        // ��ȡ��ǰĿ¼
        static bool GetCurrentDirectory(FilePath& dir);

        // ������дpickle�ľ�̬����.
        static void WriteStringTypeToPickle(Pickle* pickle,
            const std::wstring& path);
        static bool ReadStringTypeFromPickle(Pickle* pickle,
            void** iter, std::wstring* path);
        void WriteToPickle(Pickle* pickle);
        bool ReadFromPickle(Pickle* pickle, void** iter);

    private:
        wchar_t path_[MAX_PATH];
    };

} //namespace base

#define FILE_PATH_LITERAL(x) L ## x
#define PRFilePath "ls"
#define PRFilePathLiteral L"%ls"

#endif //__file_path_h__