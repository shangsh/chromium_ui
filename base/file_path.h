
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
        // 特殊路径表示"当前目录".
        static const wchar_t* kCurrentDirectory;
        // 特殊路径表示"父目录".
        static const wchar_t* kParentDirectory;

        FilePath();
        FilePath(const FilePath& that);
        // path是NULL结尾的字符串.
        explicit FilePath(const std::wstring& path);
        ~FilePath();
        FilePath& operator=(const FilePath& that);

        bool operator==(const FilePath& that) const;
        bool operator!=(const FilePath& that) const;
        bool operator<(const FilePath& that) const;

        const wchar_t* value() const { return path_; }
        bool Empty() const { return wcslen(path_) == 0; }
        bool IsAbsolute() const;

        // 清除路径.
        void Clear() { path_[0] = '\0'; }
        // 在尾部添加反斜线, 如果已经存在则不添加. 添加成功返回true.
        bool AddBackslash();
        // 移除尾部的反斜线.
        void RemoveBackslash();
        // 添加扩展名(比如".exe"). 添加成功返回true.
        bool AddExtension(const wchar_t* ext);
        // 添加路径.
        bool Append(const wchar_t* append);
        // 对路径进行规范化, 返回新的路径. 比如:
        // "A:\name_1\.\name_2\..\name_3" -> "A:\name_1\name_3"
        FilePath Canonicalize() const;
        // 返回路径的所有组成部分.
        void GetComponents(std::vector<std::wstring>* components) const;
        // 如果路径存在扩展名则移除.
        void RemoveExtension();
        // 如果路径存在扩展名则替换, 否则添加到后面. 成功返回true.
        bool RenameExtension(const wchar_t* ext);
        // 移除末尾的文件名和反斜杠, 存在则返回true, 否则返回false.
        bool RemoveFileSpec();
        // 去掉路径, 只剩文件名.
        void StripPath();
        // 只保留盘符(比如"C:\").
        bool StripToRoot();

        // 查找路径的扩展名, 成功返回"."的位置, 否则返回末尾NULL字符的地址.
        const wchar_t* FindExtension() const;
        // 查找路径的文件名, 成功返回文件名起始位置, 否则返回路径起始地址.
        const wchar_t* FindFileName() const;

        // 路径对应的文件或者目录是否存在
        bool IsExists() const;
        // 路径是否为目录
        bool IsDirectory() const;
        // 路径是否为空目录
        bool IsDirectoryEmpty() const;
        // 是否是"."(当前)目录
        bool IsDotDirectory() const;
        // 是否是".."(上级)目录
        bool IsDotDotDirectory() const;

        // 获取系统临时目录
        static bool GetTempDir(FilePath& path);
        // 获取当前目录
        static bool GetCurrentDirectory(FilePath& dir);

        // 辅助读写pickle的静态函数.
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