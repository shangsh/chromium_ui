
#include "debug_util.h"

#pragma comment(lib, "dbghelp.lib")

#include <windows.h>
#include <dbghelp.h>

#include <iostream>

#include "basic_types.h"
#include "lock.h"
#include "logging.h"
#include "singleton.h"

// ��С�Ķ�ע�����.
// ע��: ûʹ��CRT.
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
    // ��֧��REG_EXPAND_SZ
    res = RegQueryValueEx(key, value_name, NULL, &type,
        reinterpret_cast<BYTE*>(buffer), &buffer_size);
    if(ERROR_SUCCESS==res && buffer_size!=0 && type==REG_SZ)
    {
        // ȷ��buffer��NULL������.
        buffer[*len-1] = 0;
        *len = lstrlenW(buffer);
        RegCloseKey(key);
        return true;
    }
    RegCloseKey(key);
    return false;
}

// �滻input�е�ÿ��"%ld", ����Ч������������.
// ע��: ûʹ��CRT.
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
            // ȷ�����㹻��ʣ��ռ�.
            if((current_output_len+12) >= output_len)
            {
                return false;
            }

            // ���۵�_itow().
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

// SymbolContext��һ����װ��DbgHelp��Sym*ϵ�к������̰߳�ȫ�ĵ���.
// Sym*ϵ�к���ͬһʱ��ֻ�ܱ�һ���̵߳���. SymbolContext������ܻ�ͨ������
// ����symbol������, ���ʱ�����ᵼ���ӳٴӶ�Ӱ������.
//
// ����һ����֪���������: ��ʹ��Sym*ϵ�к���ʱ, ������һ���߳��е���
// breakpad, backtrace�Ĵ�����breakpad�����г�ͻ. �μ�bug:
//   http://code.google.com/p/google-breakpad/issues/detail?id=311
// �����Ǽ������, ��ǰ���迼��.
class SymbolContext
{
public:
    static SymbolContext* Get()
    {
        // ����leaky������Ϊ��������ڽ��̽���ʱ����.
        return base::Singleton<SymbolContext,
            base::LeakySingletonTraits<SymbolContext> >::get();
    }

    // ���س�ʼ��ʧ�ܵĴ�����.
    DWORD init_error() const
    {
        return init_error_;
    }

    // ����ָ����trace, ���Խ������Ų������os����. ���ݵ�ÿ�и�ʽ����:
    //     <tab>SymbolName[0xAddress+Offset] (FileName:LineNo)
    // ������Init()֮�����. �����ﲻҪ����LOG(FATAL), ��Ϊ���ܾ���LOG(FATAL)
    // ���ù�����, ������������ѭ��.
    void OutputTraceToStream(const void* const* trace, int count,
        std::ostream* os)
    {
        base::AutoLock lock(lock_);

        for(size_t i=0; (i<static_cast<size_t>(count))&&os->good(); ++i)
        {
            const int kMaxNameLength = 256;
            DWORD_PTR frame = reinterpret_cast<DWORD_PTR>(trace[i]);

            // ������MSDN���Ӹ�д��:
            // http://msdn.microsoft.com/en-us/library/ms680578(VS.85).aspx
            ULONG64 buffer[(sizeof(SYMBOL_INFO)+
                kMaxNameLength*sizeof(wchar_t)+sizeof(ULONG64)-1)/sizeof(ULONG64)];
            memset(buffer, 0, sizeof(buffer));

            // ��ʼ��������Ϣ�ṹ��.
            DWORD64 sym_displacement = 0;
            PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(&buffer[0]);
            symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            symbol->MaxNameLen = kMaxNameLength - 1;
            BOOL has_symbol = SymFromAddr(GetCurrentProcess(), frame,
                &sym_displacement, symbol);

            // ���Է�������Ϣ.
            DWORD line_displacement = 0;
            IMAGEHLP_LINE64 line = { 0 };
            line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            BOOL has_line = SymGetLineFromAddr64(GetCurrentProcess(), frame,
                &line_displacement, &line);

            // �����������Ϣ.
            (*os) << "\t";
            if(has_symbol)
            {
                (*os) << symbol->Name << " [0x" << trace[i] << "+"
                    << sym_displacement << "]";
            }
            else
            {
                // û�з�����Ϣ.
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
        // ��ʼ�����̵ķ��ű�.
        // ��Ҫʱ�ӳټ��ط���, ���з���ʹ��δ���η�ʽ, �����к���Ϣ.
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

    // ע��: ֱ�Ӽ��غ�����ַ���ⰲװ���µ�SDK
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

        // ʹ��CaptureStackBackTrace()��õ��ö�ջ��Ϣ.
        count_ = fnCaptureStackBackTrace(0, arraysize(trace_), trace_, NULL);
    }

    StackTrace::StackTrace(_EXCEPTION_POINTERS* exception_pointers)
    {
        // ʹ��StackWalk64()����쳣��ջ��Ϣ.
        count_ = 0;
        // ��ʼ����ջ����г�.
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

    // ע��: û��ʹ��CRT.
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
                // ����������Ҳû��ϵ, ֻ�Ǹ��ӵ��ý��̻�ʧ��.
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