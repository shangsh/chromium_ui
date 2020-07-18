
#ifndef __utf_string_conversion_utils_h__
#define __utf_string_conversion_utils_h__

#pragma once

#include "basic_types.h"
#include "string16.h"

// �����ڸ���UTF�ַ���ת���ļ�.

namespace base
{

    inline bool IsValidCodepoint(uint32 code_point)
    {
        // �������ߴ������([0xD800, 0xDFFF])�ʹ���0x10FFFF(�����������)
        // �����.
        // ���ַ���δָ������������.
        return code_point<0xD800u ||
            (code_point>=0xE000u && code_point<=0x10FFFFu);
    }

    inline bool IsValidCharacter(uint32 code_point)
    {
        // �Ϸ���㼯�����ų����з��ַ�(U+FDD0..U+FDEF, ������
        // ��0xFFFE��β�����).
        return code_point<0xD800u || (code_point>=0xE000u &&
            code_point<0xFDD0u) || (code_point>0xFDEFu &&
            code_point<=0x10FFFFu && (code_point&0xFFFEu)!=0xFFFEu);
    }

    // ��UTF-8��, |*code_point|�����һ�����. |src|�Ǵ�����, |*char_index|����ʼ
    // ��ȡλ��ƫ��, ����µ������ȡ���ַ�����, �������ڶ�ȡ��һ���ַ�.
    bool ReadUnicodeCharacter(const char* src, int32 src_len,
        int32* char_index, uint32* code_point);

    // ��ȡUTF-16�ַ�, ������ͬ����8-bit�汾.
    bool ReadUnicodeCharacter(const char16* src, int32 src_len,
        int32* char_index, uint32* code_point);

    // ���һ��UTF-8�ַ���������8-bit�ַ���. ����д����ֽ���.
    size_t WriteUnicodeCharacter(uint32 code_point, std::string* output);

    // ���һ��UTF-16�ַ���������16-bit�ַ���. ����д���16-bitֵ����.
    size_t WriteUnicodeCharacter(uint32 code_point, string16* output);

    // �²�UTF-8������ֽڳ���, ����output����Ԥ���ռ�. �ٶ������ַ�������
    // �޷��ŵ�, ������UTF-16��UTF-32������.
    template<typename CHAR>
    void PrepareForUTF8Output(const CHAR* src, size_t src_len, std::string* output);

    // ����UTF-8����, Ԥ��UTF-16����UTF-32���������. �μ�PrepareForUTF8Output().
    template<typename STRING>
    void PrepareForUTF16Or32Output(const char* src, size_t src_len, STRING* output);

} //namespace base

#endif //__utf_string_conversion_utils_h__