
#ifndef __base64_h__
#define __base64_h__

#pragma once

#include <string>

namespace base
{

    // base64����. �ɹ�����trueʧ�ܷ���false. outputֻ�ڳɹ�ʱ���޸�.
    bool Base64Encode(const std::string& input, std::string* output);

    // base64����. �ɹ�����trueʧ�ܷ���false. outputֻ�ڳɹ�ʱ���޸�.
    bool Base64Decode(const std::string& input, std::string* output);

} //namespace base

#endif //__base64_h__