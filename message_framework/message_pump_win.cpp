
#include "message_pump_win.h"

#include <math.h>

namespace base
{

    static const wchar_t kWndClass[] = L"Wan_MessagePumpWindow";

    // ����kMsgHaveWork��Ϣ, ����Ϣ�û�ȡʱ��Ƭ, ������һ������(�������ϵķ����γ�
    // �����������).
    static const int kMsgHaveWork = WM_USER + 1;

    void MessagePumpWin::AddObserver(Observer* observer)
    {
        observers_.AddObserver(observer);
    }

    void MessagePumpWin::RemoveObserver(Observer* observer)
    {
        observers_.RemoveObserver(observer);
    }

    void MessagePumpWin::WillProcessMessage(const MSG& msg)
    {
        FOR_EACH_OBSERVER(Observer, observers_, WillProcessMessage(msg));
    }

    void MessagePumpWin::DidProcessMessage(const MSG& msg)
    {
        FOR_EACH_OBSERVER(Observer, observers_, DidProcessMessage(msg));
    }

    void MessagePumpWin::RunWithDispatcher(Delegate* delegate,
        Dispatcher* dispatcher)
    {
        RunState s;
        s.delegate = delegate;
        s.dispatcher = dispatcher;
        s.should_quit = false;
        s.run_depth = state_ ? state_->run_depth+1 : 1;

        RunState* previous_state = state_;
        state_ = &s;

        DoRunLoop();

        state_ = previous_state;
    }

    void MessagePumpWin::Quit()
    {
        DCHECK(state_);
        state_->should_quit = true;
    }

    int MessagePumpWin::GetCurrentDelay() const
    {
        if(delayed_work_time_.is_null())
        {
            return -1;
        }

        // ������ҪС��. TimeDelta������΢��, ������ֻ��Ҫ����ֵ. ���ʣ��5.5ms, Ӧ��
        // �ӳ�5ms����6ms��? Ϊ��������ִ���ӳ�����, Ӧ����6ms.
        double timeout = ceil((delayed_work_time_ - Time::Now()).InMillisecondsF());

        // ����Ǹ���, Ӧ������ִ���ӳ�����.
        int delay = static_cast<int>(timeout);
        if(delay < 0)
        {
            delay = 0;
        }

        return delay;
    }

    MessagePumpForUI::MessagePumpForUI()
    {
        InitMessageWnd();
    }

    MessagePumpForUI::~MessagePumpForUI()
    {
        DestroyWindow(message_hwnd_);
        UnregisterClass(kWndClass, GetModuleHandle(NULL));
    }

    void MessagePumpForUI::ScheduleWork()
    {
        if(InterlockedExchange(&have_work_, 1))
        {
            return; // �𴦵���Ϣ��������.
        }

        // ȷ��MessagePump��ִ������.
        PostMessage(message_hwnd_, kMsgHaveWork, reinterpret_cast<WPARAM>(this), 0);
    }

