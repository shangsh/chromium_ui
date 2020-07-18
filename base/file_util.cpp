
#include "file_util.h"

#include <shlwapi.h>

#include "logging.h"
#include "utf_string_conversions.h"

namespace base
{

    int CountFilesCreatedAfter(const FilePath& path,
        const base::Time& comparison_time)
    {
        int file_count = 0;
        FILETIME comparison_filetime(comparison_time.ToFileTime());

        WIN32_FIND_DATA find_file_data;
        // 遍历所有文件.
        FilePath search_file(path);
        search_file.Append(L"*");
        HANDLE find_handle = FindFirstFile(search_file.value(), &find_file_data);
        if(find_handle != INVALID_HANDLE_VALUE)
        {
            do
            {
                // 当前目录和父目录不计算.
                if((wcscmp(find_file_data.cFileName, L"..")==0) ||
                    (wcscmp(find_file_data.cFileName, L".")==0))
                {
                    continue;
                }

                long result = CompareFileTime(&find_file_data.ftCreationTime,
                    &comparison_filetime);
                // 文件创建时间等于或者晚于比对时间.
                if((result==1) || (result==0))
                {
                    ++file_count;
                }
            } while(FindNextFile(find_handle,  &find_file_data));
            FindClose(find_handle);
        }

        return file_count;
    }

    int64 ComputeDirectorySize(const FilePath& root_path)
    {
        int64 running_size = 0;
        FileEnumerator file_iter(root_path, true, FileEnumerator::FILES);
        for(FilePath current=file_iter.Next(); !current.Empty();
            current=file_iter.Next())
        {
            FileEnumerator::FindInfo info;
            file_iter.GetFindInfo(&info);
            LARGE_INTEGER li = { info.nFileSizeLow, info.nFileSizeHigh };
            running_size += li.QuadPart;
        }
        return running_size;
    }

    int64 ComputeFilesSize(const FilePath& directory,
        const std::wstring& pattern)
    {
        int64 running_size = 0;
        FileEnumerator file_iter(directory, false, FileEnumerator::FILES, pattern);
        for(FilePath current=file_iter.Next(); !current.Empty();
            current=file_iter.Next())
        {
            FileEnumerator::FindInfo info;
            file_iter.GetFindInfo(&info);
            LARGE_INTEGER li = { info.nFileSizeLow, info.nFileSizeHigh };
            running_size += li.QuadPart;
        }
        return running_size;
    }

    bool Delete(const FilePath& path, bool recursive)
    {
        if(!recursive)
        {
            // 如果不递归删除, 先检查|path|是否为目录, 如果是, 用RemoveDirectory移除.
            base::PlatformFileInfo file_info;
            if(GetFileInfo(path, &file_info) && file_info.is_directory)
            {
                return !!RemoveDirectoryW(path.value());
            }

            // 否则, 路径表示文件, 通配符或者不存在. 先尝试用DeleteFile, 因为这个函数
            // 比较快. 如果DeleteFile失败, 继续调用SHFileOperation完成操作.
            if(DeleteFileW(path.value()) != 0)
            {
                return true;
            }
        }

        // SHFILEOPSTRUCT要求路径以两个NULL终结, 因此必须使用wcscpy, 因为
        // wcscpy_s会在剩余的缓冲区写入非NULL值.
        wchar_t double_terminated_path[MAX_PATH+1] = { 0 };
#pragma warning(suppress : 4996) // 不输出"反对使用wcscpy"的警告.
        wcscpy(double_terminated_path, path.value());

        SHFILEOPSTRUCT file_operation = { 0 };
        file_operation.wFunc = FO_DELETE;
        file_operation.pFrom = double_terminated_path;
        file_operation.fFlags = FOF_NOERRORUI | FOF_SILENT | FOF_NOCONFIRMATION;
        if(!recursive)
        {
            file_operation.fFlags |= FOF_NORECURSION | FOF_FILESONLY;
        }
        int err = SHFileOperation(&file_operation);
        // 有一些Windows版本在删除空目录时返回ERROR_FILE_NOT_FOUND(0x2), 有时候在
        // 应该返回ERROR_FILE_NOT_FOUND的时候却返回0x402. MSDN上说Vista及以上的
        // 版本不会返回0x402.
        return (err==0 || err==ERROR_FILE_NOT_FOUND || err==0x402);
    }

    bool DeleteAfterReboot(const FilePath& path)
    {
        return MoveFileEx(path.value(), NULL,
            MOVEFILE_DELAY_UNTIL_REBOOT|MOVEFILE_REPLACE_EXISTING) != FALSE;
    }

