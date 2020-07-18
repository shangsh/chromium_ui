
#include "string_number_conversions.h"

#include "logging.h"

namespace base
{

    template<typename STR, typename INT, typename UINT, bool NEG>
    struct IntToStringT
    {
        // 避免无符号类型对输入取反的编译器警告.
        // 例如下面的代码:
        //     template<typename INT>
        //     INT abs(INT value) { return value<0 ? -value : value; }
        // 即使INT是无符号类型, value<0永远不成立, 取反操作不可能被执行, 编译器
        // 仍然会产生警告. 这里使用一些特例化技巧...
        template<typename INT2, typename UINT2, bool NEG2>
        struct ToUnsignedT {};

        template<typename INT2, typename UINT2>
        struct ToUnsignedT<INT2, UINT2, false>
        {
            static UINT2 ToUnsigned(INT2 value)
            {
                return static_cast<UINT2>(value);
            }
        };

        template<typename INT2, typename UINT2>
        struct ToUnsignedT<INT2, UINT2, true>
        {
            static UINT2 ToUnsigned(INT2 value)
            {
                return static_cast<UINT2>(value<0 ? -value : value);
            }
        };

        // 模板与上面的模板非常类似, 用于测试整数是否为负.
        template<typename INT2, bool NEG2>
        struct TestNegT {};
        template<typename INT2>
        struct TestNegT<INT2, false>
        {
            static bool TestNeg(INT2 value)
            {
                // value是无符号的, 不可能为负.
                return false;
            }
        };
        template<typename INT2>
        struct TestNegT<INT2, true>
        {
            static bool TestNeg(INT2 value)
            {
                return value < 0;
            }
        };

        static STR IntToString(INT value)
        {
            // 每比特需要大约0.3字节(log10(2) ~= 0.3), 或者说每字节大约需要2.4字节
            // (log10(2**8) ~= 2.4). 向上取整每字节需要分配3个字符, 为'-'预留一位.
            const int kOutputBufSize = 3*sizeof(INT) + 1;

            // 分配整个字符串, 从后向前写, 最后求子串.
            STR outbuf(kOutputBufSize, 0);

            bool is_neg = TestNegT<INT, NEG>::TestNeg(value);
            UINT res = ToUnsignedT<INT, UINT, NEG>::ToUnsigned(value);

            for(typename STR::iterator it=outbuf.end(); ;)
            {
                --it;
                DCHECK(it != outbuf.begin());
                *it = static_cast<typename STR::value_type>((res%10) + '0');
                res /= 10;

                // 完成
                if(res == 0)
                {
                    if(is_neg)
                    {
                        --it;
                        DCHECK(it != outbuf.begin());
                        *it = static_cast<typename STR::value_type>('-');
                    }
                    return STR(it, outbuf.end());
                }
            }
            NOTREACHED();
            return STR();
        }
    };

    // 字符串到数字通用转换.
    //
    // StringToNumberTraits应该提供:
    // - 定义类型string_type, STL string作为输入.
    // - 定义类型value_type, 目标数字类型.
    // - 静态函数convert_func, 调用类似strtol函数, 返回value_type类型.
    // - 静态函数valid_func, 验证|input|合法性, 返回bool值. convert_func函数
    //   的转换是错误兼容的, valid_func会检测错误并返回false, 也会检测开头的空白.
    template<typename StringToNumberTraits>
    bool StringToNumber(const typename StringToNumberTraits::string_type& input,
        typename StringToNumberTraits::value_type* output)
    {
        typedef StringToNumberTraits traits;

        errno = 0;
        typename traits::string_type::value_type* endptr = NULL;
        typename traits::value_type value = traits::convert_func(
            input.c_str(), &endptr);
        *output = value;

        // 返回错误的情况：
        // - 如果errno是ERANGE, 表示有溢出或者越界.
        // - 输入字符为空, 没有任何解析.
        // - endptr没有指向字符串末尾, 表示结尾有非数字字符或者开头有不可解析成
        //   数字的字符.
        // - valid_func检测input不合法.
        return errno==0 && !input.empty() && input.c_str()+input.length()==endptr
            && traits::valid_func(input);
    }

    static int strtoi(const char* nptr, char** endptr, int base)
    {
        long res = strtol(nptr, endptr, base);
#if __LP64__
        // Long是64位, 自己处理越界/溢出.
        if(res > kint32max)
        {
            res = kint32max;
            errno = ERANGE;
        }
        else if(res < kint32min)
        {
            res = kint32min;
            errno = ERANGE;
        }
#endif
        return static_cast<int>(res);
    }

    static unsigned int strtoui(const char* nptr, char** endptr, int base)
    {
        unsigned long res = strtoul(nptr, endptr, base);
#if __LP64__
        // Long是64位, 自己处理越界/溢出. 测试结果是否能用32位存储.
        if(static_cast<int>(static_cast<long>(res))!=static_cast<long>(res) &&
            static_cast<unsigned int>(res)!=res)
        {
            res = kuint32max;
            errno = ERANGE;
        }
#endif
        return static_cast<unsigned int>(res);
    }

    class StringToIntTraits
    {
    public:
        typedef std::string string_type;
        typedef int value_type;
        static const int kBase = 10;
        static inline value_type convert_func(const string_type::value_type* str,
            string_type::value_type** endptr)
        {
            return strtoi(str, endptr, kBase);
        }
        static inline bool valid_func(const string_type& str)
        {
            return !str.empty() && !isspace(str[0]);
        }
    };

    class String16ToIntTraits
    {
    public:
        typedef string16 string_type;
        typedef int value_type;
        static const int kBase = 10;
        static inline value_type convert_func(const string_type::value_type* str,
            string_type::value_type** endptr)
        {
            return wcstol(str, endptr, kBase);
        }
        static inline bool valid_func(const string_type& str)
        {
            return !str.empty() && !iswspace(str[0]);
        }
    };

