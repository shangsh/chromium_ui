
#include "sys_string_conversions.h"

#include <windows.h>

#include "string_piece.h"

namespace base
{

    // ��Ҫ�ٺ�����ʹ�ö���, ��Ϊ���Դ�����ʹ���˸ú���.
    std::string SysWideToUTF8(const std::wstring& wide)
    {
        return SysWideToMultiByte(wide, CP_UTF8);
    }

    // ��Ҫ�ٺ�����ʹ�ö���, ��Ϊ���Դ�����ʹ���˸ú���.
    std::wstring SysUTF8ToWide(const StringPiece& utf8)
    {
        return SysMultiByteToWide(utf8, CP_UTF8);
    }

    std::string SysWideToNativeMB(const std::wstring& wide)
    {
        return SysWideToMultiByte(wide, CP_ACP);
    }

    std::wstring SysNativeMBToWide(const StringPiece& native_mb)
    {
        return SysMultiByteToWide(native_mb, CP_ACP);
    }

    // ��Ҫ�ٺ�����ʹ�ö���, ��Ϊ���Դ�����ʹ���˸ú���.
    std::wstring SysMultiByteToWide(const StringPiece& mb, uint32 code_page)
    {
        if(mb.empty())
        {
            return std::wstring();
        }

        int mb_length = static_cast<int>(mb.length());
        // ���㻺��������.
        int charcount = MultiByteToWideChar(code_page, 0,
            mb.data(), mb_length, NULL, 0);
        if(charcount == 0)
        {
            return std::wstring();
        }

        std::wstring wide;
        wide.resize(charcount);
        MultiByteToWideChar(code_page, 0, mb.data(), mb_length,
            &wide[0], charcount);

        return wide;
    }

    // ��Ҫ�ٺ�����ʹ�ö���, ��Ϊ���Դ�����ʹ���˸ú���.
    std::string SysWideToMultiByte(const std::wstring& wide, uint32 code_page)
    {
        int wide_length = static_cast<int>(wide.length());
        if(wide_length == 0)
        {
            return std::string();
        }

        // ���㻺��������.
        int charcount = WideCharToMultiByte(code_page, 0, wide.data(),
            wide_length, NULL, 0, NULL, NULL);
        if(charcount == 0)
        {
            return std::string();
        }

        std::string mb;
        mb.resize(charcount);
        WideCharToMultiByte(code_page, 0, wide.data(), wide_length,
            &mb[0], charcount, NULL, NULL);

        return mb;
    }

} //namespace base