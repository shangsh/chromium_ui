
#ifndef __string_util_h__
#define __string_util_h__

#pragma once

#include <stdarg.h> 
#include <cstdarg>
#include <cstring>
#include <cstdlib>

// Windows to POSIX compatibility
#define _strdup strdup
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#define __min(a,b) ((a)<(b)?(a):(b))
#define __max(a,b) ((a)>(b)?(a):(b))
#define vsnprintf_s vsnprintf
#define _vscprintf vsnprintf
#define _vsnwprintf_s vswprintf

#include <string>
#include <vector>

#include "logging.h"
#include "string16.h"
#include "string_piece.h"

// �����ַ���������ʵ�ú���.

namespace base
{

    // ��װ����"strncasecmp"��"snprintf"���ֲ��ǿ�ƽ̨�ı�׼c����. ����ʵ��ͨ��
    // ��������ƽ̨��صĺ���.

    // �Դ�Сд�޹صķ�ʽ�Ƚ������ַ���s1��s2; �����ֵ��ͷ�ʽ�Ƚ�, ��ȷ���0,
    // s1>s2����1, s2>s1����-1.
    int strcasecmp(const char* s1, const char* s2);

    // �Դ�Сд�޹صķ�ʽ�Ƚ������ַ���s1��s2ǰcount���ַ�; �����ֵ��ͷ�ʽ�Ƚ�,
    // ��ȷ���0, s1>s2����1, s2>s1����-1.
    int strncasecmp(const char* s1, const char* s2, size_t count);

    // char16�汾��strncmp.
    int strncmp16(const char16* s1, const char16* s2, size_t count);

    // ��װvsnprintf��֤�ַ���ʼ����null��β�������ַ�����, ��ʹ�����ض�ʱҲһ��.
    int vsnprintf(char* buffer, size_t size, const char* format, va_list arguments);

    // vsnprintf_s�ַ���ʼ����null��β, �������ض�ʱ, ����-1���߲��ضϸ�ʽ�����ַ���,
    // ʵ��ֵ������׼C���vswprintf�ײ�ʵ��.
    int vswprintf(wchar_t* buffer, size_t size, const wchar_t* format,
        va_list arguments);

    inline int snprintf(char* buffer, size_t size, const char* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);
        int result = vsnprintf(buffer, size, format, arguments);
        va_end(arguments);
        return result;
    }

    inline int swprintf(wchar_t* buffer, size_t size, const wchar_t* format, ...)
    {
        va_list arguments;
        va_start(arguments, format);
        int result = vswprintf(buffer, size, format, arguments);
        va_end(arguments);
        return result;
    }

    // BSD�����ַ�����������.
    // ����|src|��|dst|, |dst_size|��|dst|����. ��࿽��|dst_size|-1���ַ�,
    // ֻҪ|dst_size|����0, |dst|��NULL��β. ����|src|���ַ�������.
    // �������ֵ>=dst_size����������ض�.
    // ע��: ���ȶ����ַ������������ֽ���.
    size_t strlcpy(char* dst, const char* src, size_t dst_size);
    size_t wcslcpy(wchar_t* dst, const wchar_t* src, size_t dst_size);

    // ɨ��wprintf�ĸ�ʽ��ȷ���Ƿ�ƽ̨����. ����ֻ�������ƽ̨֧�ֵ�ת�����Ʒ�,
    // ������ʽ������������.
    //
    // wprintf������ֲ��ת�����Ʒ���:
    //  - û��'l'�������η���'s'��'c'. %s��%cת��char����, ����Windowsƽ̨����(
    //    ת��wchar_t����), %ls��%lcת��wchar_t����.
    //  - 'S'��'C'ת��wchar_t����, ����Windowsƽ̨����(ת��char����). Windowsƽ̨
    //    ʹ��%ls��%lcת��wchar_t����.
    //  - Windowsƽ̨��wprintf��ʶ��'F'.
    //  - 'D', 'O'��'U'�Ѿ�������, ʹ��%ld, %lo��%lu�滻.
    //
    // ע��ʹ��wprintfʱchar����û�п���ֲ��ת�����Ʒ�.
    //
    // ������base::vswprintf��ʹ��.
    bool IsWprintfFormatPortable(const wchar_t* format);

} // namespace base

namespace base
{

    // �Լ��Ĵ����в�Ҫʹ��malloc�����ַ���; ������������APIs.
    inline char* strdup(const char* str)
    {
        return _strdup(str);
    }

    inline int strcasecmp(const char* s1, const char* s2)
    {
        return _stricmp(s1, s2);
    }