    int EnsureDirectory(const FilePath& path, SECURITY_ATTRIBUTES* sa)
    {
        int ret = ERROR_SUCCESS;

        if(PathIsRelativeW(path.value()))
        {
            // 不是全路径直接退出. 确保文件夹不是在当前工作目录创建.
            SetLastError(ERROR_BAD_PATHNAME);
            return ERROR_BAD_PATHNAME;
        }

        if(!CreateDirectoryW(path.value(), sa))
        {
            wchar_t *end, *slash, temp[MAX_PATH+1]; // +1 for PathAddBackslash()

            ret = GetLastError();

            // 遇到下面的错误码, 直接退出不继续执行.
            switch(ret)
            {
            case ERROR_FILENAME_EXCED_RANGE:
            case ERROR_FILE_EXISTS:
            case ERROR_ALREADY_EXISTS:
                return ret;
            }

#pragma warning(suppress : 4996) // 不输出"反对使用wcsncpy"的警告.
            wcsncpy(temp, path.value(), ARRAYSIZE(temp)-1);
            end = PathAddBackslashW(temp);

            slash = temp + 3;

            // 顺序创建所有文件夹.
            while(*slash)
            {
                while(*slash && *slash!=L'\\')
                {
                    slash = CharNextW(slash);
                }

                if(*slash)
                {
                    DCHECK(*slash == L'\\');

                    *slash = 0;

                    ret = CreateDirectoryW(temp, slash+1==end?sa:NULL)
                        ? ERROR_SUCCESS : GetLastError();

                }
                *slash++ = L'\\';
            }
        }

        return ret;
    }

    int EnsureDirectoryForFile(const FilePath& path, SECURITY_ATTRIBUTES* sa)
    {
        FilePath path_dir(path);
        if(path_dir.RemoveFileSpec())
        {
            return EnsureDirectory(path_dir, sa);
        }

        SetLastError(ERROR_BAD_PATHNAME);
        return ERROR_BAD_PATHNAME;
    }

    FileEnumerator::FileEnumerator(const FilePath& root_path,
        bool recursive,
        FileEnumerator::FILE_TYPE file_type)
        : recursive_(recursive),
        file_type_(file_type),
        has_find_data_(false),
        find_handle_(INVALID_HANDLE_VALUE)
    {
        pending_paths_.push(root_path);
    }

