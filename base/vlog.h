
#ifndef __vlog_h__
#define __vlog_h__

#pragma once

#include <vector>

#include "basic_types.h"
#include "string_piece.h"

namespace base
{

    // ����vlogging�������õĸ�����
    class VlogInfo
    {
    public:
        // |v_switch|ȱʡֵ��0, ����V-logging����ȼ�, һ��ʹ������.
        //
        // |vmodule_switch|����ÿ��ģ�������V-logging�ȼ�, ����ȱʡֵ.
        // ����: "my_module=2,foo*=3"���ı�����"my_module.*"��"foo*.*"(
        // ����ƥ����"-inl"��׺�ᱻԤ�Ⱥ��Ե�)�����ļ�����־�ȼ�.
        VlogInfo(const std::string& v_switch, const std::string& vmodule_switch);
        ~VlogInfo();

        // ����ָ���ļ���vlog�ȼ�(ͨ���ǵ�ǰ�ļ�).
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