
#ifndef __command_line_h__
#define __command_line_h__

#pragma once

#include <map>
#include <string>
#include <vector>

namespace base
{

    // ����������������.
    // ��ѡ���ؿ���ͨ���Ⱥź����һ����ֵ���, ����"-switch=value".
    // ����switchΪǰ׺�Ĳ�������Ϊ���Ӳ���. ��"--"��β�Ĳ�����ֹswitch�Ľ���,
    // ���º����������ݱ���Ϊһ�����Ӳ���.
    // ��һ��ֻ����CommandLine������ʾ��ǰ����������������, ������main()������
    // ��ʼ��(��������ƽ̨��ͬ����).

    class CommandLine
    {
    public:
        // ֻ���������в����Ĺ��췽ʽ.
        enum NoProgram { NO_PROGRAM };
        explicit CommandLine(NoProgram no_program);
        ~CommandLine();

        // �����в�������
        typedef std::wstring StringType;

        // ͨ�������������ַ������г�ʼ��, ������������ǵ�һ���ַ���.
        void ParseFromString(const std::wstring& command_line);
        static CommandLine FromString(const std::wstring& command_line);

        // ����һ���µĿ�������.
        // |program|�ǽ����еĳ�����(argv[0]).
        explicit CommandLine(const std::wstring& program);

        // ��ʼ����ǰ����������CommandLine����. ��Windowsƽ̨���Ե�������
        // ����, ֱ�ӽ���GetCommandLineW(). ��ΪCRT�����������в����ɿ�,
        // �����ǻ�����Ҫ����CommandLineToArgvW����������.
        static void Init(int argc, const char* const* argv);

        // ���ٵ�ǰ���̵�CommandLine����. ����Ҫ���õײ�⵽��ʼ״̬ʱ����(�����ⲿ
        // ���ÿ���Ҫ���������³�ʼ��ʱ). ���Initֻ�����ù�һ��, ������main()������,
        // ����Ҫǿ�Ƶ���Reset().
        static void Reset();

        // CommandLine������ʾ��ǰ���̵�������. ע��: ����ֵ�ǿ��޸ĵ�,
        // ���̰߳�ȫ, �����޸�ʱҪȷ����ȷ��.
        static CommandLine* ForCurrentProcess();

        // ��������а���ָ�������򷵻�true.(��������Сд�޹�)
        bool HasSwitch(const std::string& switch_string) const;

        // ����ָ�����ص�ֵ. ���������ֵ���߲�����, ���ؿ��ַ���.
        std::string GetSwitchValueASCII(const std::string& switch_string) const;
        std::wstring GetSwitchValuePath(const std::string& switch_string) const;
        StringType GetSwitchValueNative(const std::string& switch_string) const;

        // �õ����̵����п�����.
        size_t GetSwitchCount() const { return switches_.size(); }

        // map����: ���������Ŀ��ص�key��values
        typedef std::map<std::string, StringType> SwitchMap;

        // ��ȡ���еĿ���
        const SwitchMap& GetSwitches() const
        {
            return switches_;
        }

        // ��ȡ���������в���
        const std::vector<StringType>& args() const { return args_; }

        // ���س�ʼ���������ַ���.
        const std::wstring& command_line_string() const
        {
            return command_line_string_;
        }

        // �����������еĳ�����(��һ���ַ���).
        std::wstring GetProgram() const;

        // ��ӿ���.
        void AppendSwitch(const std::string& switch_string);

        // ��ӿ���/ֵ��.
        void AppendSwitchPath(const std::string& switch_string,
            const std::wstring& path);
        void AppendSwitchNative(const std::string& switch_string,
            const StringType& value);
        void AppendSwitchASCII(const std::string& switch_string,
            const std::string& value);

        // ��Ӳ���.
        // ע������: ��Ҫʱ�����ŰѲ����������Ա㱻��ȷ����Ϊһ������.
        // AppendArg��Ҫ����ASCII��; ��ASCII����ᱻ��Ϊ��UTF-8�����ʽ.
        void AppendArg(const std::string& value);
        void AppendArgPath(const std::wstring& value);
        void AppendArgNative(const StringType& value);

        // �����һ�������е����в���. ���|include_program|��true, |other|
        // �ĳ�����Ҳ�ᱻ��ӽ���.
        void AppendArguments(const CommandLine& other, bool include_program);

        // �ڵ�ǰ�����в�������, �����������, ��"valgrind" ���� "gdb --args".
        void PrependWrapper(const StringType& wrapper);

        // ����һ���������п���ָ���Ŀ���(����ֵ, �������). һ�����������ӽ���.
        void CopySwitchesFrom(const CommandLine& source, const char* const switches[],
            size_t count);

    private:
        CommandLine();

        // CommandLine������ʾ��ǰ���̵�������.
        static CommandLine* current_process_commandline_;

        // ������������, �Կո�ָ����������ַ���
        std::wstring command_line_string_;
        // ������
        std::wstring program_;

        // ����true�����|switch_string|��|switch_value|���parameter_string
        // ��ʾһ������.
        static bool IsSwitch(const StringType& parameter_string,
            std::string* switch_string, StringType* switch_value);

        // �������Ŀ���ֵ��.
        SwitchMap switches_;

        // �ǿ��ص������в���.
        std::vector<StringType> args_;

        // ���������캯��, ��Ϊ�����´����ǰ���̵������в���ӱ�־λ. ����:
        //     CommandLine cl(*CommandLine::ForCurrentProcess());
        //     cl.AppendSwitch(...);
    };

} //namespace base

#endif //__command_line_h__