    inline int strncasecmp(const char* s1, const char* s2, size_t count)
    {
        return _strnicmp(s1, s2, count);
    }

    inline int strncmp16(const char16* s1, const char16* s2, size_t count)
    {
        return ::wcsncmp(s1, s2, count);
    }

    inline int vsnprintf(char* buffer, size_t size, const char* format,
        va_list arguments)
    {
        int length = vsnprintf_s(buffer, size, size-1, format, arguments);
        if(length < 0)
        {
            return _vscprintf(format, arguments);
        }
        return length;
    }

    inline int vswprintf(wchar_t* buffer, size_t size, const wchar_t* format,
        va_list arguments)
    {
        DCHECK(IsWprintfFormatPortable(format));

        int length = _vsnwprintf_s(buffer, size, size-1, format, arguments);
        if(length < 0)
        {
            return _vscwprintf(format, arguments);
        }
        return length;
    }

} // namespace base

// ��Щ�̰߳�ȫ�ĺ�������ȫ��ͬһ�����ַ���.
//
// һ������²�Ҫ���ڹ��캯��. ֻ��һ��ʹ������: ������Ҫ����һ���ַ�������(
// �����Ա����), ��û�п��ַ�������ʱ(������������). ��Ҫ���ں��������� 
// Ĭ��ֵ���ߵ��������ķ���ֵ.
const std::string& EmptyString();
const std::wstring& EmptyWString();
const string16& EmptyString16();

extern const wchar_t kWhitespaceWide[];
extern const char16 kWhitespaceUTF16[];
extern const char kWhitespaceASCII[];

extern const char kUtf8ByteOrderMark[];

// ��input�Ƴ�remove_chars�����е��ַ�. ����Ƴ����ַ��򷵻�true.
// ע��: input��outputʹ��ͬһ�����ǰ�ȫ��.
bool RemoveChars(const std::wstring& input, const wchar_t remove_chars[],
                 std::wstring* output);
bool RemoveChars(const string16& input, const char16 remove_chars[],
                 string16* output);
bool RemoveChars(const std::string& input, const char remove_chars[],
                 std::string* output);

// ��input�����Ƴ�trim_chars�����е��ַ�.
// ע��: input��outputʹ��ͬһ�����ǰ�ȫ��.
bool TrimString(const std::wstring& input, const wchar_t trim_chars[],
                std::wstring* output);
bool TrimString(const string16& input, const char16 trim_chars[],
                string16* output);
bool TrimString(const std::string& input, const char trim_chars[],
                std::string* output);

// �ַ����ض����ӽ�byte_size�ֽڴ�С.
void TruncateUTF8ToByteSize(const std::string& input, const size_t byte_size,
                            std::string* output);

// ָ��ȥ��input���˵Ŀհ��ַ�, ���������˿հ����.
// �ǿ��ַ��汾��2������:
// * TrimWhitespace(const std::string& ...)
//   �����������ASCII�ַ���, ֻʶ��ASCII�հ��ַ�.
// ������Ҫѡ��ͬ����, ����������ͱ���һ���Ա�֤��ȫ.
// ע��: input��outputʹ��ͬһ�����ǰ�ȫ��.
enum TrimPositions
{
    TRIM_NONE     = 0,
    TRIM_LEADING  = 1 << 0,
    TRIM_TRAILING = 1 << 1,
    TRIM_ALL      = TRIM_LEADING | TRIM_TRAILING,
};
TrimPositions TrimWhitespace(const std::wstring& input, TrimPositions positions,
                             std::wstring* output);
TrimPositions TrimWhitespace(const string16& input, TrimPositions positions,
                             string16* output);
TrimPositions TrimWhitespace(const std::string& input, TrimPositions positions,
                             std::string* output);
TrimPositions TrimWhitespaceASCII(const std::string& input, TrimPositions positions,
                                  std::string* output);

// ����CR����LF�ַ�. �Ƴ����а���CR����LF�������հ�. ���ڴ����ն˿������ַ�.
// ���ص�|text|������������ת��:
// (1) �Ƴ���ͷ�ͽ�β�Ŀհ�.
// (2) ���|trim_sequences_with_line_breaks|Ϊ��, �Ƴ����а���CR����LF�������հ�.
// (3) �����������հ��ַ�ת����һ���հ��ַ�.
std::wstring CollapseWhitespace(const std::wstring& text,
                                bool trim_sequences_with_line_breaks);
string16 CollapseWhitespace(const string16& text,
                            bool trim_sequences_with_line_breaks);
