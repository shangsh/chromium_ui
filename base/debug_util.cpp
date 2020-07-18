
#include "debug_util.h"

#pragma comment(lib, "dbghelp.lib")

#include <windows.h>
#include <dbghelp.h>

#include <iostream>

#include "basic_types.h"
#include "lock.h"
#include "logging.h"
#include "singleton.h"

// 最小的读注册表函数.
// 注意: 没使用CRT.
bool RegReadString(HKEY root, const wchar_t* subkey, const wchar_t* value_name,
                   wchar_t* buffer, int* len)
{
    HKEY key = NULL;
    DWORD res = RegOpenKeyEx(root, subkey, 0, KEY_READ, &key);
    if(ERROR_SUCCESS!=res || key==NULL)
    {
        return false;
    }

    DWORD type = 0;
    DWORD buffer_size = *len * sizeof(wchar_t);
    // 不支持REG_EXPAND_SZ
    res = RegQueryValueEx(key, value_name, NULL, &type,
        reinterpret_cast<BYTE*>(buffer), &buffer_size);
    if(ERROR_SUCCESS==res && buffer_size!=0 && type==REG_SZ)
    {
        // 确保buffer是NULL结束的.
        buffer[*len-1] = 0;
        *len = lstrlenW(buffer);
        RegCloseKey(key);
        return true;
    }
    RegCloseKey(key);
    return false;
}

// 替换input中的每个"%ld", 不高效但能正常工作.
// 注意: 没使用CRT.
bool StringReplace(const wchar_t* input, int value, wchar_t* output,
                   int output_len)
{
    memset(output, 0, output_len*sizeof(wchar_t));
    int input_len = lstrlenW(input);

    for(int i=0; i<input_len; ++i)
    {
        int current_output_len = lstrlenW(output);

        if(input[i]==L'%' && input[i+1]==L'l' && input[i+2]==L'd')
        {
            // 确保有足够的剩余空间.
            if((current_output_len+12) >= output_len)
            {
                return false;
            }

            // 廉价的_itow().
            wsprintf(output+current_output_len, L"%d", value);
            i += 2;
        }
        else
        {
            if(current_output_len >= output_len)
            {
                return false;
            }
            output[current_output_len] = input[i];
        }
    }
    return true;
}

// SymbolContext是一个封装了DbgHelp的Sym*系列函数的线程安全的单件.
// Sym*系列函数同一时间只能被一个线程调用. SymbolContext代码可能会通过网络
// 访问symbol服务器, 这个时候锁会导致延迟从而影响性能.
//
// 还有一个已知的问题就是: 当使用Sym*系列函数时, 在另外一个线程中调用
// breakpad, backtrace的代码会跟breakpad部分有冲突. 参见bug:
//   http://code.google.com/p/google-breakpad/issues/detail?id=311
// 这种是极端情况, 当前不予考虑.
class SymbolContext
{
public:
    static SymbolContext* Get()
    {
        // 采用leaky单件因为代码可能在进程结束时调用.
        return base::Singleton<SymbolContext,
            base::LeakySingletonTraits<SymbolContext> >::get();
    }

    // 返回初始化失败的错误码.
    DWORD init_error() const
    {
        return init_error_;
    }

    // 对于指定的trace, 尝试解析符号并输出到os流中. 回溯的每行格式如下:
    //     <tab>SymbolName[0xAddress+Offset] (FileName:LineNo)
    // 必须在Init()之后调用. 在这里不要调用LOG(FATAL), 因为可能就是LOG(FATAL)
    // 调用过来的, 这样会引起死循环.
    void OutputTraceToStream(const void* const* trace, int count,
        std::ostream* os)
    {
        base::AutoLock lock(lock_);

        for(size_t i=0; (i<static_cast<size_t>(count))&&os->good(); ++i)
        {
            const int kMaxNameLength = 256;
            DWORD_PTR frame = reinterpret_cast<DWORD_PTR>(trace[i]);

            // 代码是MSDN例子改写的:
            // http://msdn.microsoft.com/en-us/library/ms680578(VS.85).aspx
            ULONG64 buffer[(sizeof(SYMBOL_INFO)+
                kMaxNameLength*sizeof(wchar_t)+sizeof(ULONG64)-1)/sizeof(ULONG64)];
            memset(buffer, 0, sizeof(buffer));

            // 初始化符号信息结构体.
            DWORD64 sym_displacement = 0;
            PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(&buffer[0]);
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = kMaxNameLength - 1;
            BOOL has_symbol = SymFromAddr(GetCurrentProcess(), frame,
                &sym_displacement, symbol);

            // 尝试返回行信息.
            DWORD line_displacement = 0;
            IMAGEHLP_LINE64 line = { 0 };
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            BOOL has_line = SymGetLineFromAddr64(GetCurrentProcess(), frame,
                &line_displacement, &line);

            // 输出回溯行信息.
            (*os) << "\t";
            if(has_symbol)
            {
                (*os) << symbol->Name << " [0x" << trace[i] << "+"
                    << sym_displacement << "]";
            }
            else
            {
                // 没有符号信息.
                (*os) << "(No symbol) [0x" << trace[i] << "]";
            }
            if(has_line)
            {
                (*os) << " (" << line.FileName << ":" << line.LineNumber << ")";
            }
            (*os) << "\n";
        }
    }

private:
    friend struct base::DefaultSingletonTraits<SymbolContext>;

