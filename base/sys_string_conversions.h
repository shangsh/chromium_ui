
#ifndef __sys_string_conversions_h__
#define __sys_string_conversions_h__

#pragma once

#include <string>

#include "basic_types.h"

// �ṩƽ̨��ص��ַ�������ת��, ��ʹ��ICU, ������С����.

namespace base
{

    class StringPiece;

    // ���ֽں�UTF-8֮��ת��. ��������ϵͳ����.
    std::string SysWideToUTF8(const std::wstring& wide);
    std::wstring SysUTF8ToWide(const StringPiece& utf8);

    // ���ֽںͶ��ֽ�֮��ת��.
    // Σ��: �ᶪʧ��ı���Ϣ(Windows������).
    std::string SysWideToNativeMB(const std::wstring& wide);
    std::wstring SysNativeMBToWide(const StringPiece& native_mb);

    // ʹ�ô���ҳ����8�ֽںͿ��ֽ�֮��ת��. ����ҳ��ʶ������Windows����
    // MultiByteToWideChar()�ɽ��ܵ�.
    std::wstring SysMultiByteToWide(const StringPiece& mb, uint32 code_page);
    std::string SysWideToMultiByte(const std::wstring& wide, uint32 code_page);

} //namespace base

#endif //__sys_string_conversions_h__