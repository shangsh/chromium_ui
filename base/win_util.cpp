
#include "win_util.h"

#include <sddl.h>

#include "logging.h"
#include "registry.h"
#include "scoped_handle.h"
#include "scoped_ptr.h"
#include "string_util.h"
#include "stringprintf.h"
#include "windows_version.h"

namespace base
{

#define SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(struct_name, member) \
    offsetof(struct_name, member) + \
    (sizeof static_cast<struct_name*>(NULL)->member)
#define NONCLIENTMETRICS_SIZE_PRE_VISTA \
    SIZEOF_STRUCT_WITH_SPECIFIED_LAST_MEMBER(NONCLIENTMETRICS, lfMessageFont)

    void GetNonClientMetrics(NONCLIENTMETRICS* metrics)
    {
        DCHECK(metrics);

        static const UINT SIZEOF_NONCLIENTMETRICS =
            (GetWinVersion()>=WINVERSION_VISTA) ?
            sizeof(NONCLIENTMETRICS) : NONCLIENTMETRICS_SIZE_PRE_VISTA;
        metrics->cbSize = SIZEOF_NONCLIENTMETRICS;
        const bool success = !!SystemParametersInfo(SPI_GETNONCLIENTMETRICS,
            SIZEOF_NONCLIENTMETRICS, metrics, 0);
        DCHECK(success);
    }

    bool GetUserSidString(std::wstring* user_sid)
    {
        // ��ȡ��ǰ����.
        HANDLE token = NULL;
        if(!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token))
        {
            return false;
        }
        ScopedHandle token_scoped(token);

        DWORD size = sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE;
        scoped_array<BYTE> user_bytes(new BYTE[size]);
        TOKEN_USER* user = reinterpret_cast<TOKEN_USER*>(user_bytes.get());

        if(!::GetTokenInformation(token, TokenUser, user, size, &size))
        {
            return false;
        }

        if(!user->User.Sid)
        {
            return false;
        }

        // ����ת����һ���ַ���.
        wchar_t* sid_string;
        if(!::ConvertSidToStringSid(user->User.Sid, &sid_string))
        {
            return false;
        }

        *user_sid = sid_string;

        ::LocalFree(sid_string);

        return true;
    }

#pragma warning(push)
#pragma warning(disable:4312 4244)
    WNDPROC SetWindowProc(HWND hwnd, WNDPROC proc)
    {
        // ����ֱ��ʹ��SetwindowLongPtr()�����ķ���ֵ, ԭ���������ص���ԭʼ���ڹ��̶�
        // ���ǵ�ǰ���ڹ���. ��֪������һ��bug���������Ϊ.
        WNDPROC oldwindow_proc = reinterpret_cast<WNDPROC>(
            GetWindowLongPtr(hwnd, GWLP_WNDPROC));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(proc));
        return oldwindow_proc;
    }

    void* SetWindowUserData(HWND hwnd, void* user_data)
    {
        return reinterpret_cast<void*>(SetWindowLongPtr(hwnd,
            GWLP_USERDATA, reinterpret_cast<LONG_PTR>(user_data)));
    }

    void* GetWindowUserData(HWND hwnd)
    {
        return reinterpret_cast<void*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }
#pragma warning(pop)

    bool IsShiftPressed()
    {
        return (::GetKeyState(VK_SHIFT)&0x8000) == 0x8000;
    }

    bool IsCtrlPressed()
    {
        return (::GetKeyState(VK_CONTROL)&0x8000) == 0x8000;
    }

    bool IsAltPressed()
    {
        return (::GetKeyState(VK_MENU)&0x8000) == 0x8000;
    }

    std::wstring GetClassName(HWND window)
    {
        // ������ջ����������㹻��, GetClassNameW�᷵�ؽضϵĽ��(��֤null��β).
        // ����, û��ȷ�����������ĳ���.
        DWORD buffer_size = MAX_PATH;
        while(true)
        {
            std::wstring output;
            DWORD size_ret = GetClassNameW(window,
                WriteInto(&output, buffer_size), buffer_size);
            if(size_ret == 0)
            {
                break;
            }
            if(size_ret < (buffer_size-1))
            {
                output.resize(size_ret);
                return output;
            }
            buffer_size *= 2;
        }
        return std::wstring(); // ����.
    }

    std::wstring FormatMessage(unsigned messageid)
    {
        wchar_t* string_buffer = NULL;
        unsigned string_length = ::FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|
            FORMAT_MESSAGE_IGNORE_INSERTS, NULL, messageid, 0,
            reinterpret_cast<wchar_t*>(&string_buffer), 0, NULL);

        std::wstring formatted_string;
        if(string_buffer)
        {
            formatted_string = string_buffer;
            LocalFree(reinterpret_cast<HLOCAL>(string_buffer));
        }
        else
        {
            // ��ʽ������. �򵥵ĸ�ʽ����Ϣֵ���ַ�����.
            SStringPrintf(&formatted_string, L"message number %d", messageid);
        }
        return formatted_string;
    }

    std::wstring FormatLastWin32Error()
    {
        return FormatMessage(GetLastError());
    }

} //namespace base