
#ifndef __sha1_h__
#define __sha1_h__

#pragma once

#include <string>

namespace base
{

    enum
    {
        SHA1_LENGTH = 20 // SHA-1的哈希值长度.
    };

    // 计算输入字符串|str|的SHA-1哈希并返回哈希值.
    std::string SHA1HashString(const std::string& str);

} //namespace base

#endif //__sha1_h__