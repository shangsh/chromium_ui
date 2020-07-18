
#ifndef __debug_util_h__
#define __debug_util_h__

// 调试器相关的辅助函数. 用于测试程序是否在调试器下运行以及
// 是否需要在调试器里中断.

#pragma once

#include <iosfwd>

struct _EXCEPTION_POINTERS;

namespace base
{

    // 堆栈跟踪在调试的时候非常有用. 比如在对象中添加StackTrace成员(
    // 一般在#ifndef NDEBUG里), 这样就能知道对象是在哪里创建的.
    class StackTrace
    {
    public:
        // 在当前位置构造一个stacktrace.
        StackTrace();

        // 缺省的拷贝构造和赋值构造可以正常工作.

        // 为异常创建stacktrace.
        // 注意: 这个函数在没有dbghelp 5.1的系统上会抛出找不到入口点
        // (an import not found (StackWalk64))的异常.
        StackTrace(_EXCEPTION_POINTERS* exception_pointers);

        // 获得堆栈信息数组
        //   count: 返回堆栈信息的数量.
        const void* const* Addresses(size_t* count);
        // 打印堆栈回溯信息到标准输出.
        void PrintBacktrace();
        // 回溯解析符号表并写入流中.
        void OutputToStream(std::ostream* os);

    private:
        // 参见http://msdn.microsoft.com/en-us/library/bb204633.aspx,
        // FramesToSkip和FramesToCapture之和必须小于63, 所以设置堆栈跟踪
        // 的最大回溯值为62, 即使其它平台能提供更大的值, 一般也没什么意义.
        static const int MAX_TRACES = 62;
        void* trace_[MAX_TRACES];
        int count_;
    };

    class DebugUtil
    {
    public:
        // 启动系统已注册的JIT调试器并附加到指定进程.
        static bool SpawnDebuggerOnProcess(unsigned process_id);

        // 等待wait_seconds秒以便调试器附加到当前进程. 当silent==false, 监测到
        // 调试器后会抛出异常.
        static bool WaitForDebugger(int wait_seconds, bool silent);

        // 在调试器下运行?
        static bool BeingDebugged();

        // 在调试器里中断, 前提是存在调试器.
        static void BreakDebugger();

        // 仅在测试代码中用到.
        static void SuppressDialogs()
        {
            suppress_dialogs_ = true;
        }

    private:
        // true: 在非交互环境下不显示任何对话框以免引起其它问题.
        static bool suppress_dialogs_;
    };

} //namespace base

#endif //__debug_util_h__