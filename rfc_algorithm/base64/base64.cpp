
#include "base64.h"

#include "third_party/modp_b64/modp_b64.h"

namespace base
{

    bool Base64Encode(const std::string& input, std::string* output)
    {
        std::string temp;
        temp.resize(modp_b64_encode_len(input.size())); // Ԥ���ռ������ֽ�.

        // null��β��Ϊ�����base64���ı�!
        int input_size  = static_cast<int>(input.size());
        int output_size = modp_b64_encode(&(temp[0]), input.data(), input_size);
        if(output_size < 0)
        {
            return false;
        }

        temp.resize(output_size); // ȥ��������ֽ�
        output->swap(temp);
        return true;
    }

    bool Base64Decode(const std::string& input, std::string* output)
    {
        std::string temp;
        temp.resize(modp_b64_decode_len(input.size()));

        // ����null��β��Ϊ����Ƕ���������!
        int input_size = static_cast<int>(input.size());
        int output_size = modp_b64_decode(&(temp[0]), input.data(), input_size);
        if(output_size < 0)
        {
            return false;
        }

        temp.resize(output_size);
        output->swap(temp);
        return true;
    }

} //namespace base