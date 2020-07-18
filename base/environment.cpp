
#include "environment.h"

#include <windows.h>

#include "string_util.h"
#include "utf_string_conversions.h"

namespace
{

    class EnvironmentImpl : public base::Environment
    {
    public:
        virtual bool GetVar(const char* variable_name, std::string* result)
        {
            if(GetVarImpl(variable_name, result))
            {
                return true;
            }

            // �����Ļ������������еĵط��Ǵ�д, �еĵط���Сд, ����ȫһ��.
            // ���Ա任��Сд����.
            // ����: HTTP_PROXY���е��û�ϵͳ������http_proxy.
            char first_char = variable_name[0];
            std::string alternate_case_var;
            if(first_char>='a' && first_char<='z')
            {
                alternate_case_var = StringToUpperASCII(std::string(variable_name));
            }
            else if(first_char>='A' && first_char<='Z')
            {
                alternate_case_var = StringToLowerASCII(std::string(variable_name));
            }
            else
            {
                return false;
            }
            return GetVarImpl(alternate_case_var.c_str(), result);
        }

        virtual bool SetVar(const char* variable_name, const std::string& new_value)
        {
            return SetVarImpl(variable_name, new_value);
        }

        virtual bool UnSetVar(const char* variable_name)
        {
            return UnSetVarImpl(variable_name);
        }

    private:
        bool GetVarImpl(const char* variable_name, std::string* result)
        {
            DWORD value_length = ::GetEnvironmentVariable(
                UTF8ToWide(variable_name).c_str(), NULL, 0);
            if(value_length == 0)
            {
                return false;
            }
            if(result)
            {
                wchar_t* value = new wchar_t[value_length];
                ::GetEnvironmentVariable(UTF8ToWide(variable_name).c_str(),
                    value, value_length);
                *result = WideToUTF8(value);
                delete[] value;
            }
            return true;
        }

        bool SetVarImpl(const char* variable_name, const std::string& new_value)
        {
            // �ɹ����ط�0ֵ.
            return !!SetEnvironmentVariable(UTF8ToWide(variable_name).c_str(),
                UTF8ToWide(new_value).c_str());
        }

        bool UnSetVarImpl(const char* variable_name)
        {
            // �ɹ����ط�0ֵ.
            return !!SetEnvironmentVariable(UTF8ToWide(variable_name).c_str(), NULL);
        }
    };

}

namespace base
{

    Environment::~Environment() {}

    // static
    Environment* Environment::Create()
    {
        return new EnvironmentImpl();
    }

    bool Environment::HasVar(const char* variable_name)
    {
        return GetVar(variable_name, NULL);
    }

} //namespace base