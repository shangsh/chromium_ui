
#include "json_writer.h"

#include "base/logging.h"
#include "base/string_number_conversions.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "base/values.h"

#include "string_escape.h"

namespace base
{

    static const char kPrettyPrintLineEnding[] = "\r\n";

    /* static */
    const char* JSONWriter::kEmptyArray = "[]";

    /* static */
    void JSONWriter::Write(const Value* const node,
        bool pretty_print, std::string* json)
    {
        WriteWithOptionalEscape(node, pretty_print, true, json);
    }

    /* static */
    void JSONWriter::WriteWithOptionalEscape(const Value* const node,
        bool pretty_print, bool escape, std::string* json)
    {
        json->clear();
        // �Ƿ��и��õķ������������С?
        json->reserve(1024);
        JSONWriter writer(pretty_print, json);
        writer.BuildJSONString(node, 0, escape);
        if(pretty_print)
        {
            json->append(kPrettyPrintLineEnding);
        }
    }

    JSONWriter::JSONWriter(bool pretty_print, std::string* json)
        : json_string_(json), pretty_print_(pretty_print)
    {
        DCHECK(json);
    }

    void JSONWriter::BuildJSONString(const Value* const node,
        int depth, bool escape)
    {
        switch(node->GetType())
        {
        case Value::TYPE_NULL:
            json_string_->append("null");
            break;

        case Value::TYPE_BOOLEAN:
            {
                bool value;
                bool result = node->GetAsBoolean(&value);
                DCHECK(result);
                json_string_->append(value ? "true" : "false");
                break;
            }

        case Value::TYPE_INTEGER:
            {
                int value;
                bool result = node->GetAsInteger(&value);
                DCHECK(result);
                StringAppendF(json_string_, "%d", value);
                break;
            }

        case Value::TYPE_REAL:
            {
                double value;
                bool result = node->GetAsReal(&value);
                DCHECK(result);
                std::string real = DoubleToString(value);
                // ���û��С��λ����'e', Ϊ�������һ��.0. ��������ȷ����ȡJSON��ʱ��
                // ������Ϊһ��real������int.
                if(real.find('.')==std::string::npos &&
                    real.find('e')==std::string::npos &&
                    real.find('E')==std::string::npos)
                {
                    real.append(".0");
                }
                // JSON�淶�涨(-1,1)����ķ�����ֵ��С��λǰ��һ��0. ".52"�ǷǷ���,
                // "0.52"�ǺϷ���.
                if(real[0] == '.')
                {
                    real.insert(0, "0");
                }
                else if(real.length()>1 && real[0]=='-' && real[1]=='.')
                {
                    // "-.1"�Ƿ� "-0.1"�Ϸ�
                    real.insert(1, "0");
                }
                json_string_->append(real);
                break;
            }

        case Value::TYPE_STRING:
            {
                std::string value;
                bool result = node->GetAsString(&value);
                DCHECK(result);
                if(escape)
                {
                    JsonDoubleQuote(UTF8ToUTF16(value), true, json_string_);
                }
                else
                {
                    JsonDoubleQuote(value, true, json_string_);
                }
                break;
            }

        case Value::TYPE_LIST:
            {
                json_string_->append("[");
                if(pretty_print_)
                {
                    json_string_->append(" ");
                }

                const ListValue* list = static_cast<const ListValue*>(node);
                for(size_t i=0; i<list->GetSize(); ++i)
                {
                    if(i != 0)
                    {
                        json_string_->append(",");
                        if(pretty_print_)
                        {
                            json_string_->append(" ");
                        }
                    }

                    Value* value = NULL;
                    bool result = list->Get(i, &value);
                    DCHECK(result);
                    BuildJSONString(value, depth, escape);
                }

                if(pretty_print_)
                {
                    json_string_->append(" ");
                }
                json_string_->append("]");
                break;
            }

        case Value::TYPE_DICTIONARY:
            {
                json_string_->append("{");
                if(pretty_print_)
                {
                    json_string_->append(kPrettyPrintLineEnding);
                }

                const DictionaryValue* dict =
                    static_cast<const DictionaryValue*>(node);
                for(DictionaryValue::key_iterator key_itr=dict->begin_keys();
                    key_itr!=dict->end_keys(); ++key_itr)
                {
                    if(key_itr != dict->begin_keys())
                    {
                        json_string_->append(",");
                        if(pretty_print_)
                        {
                            json_string_->append(kPrettyPrintLineEnding);
                        }
                    }

                    Value* value = NULL;
                    bool result = dict->GetWithoutPathExpansion(*key_itr, &value);
                    DCHECK(result);

                    if(pretty_print_)
                    {
                        IndentLine(depth+1);
                    }
                    AppendQuotedString(*key_itr);
                    if(pretty_print_)
                    {
                        json_string_->append(": ");
                    }
                    else
                    {
                        json_string_->append(":");
                    }
                    BuildJSONString(value, depth+1, escape);
                }

                if(pretty_print_)
                {
                    json_string_->append(kPrettyPrintLineEnding);
                    IndentLine(depth);
                    json_string_->append("}");
                }
                else
                {
                    json_string_->append("}");
                }
                break;
            }

        default:
            // TODO: ����TYPE_BINARY
            NOTREACHED() << "unknown json type";
        }
    }

    void JSONWriter::AppendQuotedString(const std::string& str)
    {
        // TODO: |str|��UTF-8������ASCII, ����Ϊ����ȷת����Ҫ��ת����UTF-16.
        // �������ص�ת��������õİ취.
        JsonDoubleQuote(UTF8ToUTF16(str), true, json_string_);
    }

    void JSONWriter::IndentLine(int depth)
    {
        // ά��һ�������ַ������ܻ��һЩ, �����Ͳ���ÿ�η���.
        json_string_->append(std::string(depth*3, ' '));
    }

} //namespace base