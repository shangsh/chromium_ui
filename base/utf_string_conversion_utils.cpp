
#include "utf_string_conversion_utils.h"

#include "third_party/icu/icu_utf.h"

namespace base
{

    bool ReadUnicodeCharacter(const char* src, int32 src_len,
        int32* char_index, uint32* code_point)
    {
        // U8_NEXTʹ��-1��ʾ����, ���code_pointʹ���з�������.
        // ��������ʱ����false, ���code_pointʹ���޷�������.
        int32 cp;
        CBU8_NEXT(src, *char_index, src_len, cp);
        *code_point = static_cast<uint32>(cp);

        // �����ICU���Ƶ���һ�ַ�, ����Ҫ���Ƶ����һ���õ����ַ�.
        (*char_index)--;

        // ��֤����ֵ�ĺϷ��� .
        return IsValidCodepoint(cp);
    }

    bool ReadUnicodeCharacter(const char16* src, int32 src_len,
        int32* char_index, uint32* code_point)
    {
        if(CBU16_IS_SURROGATE(src[*char_index]))
        {
            if(!CBU16_IS_SURROGATE_LEAD(src[*char_index]) ||
                *char_index+1>=src_len ||
                !CBU16_IS_TRAIL(src[*char_index+1]))
            {
                // �Ƿ��ĸߴ����.
                return false;
            }

            // �Ϸ��ĸߴ����.
            *code_point = CBU16_GET_SUPPLEMENTARY(src[*char_index],
                src[*char_index+1]);
            (*char_index)++;
        }
        else
        {
            // ���Ǹߴ���, 16-bit�ֳ�.
            *code_point = src[*char_index];
        }

        return IsValidCodepoint(*code_point);
    }

    size_t WriteUnicodeCharacter(uint32 code_point, std::string* output)
    {
        if(code_point <= 0x7f)
        {
            // ������һ�ֽ����.
            output->push_back(code_point);
            return 1;
        }

        // CBU8_APPEND_UNSAFE���Ը���4�ֽ�.
        size_t char_offset = output->length();
        size_t original_char_offset = char_offset;
        output->resize(char_offset+CBU8_MAX_LENGTH);

        CBU8_APPEND_UNSAFE(&(*output)[0], char_offset, code_point);

        // CBU8_APPEND_UNSAFE���ƶ�ָ�뵽�����ַ���, ������֪���ַ�������.
        output->resize(char_offset);
        return char_offset - original_char_offset;
    }

    size_t WriteUnicodeCharacter(uint32 code_point, string16* output)
    {
        if(CBU16_LENGTH(code_point) == 1)
        {
            // ����ڻ���������ƽ��.
            output->push_back(static_cast<char16>(code_point));
            return 1;
        }
        // �ǻ���������ƽ���ַ�ʹ��˫�ֽڱ���.
        size_t char_offset = output->length();
        output->resize(char_offset+CBU16_MAX_LENGTH);
        CBU16_APPEND_UNSAFE(&(*output)[0], char_offset, code_point);
        return CBU16_MAX_LENGTH;
    }

    template<typename CHAR>
    void PrepareForUTF8Output(const CHAR* src,
        size_t src_len, std::string* output)
    {
        output->clear();
        if(src_len == 0)
        {
            return;
        }
        if(src[0] < 0x80)
        {
            // �ٶ�����ȫ����ASCII.
            output->reserve(src_len);
        }
        else
        {
            // �ٶ�����ȫ���ǷǷ�ASCII�Ҷ������ֽڱ���.
            output->reserve(src_len*3);
        }
    }

    // ������֪�ĵ��ð汾.
    template void PrepareForUTF8Output(const wchar_t*, size_t, std::string*);
    template void PrepareForUTF8Output(const char16*, size_t, std::string*);

    template<typename STRING>
    void PrepareForUTF16Or32Output(const char* src,
        size_t src_len, STRING* output)
    {
        output->clear();
        if(src_len == 0)
        {
            return;
        }
        if(static_cast<unsigned char>(src[0]) < 0x80)
        {
            // �ٶ�����ȫ����ASCII.
            output->reserve(src_len);
        }
        else
        {
            // Otherwise assume that the UTF-8 sequences will have 2 bytes for each
            // character.
            // �ٶ������UTF-8����ÿ2�ֽڱ�ʾһ���ַ�.
            output->reserve(src_len/2);
        }
    }

    // ������֪�ĵ��ð汾.
    template void PrepareForUTF16Or32Output(const char*, size_t, std::wstring*);
    template void PrepareForUTF16Or32Output(const char*, size_t, string16*);

} //namespace base