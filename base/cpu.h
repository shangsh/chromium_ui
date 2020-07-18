
#ifndef __cpu_h__
#define __cpu_h__

#pragma once

#include <string>

namespace base
{

    // ��ѯ������(CPU)��Ϣ.
    class CPU
    {
    public:
        CPU();

        // CPU��Ϣ���ʽӿ�
        const std::string& vendor_name() const { return cpu_vendor_; }
        int stepping() const { return stepping_; }
        int model() const { return model_; }
        int family() const { return family_; }
        int type() const { return type_; }
        int extended_model() const { return ext_model_; }
        int extended_family() const { return ext_family_; }

    private:
        // ��ѯ��������CPUID��Ϣ.
        void Initialize();

        int type_;     // process type
        int family_;   // family of the processor
        int model_;    // model of processor
        int stepping_; // processor revision number
        int ext_model_;
        int ext_family_;
        std::string cpu_vendor_;
    };

} //namespace base

#endif //__cpu_h__