    void MessagePumpForUI::ScheduleDelayedWork(const Time& delayed_work_time)
    {
        // �����ṩ�߾��ȵ�ʱ��. Windows��ʱ��ʹ��SetTimer(), ����Ϊ10ms. ���Ǳ���
        // ʹ��WM_TIMER��Ϊ���ѻ���, ��Ϊ������ܻ����ģ̬ѭ������ִ��MessageLoop;
        // ����ʱ�ӵ�Ψһ��������ͨ����ϢͶ��.
        //
        // Ϊ���ṩ���ȸ���10ms��ʱ��, ��Ҫ���Լ���ִ��ѭ���н���ʱ�Ӵ���. һ������,
        // ִ��ѭ���Ĺ��������лᴦ��ʱ��. ������Ҫ������һ��ϵͳʱ��, �ֲ�ģ̬��Ϣѭ
        // �����޷�ִ�б�ѭ�������. �п��ܴ���SetTimerʱû�еȴ�ʱ������, ��Ϊִ��
        // ѭ�������Ѿ���������.
        //
        // ʹ��SetTimer����һ�����촥����ʱ��. �����µ�ʱ�Ӵ�������, �����SetTimer.
        // ����һ���ٵ�SetTimer�¼����޺���, ��Ϊ����ֻ�Ǵ���һ���յ�ʱ�Ӷ���.
        delayed_work_time_ = delayed_work_time;

        int delay_msec = GetCurrentDelay();
        DCHECK(delay_msec >= 0);
        if(delay_msec < USER_TIMER_MINIMUM)
        {
            delay_msec = USER_TIMER_MINIMUM;
        }

        // ����WM_TIMER�¼��ỽ����Ϣ�ü��ȴ���ʱ������(��Ƕ��ִ�е�ʱ��ͨ����ѭ��).
        SetTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this), delay_msec, NULL);
    }

    void MessagePumpForUI::PumpOutPendingPaintMessages()
    {
        // ��Run����֮�����, �����κδ���.
        if(!state_)
        {
            return;
        }

        // ����һ��С����Ϣ��ǿ�ƾ��촦��WM_PAINT��Ϣ. ��Ȼ���ṩ����ѭ��, �����㹻
        // ������������. һ�����4�ξͿ���, ������20�θ���ȫһЩ.
        const int kMaxPeekCount = 20;
        int peek_count;
        for(peek_count=0; peek_count<kMaxPeekCount; ++peek_count)
        {
            MSG msg;
            if(!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE|PM_QS_PAINT))
            {
                break;
            }
            ProcessMessageHelper(msg);
            if(state_->should_quit) // ����WM_QUIT.
            {
                break;
            }
        }
    }

    // static
    LRESULT CALLBACK MessagePumpForUI::WndProcThunk(HWND hwnd, UINT message,
        WPARAM wparam, LPARAM lparam)
    {
        switch(message)
        {
        case kMsgHaveWork:
            reinterpret_cast<MessagePumpForUI*>(wparam)->HandleWorkMessage();
            break;
        case WM_TIMER:
            reinterpret_cast<MessagePumpForUI*>(wparam)->HandleTimerMessage();
            break;
        }
        return DefWindowProc(hwnd, message, wparam, lparam);
    }

    void MessagePumpForUI::DoRunLoop()
    {
        // ���ֻ�Ǽ򵥵�PeekMessage()ѭ��, ����MSDN����PeekMessage���ĵ�, Windows
        // ���ᰴ�������˳������Ϣ���������е�����, �������κι���:
        //     * Sent messages
        //     * Posted messages
        //     * Sent messages (again)
        //     * WM_PAINT messages
        //     * WM_TIMER messages
        //
        // �ܽ�: �������е���Ϣ����õ�ִ��, Sent����Ϣ��2�δ������(�ɼ��ٷ���ʱ��).

        for(;;)
        {
            // �ڴ��������ʱ��, ���ܻᴴ��һЩ��Ϣ, �����ȴ������оͻ��и���������
            // ����. ����ProcessNextWindowsMessage()ʱ, ���ܻ��и������Ϣ�ڵȴ�. ����
            // ��˵, ĳһ��û�д�������, �ٴε��ÿ���Ҳ�����������ڵȴ�. ���, ���û��
            // �κοɴ��������, ��ʱ���Կ������ߵȴ�����.
            bool more_work_is_plausible = ProcessNextWindowsMessage();
            if(state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |= state_->delegate->DoWork();
            if(state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |= state_->delegate->DoDelayedWork(
                &delayed_work_time_);
            // �����û��������ӳ�����, ��ô������ҪWM_TIMER�Ĵ��ڴ���ִ���ӳ�����.
            // ���ʱ����������, ���ǲ�Ҫȥֹͣ��. ������������ӳ����񶼱�������,
            // ��ҪֹͣWM_TIMER.
            if(more_work_is_plausible && delayed_work_time_.is_null())
            {
                KillTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this));
            }
            if(state_->should_quit)
            {
                break;
            }

            if(more_work_is_plausible)
            {
                continue;
            }

            more_work_is_plausible = state_->delegate->DoIdleWork();
            if(state_->should_quit)
            {
                break;
            }

            if(more_work_is_plausible)
            {
                continue;
            }

            WaitForWork(); // �ȴ�(����)ֱ����������.
        }
    }

    void MessagePumpForUI::InitMessageWnd()
    {
        HINSTANCE hinst = GetModuleHandle(NULL);

        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WndProcThunk;
        wc.hInstance = hinst;
        wc.lpszClassName = kWndClass;
        RegisterClassEx(&wc);

        message_hwnd_ = CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0,
            HWND_MESSAGE, 0, hinst, 0);
        DCHECK(message_hwnd_);
    }

    void MessagePumpForUI::WaitForWork()
    {
        // �ȴ�ֱ������Ϣ����, ������һ��ʱ�ӵ�ʱ��ȡ����ʱ�ӹ���.
        int delay = GetCurrentDelay();
        if(delay < 0) // ������ʾû�еȴ�ʱ��.
        {
            delay = INFINITE;
        }

        DWORD result;
        result = MsgWaitForMultipleObjectsEx(0, NULL, delay,
            QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        if(WAIT_OBJECT_0 == result)
        {
            // WM_*��Ϣ����.
            // ������Ӵ��ڷֱ��������߳�, ���ǵ����붼�ᾭ����Ϣ����.
            // ����MsgWaitForMultipleObjectsEx���ص�ԭ������Ϣ�Ѿ�׼����, �ȴ�����
            // (��ָ�Ӵ��ڵ������Ϣ, �Ӵ��ڵ�ǰ������).
            // ��������PeekMessages������Ϣʧ�ܻᵼ����ѭ��.
            // ����WaitMessage���Ӵ���һЩʱ�䴦��������Ϣ.
            // ע: �ҷ���Chrome�Ĵ�����PeekMessageû�д���NC�����Ϣ, WM_NCLBUTTONDOWN
            // ���ӳٷ�������ϵͳ��ť����Ϊ������, ��˼�����NC�����Ϣ�ļ��.
#ifndef WM_NCMOUSEFIRST
#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#endif
#ifndef WM_NCMOUSELAST
#define WM_NCMOUSELAST  WM_NCXBUTTONDBLCLK
#endif
            MSG msg = { 0 };
            DWORD queue_status = GetQueueStatus(QS_MOUSE);
            if(HIWORD(queue_status)&QS_MOUSE && !PeekMessage(&msg, NULL,
                WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) && !PeekMessage(&msg,
                NULL, WM_NCMOUSEFIRST, WM_NCMOUSELAST, PM_NOREMOVE))
            {
                WaitMessage();
            }
            return;
        }

        DCHECK_NE(WAIT_FAILED, result) << GetLastError();
    }

    void MessagePumpForUI::HandleWorkMessage()
    {
        // �����Run����֮�����, �����κδ���. ��Ӧ��MessageBox���û��������������.
        if(!state_)
        {
            // ��Ϊ������kMsgHaveWork��Ϣ, ������Ҫ���±�־λ.
            InterlockedExchange(&have_work_, 0);
            return;
        }

        // ��ֹ���õ���ϢӰ��Windows��Ϣ��������������Ϣ�Ĵ���.
        ProcessPumpReplacementMessage();

        // �ô���õ�ִ�л���, ͨ������ֵ����֪���Ƿ���Ҫ������������.
        if(state_->delegate->DoWork())
        {
            ScheduleWork();
        }
    }

    void MessagePumpForUI::HandleTimerMessage()
    {
        KillTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this));

        // �����Run����֮�����, �����κδ���. ��Ӧ��MessageBox���û��������������.
        if(!state_)
        {
            return;
        }

        state_->delegate->DoDelayedWork(&delayed_work_time_);
        if(!delayed_work_time_.is_null())
        {
            // �ٴ�����delayed_work_time_.
            ScheduleDelayedWork(delayed_work_time_);
        }
    }

    bool MessagePumpForUI::ProcessNextWindowsMessage()
    {
        // �����Ϣ��������Sent��Ϣ, PeekMessage�ڲ���ַ���Ϣ������false.
        // Ϊȷ���ٴβ쿴��Ϣѭ�������ǵ���MsgWaitForMultipleObjectsEx, ������
        // ��true.
        bool sent_messages_in_queue = false;
        DWORD queue_status = GetQueueStatus(QS_SENDMESSAGE);
        if(HIWORD(queue_status) & QS_SENDMESSAGE)
        {
            sent_messages_in_queue = true;
        }

        MSG msg;
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            return ProcessMessageHelper(msg);
        }

        return sent_messages_in_queue;
    }

    bool MessagePumpForUI::ProcessMessageHelper(const MSG& msg)
    {
        if(WM_QUIT == msg.message)
        {
            // �ٴη���QUIT��Ϣ, �Ա���ѭ��GetMessage()�ܹ����յ�.
            state_->should_quit = true;
            PostQuitMessage(static_cast<int>(msg.wParam));
            return false;
        }

        // ������Ϣ����ִ��ʱ, ����kMsgHaveWork��Ϣ.
        if(msg.message==kMsgHaveWork && msg.hwnd==message_hwnd_)
        {
            return ProcessPumpReplacementMessage();
        }

        if(CallMsgFilter(const_cast<MSG*>(&msg), kMessageFilterCode))
        {
            return true;
        }

        WillProcessMessage(msg);

        if(state_->dispatcher)
        {
            if(!state_->dispatcher->Dispatch(msg))
            {
                state_->should_quit = true;
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DidProcessMessage(msg);
        return true;
    }

    bool MessagePumpForUI::ProcessPumpReplacementMessage()
    {
        // ����kMsgHaveWork��Ϣ��ʱ����ñ�����. �쿴��������滻����Ϣ, ����WM_PAINT
        // ��WM_TIMER. Ŀ����Ϊ��ʹkMsgHaveWork����������Windows�������Ϣ����, ��ʹ
        // ������Ϣ�����ϵķ���. ����С�ĵػ�ȡһ����Ϣ, ����Ϣ��������kMsgHaveWork,
        // ����have_work_��־λ(�����ٴη���kMsgHaveWork��Ϣ), ���ַ�ȡ������Ϣ.
        // ע�������߳��ٴη���kMsgHaveWork���첽��!
        MSG msg;
        bool have_message = (0 != PeekMessage(&msg, NULL, 0, 0, PM_REMOVE));
        DCHECK(!have_message || kMsgHaveWork!=msg.message ||
            msg.hwnd!=message_hwnd_);

        // ��Ϊ������kMsgHaveWork��Ϣ, ������±�־λ.
        int old_have_work = InterlockedExchange(&have_work_, 0);
        DCHECK(old_have_work);

        // ���û����Ϣ��Ҫ����, ������Ҫִ��ʱ��Ƭ.
        if(!have_message)
        {
            return false;
        }

        // ��֤���뱾����Ϣ������ܵõ�ʱ��Ƭ. ����ǳ��ٵ�ʱ��, ScheduleWork()��һ��
        // Ӱ������, ���ǵ��¼����з�æ��ʱ��, kMsgHaveWork����Խ��Խ��(���).
        ScheduleWork();
        return ProcessMessageHelper(msg);
    }

    MessagePumpForIO::MessagePumpForIO()
    {
        port_.Set(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1));
        DCHECK(port_.IsValid());
    }

    void MessagePumpForIO::ScheduleWork()
    {
        if(InterlockedExchange(&have_work_, 1))
        {
            return; // �𴦵���Ϣ��������.
        }

        // ȷ��MessagePump��ִ������.
        BOOL ret = PostQueuedCompletionStatus(port_, 0,
            reinterpret_cast<ULONG_PTR>(this),
            reinterpret_cast<OVERLAPPED*>(this));
        DCHECK(ret);
    }

    void MessagePumpForIO::ScheduleDelayedWork(const Time& delayed_work_time)
    {
        // ���ڷ���ֻ����Run��ͬһ�߳��б�����, �������ᱻ����. ����ֻ��Ҫ��������
        // ��ʱ��.
        delayed_work_time_ = delayed_work_time;
    }

    void MessagePumpForIO::RegisterIOHandler(HANDLE file_handle,
        IOHandler* handler)
    {
        ULONG_PTR key = reinterpret_cast<ULONG_PTR>(handler);
        HANDLE port = CreateIoCompletionPort(file_handle, port_, key, 1);
        DCHECK(port == port_.Get());
    }

    void MessagePumpForIO::DoRunLoop()
    {
        for(;;)
        {
            // �ڴ��������ʱ��, ���ܻᴴ��һЩ��Ϣ, �����ȴ������оͻ��и���������
            // ����. ���統WaitForIOCompletion()ʱ, ���ܻ��и������Ϣ�ڵȴ�. ���仰
            // ˵, ĳһ��û�д�������, �ٴε��ÿ���Ҳ�����������ڵȴ�. ���, ���û��
            // �κοɴ��������, ��ʱ���Կ������ߵȴ�����.
            bool more_work_is_plausible = state_->delegate->DoWork();
            if(state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |= WaitForIOCompletion(0, NULL);
            if(state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |=
                state_->delegate->DoDelayedWork(&delayed_work_time_);
            if(state_->should_quit)
            {
                break;
            }

            if(more_work_is_plausible)
            {
                continue;
            }

            more_work_is_plausible = state_->delegate->DoIdleWork();
            if(state_->should_quit)
            {
                break;
            }

            if(more_work_is_plausible)
            {
                continue;
            }

            WaitForWork(); // �ȴ�(����)ֱ����������.
        }
    }

    // �ȴ�ֱ������Ϣ����, ������һ��ʱ�ӵ�ʱ��ȡ����ʱ�ӹ���.
    void MessagePumpForIO::WaitForWork()
    {
        // IO��Ϣѭ����֧��Ƕ��. ����ݹ�������鷳.
        DCHECK(state_->run_depth == 1) << "Cannot nest an IO message loop!";

        int timeout = GetCurrentDelay();
        if(timeout < 0) // ������ʾû�еȴ�ʱ��.
        {
            timeout = INFINITE;
        }

        WaitForIOCompletion(timeout, NULL);
    }

    bool MessagePumpForIO::WaitForIOCompletion(DWORD timeout, IOHandler* filter)
    {
        IOItem item;
        if(completed_io_.empty() || !MatchCompletedIOItem(filter, &item))
        {
            // ��ϵͳ������һ��IO���.
            if(!GetIOItem(timeout, &item))
            {
                return false;
            }

            if(ProcessInternalIOItem(item))
            {
                return true;
            }
        }

        if(item.context->handler)
        {
            if(filter && item.handler!=filter)
            {
                // ���浽����.
                completed_io_.push_back(item);
            }
            else
            {
                DCHECK_EQ(item.context->handler, item.handler);
                WillProcessIOEvent();
                item.handler->OnIOCompleted(item.context,
                    item.bytes_transfered, item.error);
                DidProcessIOEvent();
            }
        }
        else
        {
            // ��ʱ��Ҫ���handler.
            delete item.context;
        }
        return true;
    }

    // ��OS�ٴ�����һ��IO��ɽ��.
    bool MessagePumpForIO::GetIOItem(DWORD timeout, IOItem* item)
    {
        memset(item, 0, sizeof(*item));
        ULONG_PTR key = NULL;
        OVERLAPPED* overlapped = NULL;
        if(!GetQueuedCompletionStatus(port_.Get(), &item->bytes_transfered,
            &key, &overlapped, timeout))
        {
            if(!overlapped)
            {
                return false; // ������ʲô��û��.
            }
            item->error = GetLastError();
            item->bytes_transfered = 0;
        }

        item->handler = reinterpret_cast<IOHandler*>(key);
        item->context = reinterpret_cast<IOContext*>(overlapped);
        return true;
    }

    bool MessagePumpForIO::ProcessInternalIOItem(const IOItem& item)
    {
        if(this==reinterpret_cast<MessagePumpForIO*>(item.context) &&
            this==reinterpret_cast<MessagePumpForIO*>(item.handler))
        {
            // �ڲ�������¼�.
            DCHECK(!item.bytes_transfered);
            InterlockedExchange(&have_work_, 0);
            return true;
        }
        return false;
    }

    // ����һ�����յ�������¼�.
    bool MessagePumpForIO::MatchCompletedIOItem(IOHandler* filter, IOItem* item)
    {
        DCHECK(!completed_io_.empty());
        for(std::list<IOItem>::iterator it=completed_io_.begin();
            it!=completed_io_.end(); ++it)
        {
            if(!filter || it->handler==filter)
            {
                *item = *it;
                completed_io_.erase(it);
                return true;
            }
        }
        return false;
    }

    void MessagePumpForIO::AddIOObserver(IOObserver* obs)
    {
        io_observers_.AddObserver(obs);
    }

    void MessagePumpForIO::RemoveIOObserver(IOObserver* obs)
    {
        io_observers_.RemoveObserver(obs);
    }

    void MessagePumpForIO::WillProcessIOEvent()
    {
        FOR_EACH_OBSERVER(IOObserver, io_observers_, WillProcessIOEvent());
    }

    void MessagePumpForIO::DidProcessIOEvent()
    {
        FOR_EACH_OBSERVER(IOObserver, io_observers_, DidProcessIOEvent());
    }

} //namespace base