std::string CollapseWhitespaceASCII(const std::string& text,
                                    bool trim_sequences_with_line_breaks);

// ����ַ���Ϊ�ջ���ֻ���пհ��ַ��򷵻�true.
bool ContainsOnlyWhitespaceASCII(const std::string& str);
bool ContainsOnlyWhitespace(const string16& str);

// ���|input|Ϊ�ջ���ֻ����|characters|�е��ַ��򷵻�true.
bool ContainsOnlyChars(const std::wstring& input, const std::wstring& characters);
bool ContainsOnlyChars(const string16& input, const string16& characters);
bool ContainsOnlyChars(const std::string& input, const std::string& characters);

// ͨ���ضϷ�ʽת����7-bit ASCII��, ��ҪԤ��֪��ת��ǰ���ݶ���ASCII��.
std::string WideToASCII(const std::wstring& wide);
std::string UTF16ToASCII(const string16& utf16);

// ת�����ַ�����Ӧ��Latin1. �����ַ�����255��ת��ʧ��(����false).
bool WideToLatin1(const std::wstring& wide, std::string* latin1);

// �ɹ�����true. ���ʶ��һ�����ַ���8-bit����UTF8? ǰ��ֻ����<256���ַ�;
// ����Ҳֻʹ��8-bit�ַ������ܱ�ʾUTF-8�ַ���.
//
// IsStringUTF8��������ʶ�������Ƿ�Ϸ�, �����ж��Ƿ��з�ASCII������(���� U+FFFE).
// ��������Ϊ���õ������ܾ���ʶ����������. �����ֻ��Ҫʶ�������Ƿ�Ϸ�����,
// �����¼�һ������.
bool IsStringUTF8(const std::string& str);
bool IsStringASCII(const std::wstring& str);
bool IsStringASCII(const base::StringPiece& str);
bool IsStringASCII(const string16& str);

// ASCII�ض���tolower.
template<class Char>
inline Char ToLowerASCII(Char c)
{
    return (c>='A' && c<='Z') ? (c+('a'-'A')) : c;
}

template<class str>
inline void StringToLowerASCII(str* s)
{
    for(typename str::iterator i=s->begin(); i!=s->end(); ++i)
    {
        *i = ToLowerASCII(*i);
    }
}

template<class str>
inline str StringToLowerASCII(const str& s)
{
    // ���std::string��std::wstring
    str output(s);
    StringToLowerASCII(&output);
    return output;
}

// ASCII�ض���toupper.
template<class Char>
inline Char ToUpperASCII(Char c)
{
    return (c>='a' && c<='z') ? (c+('A'-'a')) : c;
}

template<class str>
inline void StringToUpperASCII(str* s)
{
    for(typename str::iterator i=s->begin(); i!=s->end(); ++i)
    {
        *i = ToUpperASCII(*i);
    }
}

template<class str>
inline str StringToUpperASCII(const str& s)
{
    // ���std::string��std::wstring
    str output(s);
    StringToUpperASCII(&output);
    return output;
}

// ��Сд��ʽ�Ƚϸ������ַ���. У�������ַ����Ƿ�ƥ��һЩ���ʱ�ǳ�����, ����
// �����Ż�����������ʱ�ַ�����.
bool LowerCaseEqualsASCII(const std::string& a, const char* b);
bool LowerCaseEqualsASCII(const std::wstring& a, const char* b);
bool LowerCaseEqualsASCII(const string16& a, const char* b);

// ʹ�õ�����.
bool LowerCaseEqualsASCII(std::string::const_iterator a_begin,
                          std::string::const_iterator a_end, const char* b);
bool LowerCaseEqualsASCII(std::wstring::const_iterator a_begin,
                          std::wstring::const_iterator a_end, const char* b);
bool LowerCaseEqualsASCII(string16::const_iterator a_begin,
                          string16::const_iterator a_end, const char* b);
bool LowerCaseEqualsASCII(const char* a_begin, const char* a_end, const char* b);
bool LowerCaseEqualsASCII(const wchar_t* a_begin, const wchar_t* a_end,
                          const char* b);
bool LowerCaseEqualsASCII(const char16* a_begin, const char16* a_end, const char* b);

// ��Сд��ص��ַ����Ƚ�. ���2���ַ���������ASCII����Ϊ��ȷ��.
bool EqualsASCII(const string16& a, const base::StringPiece& b);

// ���str��search��ͷ����true, ���򷵻�false.
bool StartsWithASCII(const std::string& str, const std::string& search,
                     bool case_sensitive);
bool StartsWith(const std::wstring& str, const std::wstring& search,
                bool case_sensitive);
