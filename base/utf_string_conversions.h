
#ifndef __utf_string_conversions_h__
#define __utf_string_conversions_h__

#pragma once

#include <string>

#include "string16.h"

namespace base
{
    class StringPiece;
}

// UTF-8��-16��-32�ַ���֮���ת��. �ײ�����Ƚ���, ���Ծ������ⲻ��Ҫ��ת��.
// bool����ֵ��ʾת���Ƿ�100%�Ϸ�, �����ܶ�Ľ���ת�����. strings����ֵ�İ汾
// ���Դ��󲢷��ؿ��ܵ�ת�����.
bool WideToUTF8(const wchar_t* src, size_t src_len, std::string* output);
std::string WideToUTF8(const std::wstring& wide);
bool UTF8ToWide(const char* src, size_t src_len, std::wstring* output);
std::wstring UTF8ToWide(const base::StringPiece& utf8);

bool WideToUTF16(const wchar_t* src, size_t src_len, string16* output);
string16 WideToUTF16(const std::wstring& wide);
bool UTF16ToWide(const char16* src, size_t src_len, std::wstring* output);
std::wstring UTF16ToWide(const string16& utf16);

bool UTF8ToUTF16(const char* src, size_t src_len, string16* output);
string16 UTF8ToUTF16(const std::string& utf8);
bool UTF16ToUTF8(const char16* src, size_t src_len, std::string* output);
std::string UTF16ToUTF8(const string16& utf16);

// ת��ASCII�ַ���(ͨ�����ַ�������)�����ֽ��ַ���.
std::wstring ASCIIToWide(const char* ascii);
std::wstring ASCIIToWide(const std::string& ascii);
string16 ASCIIToUTF16(const char* ascii);
string16 ASCIIToUTF16(const std::string& ascii);

#endif //__utf_string_conversions_h__