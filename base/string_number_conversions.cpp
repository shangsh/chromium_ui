
#include "string_number_conversions.h"

#include "logging.h"

namespace base
{

    template<typename STR, typename INT, typename UINT, bool NEG>
    struct IntToStringT
    {
        // �����޷������Ͷ�����ȡ���ı���������.
        // ��������Ĵ���:
        //     template<typename INT>
        //     INT abs(INT value) { return value<0 ? -value : value; }
        // ��ʹINT���޷�������, value<0��Զ������, ȡ�����������ܱ�ִ��, ������
        // ��Ȼ���������. ����ʹ��һЩ����������...
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

        // ģ���������ģ��ǳ�����, ���ڲ��������Ƿ�Ϊ��.
        template<typename INT2, bool NEG2>
        struct TestNegT {};
        template<typename INT2>
        struct TestNegT<INT2, false>
        {
            static bool TestNeg(INT2 value)
            {
                // value���޷��ŵ�, ������Ϊ��.
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
            // ÿ������Ҫ��Լ0.3�ֽ�(log10(2) ~= 0.3), ����˵ÿ�ֽڴ�Լ��Ҫ2.4�ֽ�
            // (log10(2**8) ~= 2.4). ����ȡ��ÿ�ֽ���Ҫ����3���ַ�, Ϊ'-'Ԥ��һλ.
            const int kOutputBufSize = 3*sizeof(INT) + 1;

            // ���������ַ���, �Ӻ���ǰд, ������Ӵ�.
            STR outbuf(kOutputBufSize, 0);

            bool is_neg = TestNegT<INT, NEG>::TestNeg(value);
            UINT res = ToUnsignedT<INT, UINT, NEG>::ToUnsigned(value);

            for(typename STR::iterator it=outbuf.end(); ;)
            {
                --it;
                DCHECK(it != outbuf.begin());
                *it = static_cast<typename STR::value_type>((res%10) + '0');
                res /= 10;

                // ���
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

    // �ַ���������ͨ��ת��.
    //
    // StringToNumberTraitsӦ���ṩ:
    // - ��������string_type, STL string��Ϊ����.
    // - ��������value_type, Ŀ����������.
    // - ��̬����convert_func, ��������strtol����, ����value_type����.
    // - ��̬����valid_func, ��֤|input|�Ϸ���, ����boolֵ. convert_func����
    //   ��ת���Ǵ�����ݵ�, valid_func������󲢷���false, Ҳ���⿪ͷ�Ŀհ�.
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

        // ���ش���������
        // - ���errno��ERANGE, ��ʾ���������Խ��.
        // - �����ַ�Ϊ��, û���κν���.
        // - endptrû��ָ���ַ���ĩβ, ��ʾ��β�з������ַ����߿�ͷ�в��ɽ�����
        //   ���ֵ��ַ�.
        // - valid_func���input���Ϸ�.
        return errno==0 && !input.empty() && input.c_str()+input.length()==endptr
            && traits::valid_func(input);
    }

    static int strtoi(const char* nptr, char** endptr, int base)
    {
        long res = strtol(nptr, endptr, base);
#if __LP64__
        // Long��64λ, �Լ�����Խ��/���.
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
        // Long��64λ, �Լ�����Խ��/���. ���Խ���Ƿ�����32λ�洢.
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

    // HexStringʹ��strtoul�����޷��ŵ�convert_func, ������"0x80000000"������
    // ���벻�ᵼ�����.
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
            uint8 msb = 0; // ��λ4����
            uint8 lsb = 0; // ��λ4����
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
        // 32�ֽڵĻ������㹻.
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

        // ÿ�ֽ����2��ʮ�������ַ�.
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