    SymbolContext() : init_error_(ERROR_SUCCESS)
    {
        // 初始化进程的符号表.
        // 需要时延迟加载符号, 所有符号使用未修饰方式, 加载行号信息.
        SymSetOptions(SYMOPT_DEFERRED_LOADS|SYMOPT_UNDNAME|SYMOPT_LOAD_LINES);
        if(SymInitialize(GetCurrentProcess(), NULL, TRUE))
        {
            init_error_ = ERROR_SUCCESS;
        }
        else
        {
            init_error_ = GetLastError();
            DLOG(ERROR) << "SymInitialize failed: " << init_error_;
        }
    }

    DWORD init_error_;
    base::Lock lock_;
    DISALLOW_COPY_AND_ASSIGN(SymbolContext);
};

namespace base
{

    // 注意: 直接加载函数地址避免安装最新的SDK
    typedef USHORT (WINAPI *pfnCaptureStackBackTrace)(
        __in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
    static pfnCaptureStackBackTrace fnCaptureStackBackTrace = NULL;
    StackTrace::StackTrace()
    {
        if(fnCaptureStackBackTrace == NULL)
        {
            fnCaptureStackBackTrace = (pfnCaptureStackBackTrace)GetProcAddress(
                LoadLibraryA("kernel32.dll"), "RtlCaptureStackBackTrace");
        }
        CHECK(fnCaptureStackBackTrace != NULL);

        // 使用CaptureStackBackTrace()获得调用堆栈信息.
        count_ = fnCaptureStackBackTrace(0, arraysize(trace_), trace_, NULL);
    }

    StackTrace::StackTrace(_EXCEPTION_POINTERS* exception_pointers)
    {
        // 使用StackWalk64()获得异常堆栈信息.
        count_ = 0;
        // 初始化堆栈查核行程.
        STACKFRAME64 stack_frame;
        memset(&stack_frame, 0, sizeof(stack_frame));
#if defined(_WIN64)
        int machine_type = IMAGE_FILE_MACHINE_AMD64;
        stack_frame.AddrPC.Offset = exception_pointers->ContextRecord->Rip;
        stack_frame.AddrFrame.Offset = exception_pointers->ContextRecord->Rbp;
        stack_frame.AddrStack.Offset = exception_pointers->ContextRecord->Rsp;
#else
        int machine_type = IMAGE_FILE_MACHINE_I386;
        stack_frame.AddrPC.Offset = exception_pointers->ContextRecord->Eip;
        stack_frame.AddrFrame.Offset = exception_pointers->ContextRecord->Ebp;
        stack_frame.AddrStack.Offset = exception_pointers->ContextRecord->Esp;
#endif
        stack_frame.AddrPC.Mode = AddrModeFlat;
        stack_frame.AddrFrame.Mode = AddrModeFlat;
        stack_frame.AddrStack.Mode = AddrModeFlat;
        while(StackWalk64(machine_type,
            GetCurrentProcess(),
            GetCurrentThread(),
            &stack_frame,
            exception_pointers->ContextRecord,
            NULL,
            &SymFunctionTableAccess64,
            &SymGetModuleBase64,
            NULL) && count_<arraysize(trace_))
        {
            trace_[count_++] = reinterpret_cast<void*>(stack_frame.AddrPC.Offset);
        }
    }

    const void* const* StackTrace::Addresses(size_t* count)
    {
        *count = count_;
        if(count_)
        {
            return trace_;
        }
        return NULL;
    }

    void StackTrace::PrintBacktrace()
    {
        OutputToStream(&std::cerr);
    }

    void StackTrace::OutputToStream(std::ostream* os)
    {
        SymbolContext* context = SymbolContext::Get();
        DWORD error = context->init_error();
        if(error != ERROR_SUCCESS)
        {
            (*os) << "Error initializing symbols (" << error
                << ").  Dumping unresolved backtrace:\n";
            for(int i=0; (i<count_)&&os->good(); ++i)
            {
                (*os) << "\t" << trace_[i] << "\n";
            }
        }
        else
        {
            (*os) << "Backtrace:\n";
            context->OutputTraceToStream(trace_, count_, os);
        }
    }


    bool DebugUtil::suppress_dialogs_ = false;

    // 注意: 没有使用CRT.
    bool DebugUtil::SpawnDebuggerOnProcess(unsigned process_id)
    {
        wchar_t reg_value[1026];
        int len = arraysize(reg_value);
        if(RegReadString(HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug",
            L"Debugger", reg_value, &len))
        {
            wchar_t command_line[1026];
            if(StringReplace(reg_value, process_id, command_line,
                arraysize(command_line)))
            {
                // 调试器存在也没关系, 只是附加到该进程会失败.
                STARTUPINFO startup_info = { 0 };
                startup_info.cb = sizeof(startup_info);
                PROCESS_INFORMATION process_info = { 0 };

                if(CreateProcess(NULL, command_line, NULL, NULL, FALSE, 0, NULL, NULL,
                    &startup_info, &process_info))
                {
                    CloseHandle(process_info.hThread);
                    WaitForInputIdle(process_info.hProcess, 10000);
                    CloseHandle(process_info.hProcess);
                    return true;
                }
            }
        }
        return false;
    }

    bool DebugUtil::WaitForDebugger(int wait_seconds, bool silent)
    {
        for(int i=0; i<wait_seconds*10; ++i)
        {
            if(BeingDebugged())
            {
                if(!silent)
                {
                    BreakDebugger();
                }
                return true;
            }
            ::Sleep(100);
        }
        return false;
    }

    bool DebugUtil::BeingDebugged()
    {
        return ::IsDebuggerPresent() != 0;
    }

    void DebugUtil::BreakDebugger()
    {
        if(suppress_dialogs_)
        {
            _exit(1);
        }

        __debugbreak();
    }

} //namespace base