    class StringToInt64Traits
    {
    public:
        typedef std::string string_type;
        typedef int64 value_type;
        static const int kBase = 10;
        static inline value_type convert_func(const string_type::value_type* str,
            string_type::value_type** endptr)
        {
            return _strtoi64(str, endptr, kBase);
        }
        static inline bool valid_func(const string_type& str)
        {
            return !str.empty() && !isspace(str[0]);
        }
    };

    class String16ToInt64Traits
    {
    public:
        typedef string16 string_type;
        typedef int64 value_type;
        static const int kBase = 10;
        static inline value_type convert_func(const string_type::value_type* str,
            string_type::value_type** endptr)
        {
            return _wcstoi64(str, endptr, kBase);
        }
        static inline bool valid_func(const string_type& str)
        {
            return !str.empty() && !iswspace(str[0]);
        }
    };

    // HexString使用strtoul这种无符号的convert_func, 所以像"0x80000000"这样的
    // 输入不会导致溢出.
    class HexStringToIntTraits
    {
    public:
        typedef std::string string_type;
        typedef int value_type;
        static const int kBase = 16;
        static inline value_type convert_func(const string_type::value_type* str,
            string_type::value_type** endptr)
        {
            return strtoui(str, endptr, kBase);
        }
        static inline bool valid_func(const string_type& str)
        {
            return !str.empty() && !isspace(str[0]);
        }
    };

    class StringToDoubleTraits
    {
    public:
        typedef std::string string_type;
        typedef double value_type;
        static inline value_type convert_func(const string_type::value_type* str,
            string_type::value_type** endptr)
        {
            return strtod(str, endptr);
        }
        static inline bool valid_func(const string_type& str)
        {
            return !str.empty() && !isspace(str[0]);
        }
    };

    template<class CHAR>
    bool HexDigitToIntT(const CHAR digit, uint8* val)
    {
        if(digit>='0' && digit<='9')
        {
            *val = digit - '0';
        }
        else if(digit>='a' && digit<='f')
        {
            *val = 10 + digit - 'a';
        }
        else if(digit>='A' && digit<='F')
        {
            *val = 10 + digit - 'A';
        }
        else
        {
            return false;
        }
        return true;
    }

    template<typename STR>
    bool HexStringToBytesT(const STR& input, std::vector<uint8>* output)
    {
        DCHECK(output->size() == 0);
        size_t count = input.size();
        if(count==0 || (count%2)!=0)
        {
            return false;
        }
        for(uintptr_t i=0; i<count/2; ++i)
        {
            uint8 msb = 0; // 高位4比特
            uint8 lsb = 0; // 低位4比特
            if(!HexDigitToIntT(input[i*2], &msb) ||
                !HexDigitToIntT(input[i*2+1], &lsb))
            {
                return false;
            }
            output->push_back((msb<<4) | lsb);
        }
        return true;
    }

    std::string IntToString(int value)
    {
        return IntToStringT<std::string, int, unsigned int, true>::
            IntToString(value);
    }

    string16 IntToString16(int value)
    {
        return IntToStringT<string16, int, unsigned int, true>::
            IntToString(value);
    }

    std::string UintToString(unsigned int value)
    {
        return IntToStringT<std::string, unsigned int, unsigned int, false>::
            IntToString(value);
    }

    string16 UintToString16(unsigned int value)
    {
        return IntToStringT<string16, unsigned int, unsigned int, false>::
            IntToString(value);
    }

    std::string Int64ToString(int64 value)
    {
        return IntToStringT<std::string, int64, uint64, true>::
            IntToString(value);
    }

    string16 Int64ToString16(int64 value)
    {
        return IntToStringT<string16, int64, uint64, true>::IntToString(value);
    }

    std::string Uint64ToString(uint64 value)
    {
        return IntToStringT<std::string, uint64, uint64, false>::
            IntToString(value);
    }

    string16 Uint64ToString16(uint64 value)
    {
        return IntToStringT<string16, uint64, uint64, false>::
            IntToString(value);
    }

    std::string DoubleToString(double value)
    {
        // 32字节的缓存区足够.
        char buffer[32];
        sprintf_s(buffer, 32, "%f", value);
        return std::string(buffer);
    }

    bool StringToInt(const std::string& input, int* output)
    {
        return StringToNumber<StringToIntTraits>(input, output);
    }

    bool StringToInt(const string16& input, int* output)
    {
        return StringToNumber<String16ToIntTraits>(input, output);
    }

    bool StringToInt64(const std::string& input, int64* output)
    {
        return StringToNumber<StringToInt64Traits>(input, output);
    }

    bool StringToInt64(const string16& input, int64* output)
    {
        return StringToNumber<String16ToInt64Traits>(input, output);
    }

    bool StringToDouble(const std::string& input, double* output)
    {
        return StringToNumber<StringToDoubleTraits>(input, output);
    }

    std::string HexEncode(const void* bytes, size_t size)
    {
        static const char kHexChars[] = "0123456789ABCDEF";

        // 每字节输出2个十六进制字符.
        std::string ret(size*2, '\0');

        for(size_t i=0; i<size; ++i)
        {
            char b = reinterpret_cast<const char*>(bytes)[i];
            ret[(i*2)] = kHexChars[(b>>4) & 0xf];
            ret[(i*2)+1] = kHexChars[b & 0xf];
        }
        return ret;
    }

    bool HexStringToInt(const std::string& input, int* output)
    {
        return StringToNumber<HexStringToIntTraits>(input, output);
    }

    bool HexStringToBytes(const std::string& input, std::vector<uint8>* output)
    {
        return HexStringToBytesT(input, output);
    }

} //namespace base