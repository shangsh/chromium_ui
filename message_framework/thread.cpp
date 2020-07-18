
#include "thread.h"

#include "base/lazy_instance.h"
#include "base/thread_local.h"
#include "base/waitable_event.h"
#include "base/windows_version.h"

namespace base
{

    // ���ڴ�����Ϣѭ���˳�.
    class ThreadQuitTask : public Task
    {
    public:
        virtual void Run()
        {
            MessageLoop::current()->Quit();
            Thread::SetThreadWasQuitProperly(true);
        }
    };

    // ���ڴ������ݸ�ThreadMain, ��StartWithOptions������ջ�Ϸ������.
    struct Thread::StartupData
    {
        // ��Ϊ��ջ�Ϸ���ṹ�����, �����������ֻ�ó�������.
        const Thread::Options& options;

        // ����ͬ���߳�����.
        WaitableEvent event;

        explicit StartupData(const Options& opt)
            : options(opt), event(false, false) {}
    };

    Thread::Thread(const char* name)
        : started_(false),
        stopping_(false),
        startup_data_(NULL),
        thread_(0),
        message_loop_(NULL),
        thread_id_(0),
        name_(name) {}

    Thread::~Thread()
    {
        Stop();
    }

    // ��������߳����ֵ���Ϣ����һƪMSDN�ĵ�:
    //   http://msdn2.microsoft.com/en-us/library/xcb2z8hs.aspx
    const DWORD kVCThreadNameException = 0x406D1388;

    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType;     // ������0x1000.
        LPCSTR szName;    // ָ������(���û���ַ�ռ�).
        DWORD dwThreadID; // �߳�ID(-1=�������߳�).
        DWORD dwFlags;    // �����Ա㽫��ʹ��, ����Ϊ0.
    } THREADNAME_INFO;

    DWORD __stdcall ThreadFunc(void* closure)
    {
        ThreadDelegate* delegate = static_cast<ThreadDelegate*>(closure);
        delegate->ThreadMain();
        return NULL;
    }

    // static
    void Thread::SetName(const char* name)
    {
        // ���쳣ʱ�����߳�������Ҫ�е���������. ���û�е�����, ���׳��쳣ʱҲ��
        // ��Ҫ�߳�����.
        if(!::IsDebuggerPresent())
        {
            return;
        }

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = GetCurrentThreadId();
        info.dwFlags = 0;

        __try
        {
            RaiseException(kVCThreadNameException, 0, sizeof(info)/sizeof(DWORD),
                reinterpret_cast<DWORD_PTR*>(&info));
        }
        __except(EXCEPTION_CONTINUE_EXECUTION)
        {
        }
    }

    // static
    bool Thread::Create(size_t stack_size, ThreadDelegate* delegate,
        HANDLE* thread_handle)
    {
        unsigned int flags = 0;
        if(stack_size>0 && base::GetWinVersion()>=base::WINVERSION_XP)
        {
            flags = STACK_SIZE_PARAM_IS_A_RESERVATION;
        }
        else
        {
            stack_size = 0;
        }

        // ������CreateThread������_beginthreadex, ����ʹ�̴߳�������һЩ, �Ҳ���Ҫ
        // ����������. ���ǵĴ���ֻ����CreateThread()���߳�������, ��Ϊ������Windows
        // ���̳߳���ִ�д���. �����ַ�ʽ�Ĳ���:
        //   http://www.microsoft.com/msj/1099/win32/win321099.aspx
        *thread_handle = CreateThread(NULL, stack_size, ThreadFunc,
            delegate, flags, NULL);
        return *thread_handle != NULL;
    }

    // static
    void Thread::Join(HANDLE thread_handle)
    {
        DCHECK(thread_handle);

        // �ȴ��߳��˳�. �߳�Ӧ�û����, ����Ҫ��֤�����������ȷ��.
        DWORD result = WaitForSingleObject(thread_handle, INFINITE);
        DCHECK_EQ(WAIT_OBJECT_0, result);

        CloseHandle(thread_handle);
    }

    namespace
    {

        // ʹ���ֲ߳̾�������¼һ���߳��Ƿ�ͨ������Stop�����˳���. �������ǿ��Բ�׽��
        // ֱ�ӵ���MessageLoop::Quit()�����, ����Thread���������е�MessageLoop, ��ϣ
        // ��ʹ�������˳���ʽ.
        base::LazyInstance<base::ThreadLocalBoolean> lazy_tls_bool(
            base::LINKER_INITIALIZED);

    }

    void Thread::SetThreadWasQuitProperly(bool flag)
    {
        lazy_tls_bool.Pointer()->Set(flag);
    }

    bool Thread::GetThreadWasQuitProperly()
    {
        bool quit_properly = true;
#ifndef NDEBUG
        quit_properly = lazy_tls_bool.Pointer()->Get();
#endif
        return quit_properly;
    }

    bool Thread::Start()
    {
        return StartWithOptions(Options());
    }

    bool Thread::StartWithOptions(const Options& options)
    {
        DCHECK(!message_loop_);

        SetThreadWasQuitProperly(false);

        StartupData startup_data(options);
        startup_data_ = &startup_data;

        if(!Create(options.stack_size, this, &thread_))
        {
            DLOG(ERROR) << "failed to create thread";
            startup_data_ = NULL;
            return false;
        }

        // �ȴ��߳���������ʼ��message_loop_.
        startup_data.event.Wait();

        // ���ó�NULL, �������ǲ��ᱣ��һ��ջ�϶����ָ��.
        startup_data_ = NULL;
        started_ = true;

        DCHECK(message_loop_);
        return true;
    }

    void Thread::Stop()
    {
        if(!thread_was_started())
        {
            return;
        }

        StopSoon();

        // �ȴ��߳��˳�.
        //
        // TODO: �ܲ���, ������Ҫ����message_loop_����ֱ���߳��˳�. ��Щ�û�����
        // API. ʹ����ֹͣ.
        Join(thread_);

        // �߳��˳�ʱ���message_loop_���ó�NULL.
        DCHECK(!message_loop_);

        // �̲߳���Ҫ�ٴν���.
        started_ = false;

        stopping_ = false;
    }

    void Thread::StopSoon()
    {
        // ֻ���������߳��е���(�������̵߳��߳�!=���߳�).
        DCHECK_NE(thread_id_, GetCurrentThreadId());

        if(stopping_ || !message_loop_)
        {
            return;
        }

        stopping_ = true;
        message_loop_->PostTask(new ThreadQuitTask());
    }

    void Thread::Run(MessageLoop* message_loop)
    {
        message_loop->Run();
    }

    void Thread::ThreadMain()
    {
        {
            // ���̵߳���Ϣѭ��.
            MessageLoop message_loop(startup_data_->options.message_loop_type);

            // ����̶߳���ĳ�ʼ��.
            thread_id_ = GetCurrentThreadId();
            SetName(name_.c_str());
            message_loop.set_thread_name(name_);
            message_loop_ = &message_loop;
            message_loop_proxy_ = MessageLoopProxy::CreateForCurrentThread();

            // �����߳�������ĳ�ʼ������, ��֪ͨ�����߳�ǰ����.
            Init();

            startup_data_->event.Signal();
            // ������ʹ��startup_data_, ��Ϊ�����̴߳�ʱ�Ѿ�����.

            Run(message_loop_);

            // �����߳��������������.
            CleanUp();

            // ����MessageLoop::Quit�Ǳ�ThreadQuitTask���õ�.
            DCHECK(GetThreadWasQuitProperly());

            // ���ٽ����κ���Ϣ.
            message_loop_ = NULL;
            message_loop_proxy_ = NULL;
        }
        CleanUpAfterMessageLoopDestruction();
        thread_id_ = 0;
    }

} //namespace base