
#ifndef __json_reader_h__
#define __json_reader_h__

#pragma once

#include <string>

#include "base/basic_types.h"

// JSON������. ת��JSON��Value����.
// http://www.ietf.org/rfc/rfc4627.txt?number=4627
//
//
// ��RFC�ĳ���:
// - ֻ�ܽ��������ŵ�32λ�����ʹ�С����double.
// - Ҫ������ı�����UTF8, �淶����ʹ��UTF-16(BE����LE)����UTF-32(BE����LE)
//   ����.
// - �������Ƕ��100���Է�ջ���(�淶û������).
// - Unicode����������ǻ���������ǰ��д��Unicode Byte-Order-Mark (U+FEFF),
//   ���紫�ݸ�JSONReader::JsonToValue()������UTF-8�ַ����Ŀ�ͷ�п�����
//   UTF-8 BOM (0xEF, 0xBB, 0xBF). Ϊ�˱����UTF-8 BOM����Ĵ���ɺϷ��ַ�,
//   �����ڽ���ǰ����Unicode�ַ�����ͷ��Unicode BOM.
//
// TODO: ��ӽ���ѡ�����keys�Ƿ������""������.
// TODO: ���ѡ������Ƿ�����ע��.

class Value;

namespace base
{

    class JSONReader
    {
    public:
        // ����JS token.
        class Token
        {
        public:
            enum Type
            {
                OBJECT_BEGIN,           // {
                OBJECT_END,             // }
                ARRAY_BEGIN,            // [
                ARRAY_END,              // ]
                STRING,
                NUMBER,
                BOOL_TRUE,              // true
                BOOL_FALSE,             // false
                NULL_TOKEN,             // null
                LIST_SEPARATOR,         // ,
                OBJECT_PAIR_SEPARATOR,  // :
                END_OF_INPUT,
                INVALID_TOKEN,
            };
            Token(Type t, const wchar_t* b, int len)
                : type(t), begin(b), length(len) {}

            Type type;

            // ָ��token��JSONReader::json_pos_��ʼλ�õ�ָ��.
            const wchar_t* begin;

            // ����λ����token���һ���ַ�.
            int length;

            // ����token����λ�ú���ַ�.
            wchar_t NextChar()
            {
                return *(begin + length);
            }
        };

        // ���������еĴ�����.
        enum JsonParseError
        {
            JSON_NO_ERROR = 0,
            JSON_BAD_ROOT_ELEMENT_TYPE,
            JSON_INVALID_ESCAPE,
            JSON_SYNTAX_ERROR,
            JSON_TRAILING_COMMA,
            JSON_TOO_MUCH_NESTING,
            JSON_UNEXPECTED_DATA_AFTER_ROOT,
            JSON_UNSUPPORTED_ENCODING,
            JSON_UNQUOTED_DICTIONARY_KEY,
        };

        // �������Ӧ���ַ���.
        static const char* kBadRootElementType;
        static const char* kInvalidEscape;
        static const char* kSyntaxError;
        static const char* kTrailingComma;
        static const char* kTooMuchNesting;
        static const char* kUnexpectedDataAfterRoot;
        static const char* kUnsupportedEncoding;
        static const char* kUnquotedDictionaryKey;

        JSONReader();

        // ��ȡ������|json|, ����Value����, ������ӵ�з��ض��������Ȩ. ���|json|
        // ��ʽ����, ����NULL. ���|allow_trailing_comma|Ϊtrue, ���Զ���������β
        // �Ķ���, �������������RFC.
        static Value* Read(const std::string& json, bool allow_trailing_comma);

        // ��ȡ������|json|. |error_code_out|��|error_msg_out|�ǿ�ѡ��, ���ָ����,
        // ��������NULL��ʱ��, �Ὣ������ʹ�����Ϣ(��ð�������λ��)�洢����, ����
        // �������κ��޸�.
        static Value* ReadAndReturnError(const std::string& json,
            bool allow_trailing_comma, int* error_code_out,
            std::string* error_msg_out);

        // ת��JSON������ɿɶ�����Ϣ. ���error_code��JSON_NO_ERROR���ؿ��ַ���.
        static std::string ErrorCodeToString(JsonParseError error_code);

        // ����JsonToValue()ʧ��ʱ, ���ش�����, ���򷵻�JSON_NO_ERROR.
        JsonParseError error_code() const { return error_code_; }

        // ת��JSON������ɿɶ�����Ϣ, ����ܴ������к�.
        std::string GetErrorMessage() const;

        // ��ȡ������|json|, ����Value����, ������ӵ�з��ض��������Ȩ. ���|json|
        // ��ʽ����, ����NULL, ����ͨ��|error_message()|��ȡ��ϸ�Ĵ�����Ϣ. ���
        // |allow_trailing_comma|Ϊtrue, ���Զ���������β�Ķ���, �����������
        // ��RFC.
        Value* JsonToValue(const std::string& json, bool check_root,
            bool allow_trailing_comma);

    private:
        static std::string FormatErrorMessage(int line, int column,
            const std::string& description);

        DISALLOW_COPY_AND_ASSIGN(JSONReader);

        // �ݹ鹹��Value. ���JSON�ַ������Ϸ��򷵻�NULL. ���|is_root|Ϊtrue, ��֤
        // ��Ԫ���Ƿ�Ϊ�����������.
        Value* BuildValue(bool is_root);

        // ���ַ����н�����Token::NUMBER. ��������е��ַ������ǺϷ�������, ����
        // Token::INVALID_TOKEN. ʵ���ڲ����õ���DecodeNumber, �˺������ַ���ת��
        // ��int/double.
        Token ParseNumberToken();

        // ת��token�Ӵ���int����double. �������ת��(���粻���)����Value, ����
        // ����NULL.
        Value* DecodeNumber(const Token& token);

        // ���ַ����н�����Token::STRING. ��������е��ַ������ǺϷ�������, ����
        // Token::INVALID_TOKEN. ʵ���ڲ����õ���DecodeString, �˺������ַ���ת��
        // ��wstring.
        Token ParseStringToken();

        // ת���Ӵ��ɺϷ���Value�ַ���. �������ǳɹ�(����ParseStringToken��ʧ��).
        Value* DecodeString(const Token& token);

        // ��ȡJSON������һ��token. ���޸ĵ�ǰλ��, ������ʱ��ȡ�����token.
        Token ParseToken();

        // ��ǰ�ƶ�|json_pos_|�����հ׺�ע��.
        void EatWhitespaceAndComments();

        // ���|json_pos_|��ע����ʼλ��������ע��, ���򷵻�false.
        bool EatComment();

        // ���|json_pos_|�Ƿ�ƥ��str.
        bool NextStringMatch(const std::wstring& str);

        // ���÷��ظ������ߵĴ�����. ȷ�����кŲ���ӵ�error_pos��.
        void SetErrorCode(const JsonParseError error, const wchar_t* error_pos);

        // ָ�������ַ�����ʼλ�õ�ָ��.
        const wchar_t* start_pos_;

        // ָ�������ַ�����ǰλ�õ�ָ��.
        const wchar_t* json_pos_;

        // ά��lists/dictsǶ�ײ���.
        int stack_depth_;

        // ����������β�Ƿ������ж��ŵĽ���ѡ��.
        bool allow_trailing_comma_;

        // ���һ�ε���JsonToValue()�Ĵ�����.
        JsonParseError error_code_;
        int error_line_;
        int error_col_;
    };

} //namespace base

#endif //__json_reader_h__