
#ifndef __string_escape_h__
#define __string_escape_h__

#pragma once

#include <string>

#include "base/string16.h"

namespace base
{

    // ת�����|str|�ɺϷ���JSON�ַ�, ���ӽ����|dst|, ����ͳһ��ת���ַ�����.
    // ���|put_in_quotes|Ϊtrue, ��""�ѽ��������. ������ı�, ������������
    // javascript���ַ����������|str|��ȫ��ͬ.
    void JsonDoubleQuote(const std::string& str,
        bool put_in_quotes, std::string* dst);

    // ������ĺ���һ��, ��ʼ����""�ѷ��صĽ��������.
    std::string GetDoubleQuotedJson(const std::string& str);

    void JsonDoubleQuote(const string16& str,
        bool put_in_quotes, std::string* dst);

    // ������ĺ���һ��, ��ʼ����""�ѷ��صĽ��������.
    std::string GetDoubleQuotedJson(const string16& str);

} //namespace base

#endif //__string_escape_h__