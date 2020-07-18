
#include "string_escape.h"

#include "base/string_util.h"
#include "base/stringprintf.h"

namespace
{

    // ���Զ�|c|����ת��(\n��). �ɹ�����true����ת���ַ�����ӵ�|dst|. �淶û��
    // ǿ�ƹ涨, ��������\uXXXX��ʽ���߿ɶ���.
    template<typename CHAR>
    static bool JsonSingleEscapeChar(const CHAR c, std::string* dst)
    {
        // ����: �����������µ�ת��, ��Ҫͬʱ����reader.
        // ע��: \v��reader��, JSON�淶����������û����������.
        switch(c)
        {
        case '\b':
            dst->append("\\b");
            break;
        case '\f':
            dst->append("\\f");
            break;
        case '\n':
            dst->append("\\n");
            break;
        case '\r':
            dst->append("\\r");
            break;
        case '\t':
            dst->append("\\t");
            break;
        case '\\':
            dst->append("\\\\");
            break;
        case '"':
            dst->append("\\\"");
            break;
        default:
            return false;
        }
        return true;
    }

    template<class STR>
    void JsonDoubleQuoteT(const STR& str, bool put_in_quotes, std::string* dst)
    {
        if(put_in_quotes)
        {
            dst->push_back('"');
        }

        for(typename STR::const_iterator it=str.begin(); it!=str.end(); ++it)
        {
            typename ToUnsigned<typename STR::value_type>::Unsigned c = *it;
            if(!JsonSingleEscapeChar(c, dst))
            {
                if(c<32 || c>126 || c=='<' || c=='>')
                {
                    unsigned int as_uint = static_cast<unsigned int>(c);
                    // 1. ��<��>ת���ֹ�ű�ִ��.
                    // 2. ����������, ���԰�c>126���ַ������UTF8, ���ǿ�ѡ����.
                    //    ����ʵ������������ô��.
                    base::StringAppendF(dst, "\\u%04X", as_uint);
                }
                else
                {
                    unsigned char ascii = static_cast<unsigned char>(*it);
                    dst->push_back(ascii);
                }
            }
        }

        if(put_in_quotes)
        {
            dst->push_back('"');
        }
    }

}

namespace base
{

    void JsonDoubleQuote(const std::string& str,
        bool put_in_quotes, std::string* dst)
    {
        JsonDoubleQuoteT(str, put_in_quotes, dst);
    }

    std::string GetDoubleQuotedJson(const std::string& str)
    {
        std::string dst;
        JsonDoubleQuote(str, true, &dst);
        return dst;
    }

    void JsonDoubleQuote(const string16& str,
        bool put_in_quotes, std::string* dst)
    {
        JsonDoubleQuoteT(str, put_in_quotes, dst);
    }

    std::string GetDoubleQuotedJson(const string16& str)
    {
        std::string dst;
        JsonDoubleQuote(str, true, &dst);
        return dst;
    }

} //namespace base