    bool GetFileInfo(const FilePath& file_path, base::PlatformFileInfo* results)
    {
        WIN32_FILE_ATTRIBUTE_DATA attr;
        if(!GetFileAttributesEx(file_path.value(), GetFileExInfoStandard, &attr))
        {
            return false;
        }

        ULARGE_INTEGER size;
        size.HighPart = attr.nFileSizeHigh;
        size.LowPart = attr.nFileSizeLow;
        results->size = size.QuadPart;

        results->is_directory =
            (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        results->last_modified = base::Time::FromFileTime(attr.ftLastWriteTime);
        results->last_accessed = base::Time::FromFileTime(attr.ftLastAccessTime);
        results->creation_time = base::Time::FromFileTime(attr.ftCreationTime);

        return true;
    }

    FILE* OpenFile(const FilePath& filename, const char* mode)
    {
        std::wstring w_mode = ASCIIToWide(std::string(mode));
        return _wfsopen(filename.value(), w_mode.c_str(), _SH_DENYNO);
    }

    bool CloseFile(FILE* file)
    {
        if(file == NULL)
        {
            return true;
        }
        return fclose(file) == 0;
    }

    bool GetCurrentDirectory(FilePath* dir)
    {
        wchar_t system_buffer[MAX_PATH] = { 0 };
        DWORD len = ::GetCurrentDirectory(MAX_PATH, system_buffer);
        if(len==0 || len>MAX_PATH)
        {
            return false;
        }
        *dir = FilePath(system_buffer).Canonicalize();
        return true;
    }

    bool SetCurrentDirectory(const FilePath& directory)
    {
        BOOL ret = ::SetCurrentDirectory(directory.value());
        return ret != 0;
    }

    FileEnumerator::FileEnumerator(const FilePath& root_path,
        bool recursive,
        FileEnumerator::FILE_TYPE file_type,
        const std::wstring& pattern)
        : recursive_(recursive),
        file_type_(file_type),
        has_find_data_(false),
        pattern_(pattern),
        find_handle_(INVALID_HANDLE_VALUE)
    {
        pending_paths_.push(root_path);
    }

    FileEnumerator::~FileEnumerator()
    {
        if(find_handle_ != INVALID_HANDLE_VALUE)
        {
            FindClose(find_handle_);
        }
    }

    void FileEnumerator::GetFindInfo(FindInfo* info)
    {
        DCHECK(info);

        if(!has_find_data_)
        {
            return;
        }

        memcpy(info, &find_data_, sizeof(*info));
    }

    bool FileEnumerator::IsDirectory(const FindInfo& info)
    {
        return (info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    // static
    FilePath FileEnumerator::GetFilename(const FindInfo& find_info)
    {
        return FilePath(find_info.cFileName);
    }

    FilePath FileEnumerator::Next()
    {
        while(has_find_data_ || !pending_paths_.empty())
        {
            if(!has_find_data_)
            {
                // 上一次FindFirstFile操作结束, 开始新的查找.
                root_path_ = pending_paths_.top();
                pending_paths_.pop();

                // 开始新的查找.
                FilePath src = root_path_;

                if(pattern_.empty())
                {
                    src.Append(L"*"); // 匹配模式为空表示匹配所有.
                }
                else
                {
                    src.Append(pattern_.c_str());
                }

                find_handle_ = FindFirstFile(src.value(), &find_data_);
                has_find_data_ = true;
            }
            else
            {
                // 查找下一个文件/目录.
                if(!FindNextFile(find_handle_, &find_data_))
                {
                    FindClose(find_handle_);
                    find_handle_ = INVALID_HANDLE_VALUE;
                }
            }

            if(INVALID_HANDLE_VALUE == find_handle_)
            {
                has_find_data_ = false;

                // 此时完成一个目录的搜索, 下面要进行队列中下一个目录的搜索. 匹配模式只在
                // 根目录的搜索时有效, 符合条件的子目录会遍历所有文件.
                pattern_ = std::wstring();

                continue;
            }

            FilePath cur_file(find_data_.cFileName);
            if(ShouldSkip(cur_file))
            {
                continue;
            }

            // 构建绝对路径.
            cur_file = root_path_;
            cur_file.Append(find_data_.cFileName);

            if(find_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(recursive_)
                {
                    // 如果|cur_file|是目录, 且需要递归搜索, 添加目录到pending_paths_,
                    // 这样在当前目录扫描完成之后会搜索|cur_file|.
                    pending_paths_.push(cur_file);
                }
                if(file_type_ & FileEnumerator::DIRECTORIES)
                {
                    return cur_file;
                }
            }
            else if(file_type_ & FileEnumerator::FILES)
            {
                return cur_file;
            }
        }

        return FilePath();
    }

    bool FileEnumerator::ShouldSkip(const FilePath& path)
    {
        FilePath basename(path);
        basename.StripPath();
        return basename.IsDotDirectory() || basename.IsDotDotDirectory();
    }


    MemoryMappedFile::MemoryMappedFile()
        : file_(INVALID_HANDLE_VALUE),
        file_mapping_(INVALID_HANDLE_VALUE),
        data_(NULL),
        length_(INVALID_FILE_SIZE) {}

    MemoryMappedFile::~MemoryMappedFile()
    {
        CloseHandles();
    }

    bool MemoryMappedFile::Initialize(PlatformFile file)
    {
        if(IsValid())
        {
            return false;
        }

        file_ = file;

        if(!MapFileToMemoryInternal())
        {
            CloseHandles();
            return false;
        }

        return true;
    }

    bool MemoryMappedFile::Initialize(const FilePath& file_name)
    {
        if(IsValid())
        {
            return false;
        }

        if(!MapFileToMemory(file_name))
        {
            CloseHandles();
            return false;
        }

        return true;
    }

    bool MemoryMappedFile::IsValid()
    {
        return data_ != NULL;
    }

    bool MemoryMappedFile::MapFileToMemory(const FilePath& file_name)
    {
        file_ = CreatePlatformFile(file_name,
            PLATFORM_FILE_OPEN|PLATFORM_FILE_READ, NULL, NULL);

        if(file_ == kInvalidPlatformFileValue)
        {
            LOG(ERROR) << "Couldn't open " << file_name.value();
            return false;
        }

        return MapFileToMemoryInternal();
    }

    bool MemoryMappedFile::MapFileToMemoryInternal()
    {
        if(file_ == INVALID_HANDLE_VALUE)
        {
            return false;
        }

        length_ = ::GetFileSize(file_, NULL);
        if(length_ == INVALID_FILE_SIZE)
        {
            return false;
        }

        // length_的值来自于上面的GetFileSize(). GetFileSize()返回DWORD, 所以
        // 这里的强转是安全的.
        file_mapping_ = ::CreateFileMapping(file_, NULL, PAGE_READONLY,
            0, static_cast<DWORD>(length_), NULL);
        if(!file_mapping_)
        {
            return false;
        }

        data_ = static_cast<uint8*>(::MapViewOfFile(file_mapping_,
            FILE_MAP_READ, 0, 0, length_));
        return data_ != NULL;
    }

    void MemoryMappedFile::CloseHandles()
    {
        if(data_)
        {
            ::UnmapViewOfFile(data_);
        }
        if(file_mapping_ != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(file_mapping_);
        }
        if(file_ != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(file_);
        }

        data_ = NULL;
        file_mapping_ = file_ = INVALID_HANDLE_VALUE;
        length_ = INVALID_FILE_SIZE;
    }

} //namespace base