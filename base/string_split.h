
#ifndef __string_split_h__
#define __string_split_h__

#pragma once

#include <vector>

#include "string16.h"

namespace base
{

    // ��|str|�зֳ���|s|�ָ����Ӵ�������, ������θ��ӵ�|r|��.
    // ���ڶ��������|s|������|s|��ͷ���߽�β�����, �����һ���մ�.
    //
    // ÿ���Ӵ���ͷ�ͽ�β�Ŀհ��ַ��ᱻȥ����.
    // ��wchar_t��char16ʱ��(����Windows), |c|�����ڻ���������ƽ��.
    // �����ط�(Linux/Mac), wchar_tӦ���ǺϷ���Unicode code point(32-bit).
    void SplitString(const std::wstring& str,
        wchar_t c, std::vector<std::wstring>* r);
    // ע��: |c|�����ڻ���������ƽ��.
    void SplitString(const std::string& str,
        char16 c, std::vector<std::string>* r);
    // |str|��Ӧ��ʹ�ö��ֽڱ���, ��Shift-JIS����GBK�������ֽ��ַ���β�ֽ�
    // ������ASCII����.
    // UTF-8������ASCII���ݵĵ�/���ֽڱ�����û�����.
    // ע��: |c|������ASCII����.
    void SplitString(const std::string& str,
        char c, std::vector<std::string>* r);

    bool SplitStringIntoKeyValues(const std::string& line,
        char key_value_delimiter, std::string* key,
        std::vector<std::string>* values);

    bool SplitStringIntoKeyValuePairs(const std::string& line,
        char key_value_delimiter, char key_value_pair_delimiter,
        std::vector<std::pair<std::string, std::string> >* kv_pairs);

    // ��SplitStringһ��, ֻ����substring��Ϊ�ָ���.
    void SplitStringUsingSubstr(const string16& str,
        const string16& s, std::vector<string16>* r);
    void SplitStringUsingSubstr(const std::string& str,
        const std::string& s, std::vector<std::string>* r);

    // ��SplitStringһ��, ֻ�ǲ�ȥ���հ�.
    // ע��: |c|�����ڻ���������ƽ��.
    void SplitStringDontTrim(const string16& str,
        char16 c, std::vector<string16>* r);
    // |str|��Ӧ��ʹ�ö��ֽڱ���, ��Shift-JIS����GBK�������ֽ��ַ���β�ֽ�
    // ������ASCII����.
    // UTF-8������ASCII���ݵĵ�/���ֽڱ�����û�����.
    // ע��: |c|������ASCII����.
    void SplitStringDontTrim(const std::string& str,
        char c, std::vector<std::string>* r);

} //namespace base

#endif //__string_split_h__