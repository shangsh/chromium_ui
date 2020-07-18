
#ifndef __sha1_h__
#define __sha1_h__

#pragma once

#include <string>

namespace base
{

    enum
    {
        SHA1_LENGTH = 20 // SHA-1�Ĺ�ϣֵ����.
    };

    // ���������ַ���|str|��SHA-1��ϣ�����ع�ϣֵ.
    std::string SHA1HashString(const std::string& str);

} //namespace base

#endif //__sha1_h__