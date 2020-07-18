
#ifndef __vlog_h__
#define __vlog_h__

#pragma once

#include <vector>

#include "basic_types.h"
#include "string_piece.h"

namespace base
{

    // 包含vlogging所有设置的辅助类
    class VlogInfo
    {
    public:
        // |v_switch|缺省值是0, 最大的V-logging激活等级, 一般使用正数.
        //
        // |vmodule_switch|给出每个模块的最大的V-logging等级, 覆盖缺省值.
        // 例如: "my_module=2,foo*=3"将改变所有"my_module.*"和"foo*.*"(
        // 这种匹配中"-inl"后缀会被预先忽略掉)代码文件的日志等级.
        VlogInfo(const std::string& v_switch, const std::string& vmodule_switch);
        ~VlogInfo();

        // 返回指定文件的vlog等级(通常是当前文件).
        int GetVlogLevel(const base::StringPiece& file);

        static const int kDefaultVlogLevel;

    private:
        typedef std::pair<std::string, int> VmodulePattern;

        int max_vlog_level_;
        std::vector<VmodulePattern> vmodule_levels_;

        DISALLOW_COPY_AND_ASSIGN(VlogInfo);
    };

}

#endif //__vlog_h__