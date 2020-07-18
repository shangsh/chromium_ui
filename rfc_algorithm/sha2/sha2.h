
#ifndef __sha2_h__
#define __sha2_h__

#pragma once

#include <string>

namespace base
{

    enum
    {
        SHA256_LENGTH = 32 // SHA-256�Ĺ�ϣֵ����.
    };

    // ���������ַ���|str|��SHA-256��ϣ�����ع�ϣֵ��output������.
    // ���'len'>32, ֻ��洢32�ֽ�(��ϣ�ܳ���)��'output'������.
    void SHA256HashString(const std::string& str, void* output, size_t len);

    // 32�ֽ��ַ������ð汾.
    std::string SHA256HashString(const std::string& str);

} //namespace base

#endif //__sha2_h__