bool StartsWith(const string16& str, const string16& search,
                bool case_sensitive);

// ���str��search��β����true, ���򷵻�false.
bool EndsWith(const std::string& str, const std::string& search,
              bool case_sensitive);
bool EndsWith(const std::wstring& str, const std::wstring& search,
              bool case_sensitive);
bool EndsWith(const string16& str, const string16& search,
              bool case_sensitive);

// ȷ��ASCII�ַ�����.
template<typename Char>
inline bool IsAsciiWhitespace(Char c)
{
    return c==' ' || c=='\r' || c=='\n' || c=='\t';
}
template<typename Char>
inline bool IsAsciiAlpha(Char c)
{
    return ((c>='A') && (c<='Z')) || ((c>='a') && (c<='z'));
}
template<typename Char>
inline bool IsAsciiDigit(Char c)
{
    return c>='0' && c<='9';
}

template<typename Char>
inline bool IsHexDigit(Char c)
{
    return (c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f');
}

template<typename Char>
inline Char HexDigitToInt(Char c)
{
    DCHECK(IsHexDigit(c));
    if(c>='0' && c<='9')
    {
        return c - '0';
    }
    if(c>='A' && c<='F')
    {
        return c - 'A' + 10;
    }
    if(c>='a' && c<='f')
    {
        return c - 'a' + 10;
    }
    return 0;
}

// ����ǿհ��ַ��򷵻�true.
inline bool IsWhitespace(wchar_t c)
{
    return wcschr(kWhitespaceWide, c) != NULL;
}

enum DataUnits
{
    DATA_UNITS_BYTE = 0,
    DATA_UNITS_KIBIBYTE,
    DATA_UNITS_MEBIBYTE,
    DATA_UNITS_GIBIBYTE,
};

// �����ֽڵ���ʾ��λ.
DataUnits GetByteDisplayUnits(int64 bytes);

// ���ؿɶ����ֽ��ַ���, unitsָ������ʾ��λ, ��λ��׺��ѡ.
// ����: FormatBytes(512, DATA_UNITS_KIBIBYTE, true) => "0.5 KB"
// ����: FormatBytes(100*1024, DATA_UNITS_MEBIBYTE, false) => "0.1"
string16 FormatBytes(int64 bytes, DataUnits units, bool show_units);

// ͬ��, ������"/s"��λ.
// ����: FormatSpeed(512, DATA_UNITS_KIBIBYTE, true) => "0.5 KB/s"
// ����: FormatSpeed(100*1024, DATA_UNITS_MEBIBYTE, false) => "0.1"
string16 FormatSpeed(int64 bytes, DataUnits units, bool show_units);

// �������ֵĸ�ʽ��.
// ����: FormatNumber(1234567) => 1,234,567
string16 FormatNumber(int64 number);

// ��start_offset(ͨ��Ϊ0)��ʼ�滻��һ��|find_this|Ϊ|replace_with|.
void ReplaceFirstSubstringAfterOffset(string16* str,
                                      string16::size_type start_offset,
                                      const string16& find_this,
                                      const string16& replace_with);
void ReplaceFirstSubstringAfterOffset(std::string* str,
                                      std::string::size_type start_offset,
                                      const std::string& find_this,
                                      const std::string& replace_with);

// ��start_offset(ͨ��Ϊ0)��ʼ�滻����|find_this|Ϊ|replace_with|.
//
// ���������Ӵ�; ���ڵ�һ�ַ�ʹ��<algorithm>�е�std::replace, ����:
//     std::replace(str.begin(), str.end(), 'a', 'b');
void ReplaceSubstringsAfterOffset(string16* str,
                                  string16::size_type start_offset,
                                  const string16& find_this,
                                  const string16& replace_with);
void ReplaceSubstringsAfterOffset(std::string* str,
                                  std::string::size_type start_offset,
                                  const std::string& find_this,
                                  const std::string& replace_with);

// һ�ֽ�ʡstring�����İ취, ���д����wchar_t[]��std::wstring�е�����.
// Ԥ���㹻�Ŀռ䲢�����ַ�����Ϊ��ȷ��ֵ��֤.length()��������.
//
// reserve()����ռ���Ҫ��ĩβ���һ���ս�����. ����������Ϊresize()����
// ��֤Ԥ���ս�����. ����resize()ֻ��Ϊ�˸ı�string��'length'��Ա.
//
// ����: ����wide.resize()������ʱ�临�Ӷ�, ��Ϊ��Ҫ�ÿ��ַ���仺����.
// ����������Ϊ��Ҫ�ı��ַ�����ʵ�ʳ���(ֱ��д����������ı�ʵ�ʳ���).
// �����г���ʱ�临�Ӷȸı�string���ȵİ취.
template<class string_type>
inline typename string_type::value_type* WriteInto(string_type* str,
                                                   size_t length_with_null)
{
    str->reserve(length_with_null);
    str->resize(length_with_null-1);
    return &((*str)[0]);
}

// �����ַ����Ƚ�/�����ķº���.

template<typename Char> struct CaseInsensitiveCompare
{
public:
    bool operator()(Char x, Char y) const
    {
        return tolower(x) == tolower(y);
    }
};

template<typename Char> struct CaseInsensitiveCompareASCII
{
public:
    bool operator()(Char x, Char y) const
    {
        return ToLowerASCII(x) == ToLowerASCII(y);
    }
};

// ʹ��|delimiters|�����ַ��з��ַ���, ÿһ�ζ����ӵ�|tokens|��,
// �����ҵ��ı������.
size_t Tokenize(const std::wstring& str, const std::wstring& delimiters,
                std::vector<std::wstring>* tokens);
size_t Tokenize(const string16& str, const string16& delimiters,
                std::vector<string16>* tokens);
size_t Tokenize(const std::string& str, const std::string& delimiters,
                std::vector<std::string>* tokens);
size_t Tokenize(const base::StringPiece& str,
                const base::StringPiece& delimiters,
                std::vector<base::StringPiece>* tokens);

// SplitString()�ķ�����.
string16 JoinString(const std::vector<string16>& parts, char16 s);
std::string JoinString(const std::vector<std::string>& parts, char s);

// ����: ����ʹ��HTML5�淶����Ŀհ�. ������ĺ�������ʵ��ȥ���ַ������пհ׵�
// ����.
//
// �ؿհ�(HTML 5��������ֿհ��ַ�)�з��ַ���. �����ķǿհ��ַ������ӵ������.
void SplitStringAlongWhitespace(const std::wstring& str,
                                std::vector<std::wstring>* result);
void SplitStringAlongWhitespace(const string16& str,
                                std::vector<string16>* result);
void SplitStringAlongWhitespace(const std::string& str,
                                std::vector<std::string>* result);

// ��$1-$2-$3..$9��ʽ���ַ����滻��|a|-|b|-|c|..|i|. ������'$'�ַ�ȥ��һ��,
// ����: $$->$, $$$->$$��. offsets����ΪNULL. �������9���滻.
string16 ReplaceStringPlaceholders(const string16& format_string,
                                   const std::vector<string16>& subst,
                                   std::vector<size_t>* offsets);

std::string ReplaceStringPlaceholders(const base::StringPiece& format_string,
                                      const std::vector<std::string>& subst,
                                      std::vector<size_t>* offsets);

// ReplaceStringPlaceholders�ĵ�һ�ַ����򻯰汾. offsets����ΪNULL.
string16 ReplaceStringPlaceholders(const string16& format_string,
                                   const string16& a,
                                   size_t* offset);

// ���|input|���ȴ���|max_len|, ��������true, �ڱ�֤��󳤶ȵĻ�����ʹ��...�滻
// �м���ַ�����|output|.
// ����: ElideString(L"Hello", 10, &str)���Hello��output������false.
// ElideString(L"Hello my name is Tom", 10, &str)���"Hell...Tom"��output������true.
bool ElideString(const std::wstring& input, int max_len, std::wstring* output);

// stringƥ��pattern�ɹ�����true. pattern�ַ������԰���ͨ���*��?.
// ��б���ַ�(\)��*��?��ת���ַ�. ����pattern��ຬ��16��*����?�ַ�.
// ?ƥ��0����1���ַ�, *ƥ��0���߶���ַ�.
bool MatchPattern(const base::StringPiece& string,
                  const base::StringPiece& pattern);
bool MatchPattern(const string16& string, const string16& pattern);

// ת�������ַ����͵���Ӧ���޷�������.
// ����: char, signed char, unsigned char -> unsigned char.
template<typename T>
struct ToUnsigned
{
    typedef T Unsigned;
};

template<>
struct ToUnsigned<char>
{
    typedef unsigned char Unsigned;
};
template<>
struct ToUnsigned<signed char>
{
    typedef unsigned char Unsigned;
};
template<>
struct ToUnsigned<wchar_t>
{
    typedef unsigned short Unsigned;
};
template<>
struct ToUnsigned<short>
{
    typedef unsigned short Unsigned;
};

#endif //__string_util_h__