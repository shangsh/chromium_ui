
#include "file_path.h"

#include <shlwapi.h>

#include "logging.h"
#include "pickle.h"

namespace base
{

    const wchar_t* FilePath::kCurrentDirectory = L".";
    const wchar_t* FilePath::kParentDirectory = L"..";

    static const FilePath kCurrentDirectoryPath(FilePath::kCurrentDirectory);
    static const FilePath kParentDirectoryPath(FilePath::kParentDirectory);

    FilePath::FilePath()
    {
        path_[0] = '\0';
    }

    FilePath::FilePath(const FilePath& that)
    {
#pragma warning(suppress : 4996) // 不输出"反对使用wcscpy"的警告.
        wcscpy(path_, that.path_);
    }

    FilePath::FilePath(const std::wstring& path)
    {
        if(path.empty())
        {
            path_[0] = '\0';
        }
        else
        {
#pragma warning(suppress : 4996) // 不输出"反对使用wcscpy"的警告.
            wcscpy(path_, path.c_str());
        }
    }

    FilePath::~FilePath() {}

    FilePath& FilePath::operator=(const FilePath& that)
    {
#pragma warning(suppress : 4996) // 不输出"反对使用wcscpy"的警告.
        wcscpy(path_, that.path_);
        return *this;
    }

    bool FilePath::operator==(const FilePath& that) const
    {
#pragma warning(suppress : 4996) // 不输出"反对使用wcsicmp"的警告.
        return wcsicmp(path_, that.path_) == 0;
    }

    bool FilePath::operator!=(const FilePath& that) const
    {
#pragma warning(suppress : 4996) // 不输出"反对使用wcsicmp"的警告.
        return wcsicmp(path_, that.path_) != 0;
    }

    bool FilePath::operator<(const FilePath& that) const
    {
#pragma warning(suppress : 4996) // 不输出"反对使用wcsicmp"的警告.
        return wcsicmp(path_, that.path_) <= 0;
    }

    bool FilePath::IsAbsolute() const
    {
        return !PathIsRelativeW(path_);
    }

    bool FilePath::AddBackslash()
    {
        return PathAddBackslashW(path_) != NULL;
    }

    void FilePath::RemoveBackslash()
    {
        PathRemoveBackslash(path_);
    }

    bool FilePath::AddExtension(const wchar_t* ext)
    {
        return !!PathAddExtensionW(path_, ext);
    }

    bool FilePath::Append(const wchar_t* append)
    {
        return !!PathAppendW(path_, append);
    }

    FilePath FilePath::Canonicalize() const
    {
        FilePath path;
        PathCanonicalizeW(path.path_, path_);
        return path;
    }

    void FilePath::GetComponents(std::vector<std::wstring>* components) const
    {
        DCHECK(components);

        if(!components)
        {
            return;
        }
        components->clear();

        if(Empty())
        {
            return;
        }

        std::vector<std::wstring> ret_val;
        FilePath current = *this;
        const wchar_t* last_component = current.FindFileName();
        while(last_component)
        {
            ret_val.push_back(std::wstring(last_component));
            if(last_component == current.value())
            {
                break;
            }
            current.RemoveFileSpec();
            last_component = current.FindFileName();
        }
        *components = std::vector<std::wstring>(ret_val.rbegin(),
            ret_val.rend());
    }

    void FilePath::RemoveExtension()
    {
        PathRemoveExtensionW(path_);
    }

    bool FilePath::RenameExtension(const wchar_t* ext)
    {
        return !!PathRenameExtensionW(path_, ext);
    }

    bool FilePath::RemoveFileSpec()
    {
        return !!PathRemoveFileSpecW(path_);
    }

    void FilePath::StripPath()
    {
        PathStripPathW(path_);
    }

    bool FilePath::StripToRoot()
    {
        return !!PathStripToRootW(path_);
    }

    const wchar_t* FilePath::FindExtension() const
    {
        return PathFindExtensionW(path_);
    }

    const wchar_t* FilePath::FindFileName() const
    {
        return PathFindFileNameW(path_);
    }

    bool FilePath::IsExists() const
    {
        return !!PathFileExistsW(path_);
    }

    bool FilePath::IsDirectory() const
    {
        return !!PathIsDirectoryW(path_);
    }

    bool FilePath::IsDirectoryEmpty() const
    {
        return !!PathIsDirectoryEmptyW(path_);
    }

    bool FilePath::IsDotDirectory() const
    {
        return *this == kCurrentDirectoryPath;
    }

    bool FilePath::IsDotDotDirectory() const
    {
        return *this == kParentDirectoryPath;
    }

    bool FilePath::GetTempDir(FilePath& path)
    {
        wchar_t temp_path[MAX_PATH] = { 0 };
        DWORD path_len = ::GetTempPathW(MAX_PATH, temp_path);
        if(path_len>=MAX_PATH || path_len<=0)
        {
            return false;
        }
        path = FilePath(temp_path);
        return true;
    }

    bool FilePath::GetCurrentDirectory(FilePath& dir)
    {
        wchar_t current_path[MAX_PATH] = { 0 };
        DWORD path_len = ::GetCurrentDirectoryW(MAX_PATH, current_path);
        if(path_len>=MAX_PATH || path_len<=0)
        {
            return false;
        }

        dir = FilePath(current_path);
        return true;
    }

    void FilePath::WriteStringTypeToPickle(Pickle* pickle,
        const std::wstring& path)
    {
        pickle->WriteWString(path);
    }

    bool FilePath::ReadStringTypeFromPickle(Pickle* pickle,
        void** iter, std::wstring* path)
    {
        if(!pickle->ReadWString(iter, path))
        {
            return false;
        }

        return true;
    }

    void FilePath::WriteToPickle(Pickle* pickle)
    {
        WriteStringTypeToPickle(pickle, value());
    }

    bool FilePath::ReadFromPickle(Pickle* pickle, void** iter)
    {
        std::wstring path;
        if(ReadStringTypeFromPickle(pickle, iter, &path))
        {
#pragma warning(suppress : 4996) // 不输出"反对使用wcscpy"的警告.
            wcscpy(path_, path.c_str());
            return true;
        }

        return false;
    }

} //namespace base