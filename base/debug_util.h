
#ifndef __debug_util_h__
#define __debug_util_h__

// ��������صĸ�������. ���ڲ��Գ����Ƿ��ڵ������������Լ�
// �Ƿ���Ҫ�ڵ��������ж�.

#pragma once

#include <iosfwd>

struct _EXCEPTION_POINTERS;

namespace base
{

    // ��ջ�����ڵ��Ե�ʱ��ǳ�����. �����ڶ��������StackTrace��Ա(
    // һ����#ifndef NDEBUG��), ��������֪�������������ﴴ����.
    class StackTrace
    {
    public:
        // �ڵ�ǰλ�ù���һ��stacktrace.
        StackTrace();

        // ȱʡ�Ŀ�������͸�ֵ���������������.

        // Ϊ�쳣����stacktrace.
        // ע��: ���������û��dbghelp 5.1��ϵͳ�ϻ��׳��Ҳ�����ڵ�
        // (an import not found (StackWalk64))���쳣.
        StackTrace(_EXCEPTION_POINTERS* exception_pointers);

        // ��ö�ջ��Ϣ����
        //   count: ���ض�ջ��Ϣ������.
        const void* const* Addresses(size_t* count);
        // ��ӡ��ջ������Ϣ����׼���.
        void PrintBacktrace();
        // ���ݽ������ű�д������.
        void OutputToStream(std::ostream* os);

    private:
        // �μ�http://msdn.microsoft.com/en-us/library/bb204633.aspx,
        // FramesToSkip��FramesToCapture֮�ͱ���С��63, �������ö�ջ����
        // ��������ֵΪ62, ��ʹ����ƽ̨���ṩ�����ֵ, һ��Ҳûʲô����.
        static const int MAX_TRACES = 62;
        void* trace_[MAX_TRACES];
        int count_;
    };

    class DebugUtil
    {
    public:
        // ����ϵͳ��ע���JIT�����������ӵ�ָ������.
        static bool SpawnDebuggerOnProcess(unsigned process_id);

        // �ȴ�wait_seconds���Ա���������ӵ���ǰ����. ��silent==false, ��⵽
        // ����������׳��쳣.
        static bool WaitForDebugger(int wait_seconds, bool silent);

        // �ڵ�����������?
        static bool BeingDebugged();

        // �ڵ��������ж�, ǰ���Ǵ��ڵ�����.
        static void BreakDebugger();

        // ���ڲ��Դ������õ�.
        static void SuppressDialogs()
        {
            suppress_dialogs_ = true;
        }

    private:
        // true: �ڷǽ��������²���ʾ�κζԻ�������������������.
        static bool suppress_dialogs_;
    };

} //namespace base

#endif //__debug_util_h__