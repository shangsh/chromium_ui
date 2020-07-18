
#ifndef __json_writer_h__
#define __json_writer_h__

#pragma once

#include <string>

#include "base/basic_types.h"

class Value;

namespace base
{

    class JSONWriter
    {
    public:
        // ���ݸ����ĸ��ڵ�����JSON�ַ�������ӵ�|json|. ���|pretty_print|Ϊtrue,
        // ���صĸ�ʽ���õ�json�ַ���(�ÿո񲹰��Ա��Ķ�). ���|pretty_print|Ϊ
        // false, ���ɾ����ܽ��յ��ַ���.
        // TODO: �Ƿ��������(����|node|����DictionaryValue/ListValue������������
        // ��/������󸡵���)�Ƿ�Ӧ������json?
        static void Write(const Value* const node, bool pretty_print,
            std::string* json);

        // ͬ��. ���Ƿ���Ҫ���ַ���ת���ѡ��, �����Զ����Ʒ�ʽ(UTF8)���ݽ������
        // JSON������.
        static void WriteWithOptionalEscape(const Value* const node,
            bool pretty_print, bool escape, std::string* json);

        // JSON��̬�����ַ���, ��ʾ������, ���ڴ��ݿյ�JSON����.
        static const char* kEmptyArray;

    private:
        JSONWriter(bool pretty_print, std::string* json);

        // �ݹ���ù���JSON�ַ���. ���ʱ�������json_string_��.
        void BuildJSONString(const Value* const node, int depth, bool escape);

        // ���һ����""������ת�����(UTF-8)�ַ�����json_string_.
        void AppendQuotedString(const std::string& str);

        // ��ӿո�json_string_���ڲ㼶����.
        void IndentLine(int depth);

        // �洢���ɵ�JSON����.
        std::string* json_string_;

        bool pretty_print_;

        DISALLOW_COPY_AND_ASSIGN(JSONWriter);
    };

} //namespace base

#endif //__json_writer_h__