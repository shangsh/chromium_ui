
#ifndef __environment_h__
#define __environment_h__

#pragma once

#include <string>

namespace base
{

    class Environment
    {
    public:
        virtual ~Environment();

        // ��̬�������������ض�ƽ̨ʵ�ֵ�ʵ��.
        static Environment* Create();

        // ��ȡ����������ֵ����|result|. ���keyδ���÷���false.
        virtual bool GetVar(const char* variable_name, std::string* result) = 0;

        // ��ͬGetVar(variable_name, NULL);
        virtual bool HasVar(const char* variable_name);

        // �ɹ�����true, ���򷵻�false.
        virtual bool SetVar(const char* variable_name,
            const std::string& new_value) = 0;

        // �ɹ�����true, ���򷵻�false.
        virtual bool UnSetVar(const char* variable_name) = 0;
    };

} //namespace base

#endif //__environment_h__