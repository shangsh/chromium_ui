
#include "native_library.h"

#include "file_path.h"
#include "file_util.h"
#include "utf_string_conversions.h"

namespace base
{

    // static
    NativeLibrary LoadNativeLibrary(const FilePath& library_path)
    {
        // 切换当前目录到库目录, 方式库依赖那个目录的其他DLLs.
        bool restore_directory = false;
        FilePath current_directory;
        if(base::GetCurrentDirectory(&current_directory))
        {
            FilePath plugin_path = library_path;
            plugin_path.RemoveFileSpec();
            if(!plugin_path.Empty())
            {
                base::SetCurrentDirectory(plugin_path);
                restore_directory = true;
            }
        }

        HMODULE module = LoadLibrary(library_path.value());
        if(restore_directory)
        {
            base::SetCurrentDirectory(current_directory);
        }

        return module;
    }

    // static
    void UnloadNativeLibrary(NativeLibrary library)
    {
        FreeLibrary(library);
    }

    // static
    void* GetFunctionPointerFromNativeLibrary(NativeLibrary library,
        const char* name)
    {
        return GetProcAddress(library, name);
    }

    // static
    string16 GetNativeLibraryName(const string16& name)
    {
        return name + ASCIIToUTF16(".dll");
    }

} //namespace base