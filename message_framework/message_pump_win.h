
#ifndef __message_pump_win_h__
#define __message_pump_win_h__

#pragma once

#include <windows.h>

#include <list>

#include "base/scoped_handle.h"
#include "base/time.h"

#include "message_pump.h"
#include "observer_list.h"

namespace base
{

    // MessagePumpWin��MessagePump��Windows��ʵ�ֵĻ���. �ṩ��һЩ��������, ��۲���
    // �Ĵ����Լ���Ϣ���������ڵĿ���.
    class MessagePumpWin : public MessagePump
    {
    public:
        // Observer�����������UI��Ϣѭ����ȫ��֪ͨ��Ϣ.
        //
        // ע��: �۲��ߵ�ʵ��Ӧ���Ƿǳ����!
        class Observer
        {
        public:
            virtual ~Observer() {}

            // ����UI��Ϣǰ����, ������δ������Ϣ(msg.messageΪ0).
            virtual void WillProcessMessage(const MSG& msg) = 0;

            // ����UI��Ϣ�����, ������δ������Ϣ(msg.messageΪ0).
            virtual void DidProcessMessage(const MSG& msg) = 0;
        };

        // Ƕ�׵��õ�Runʹ��Dispatcher���ɷ��¼�. �������Runʱ���ݵ�Dispatcher��
        // Ϊ��, MessageLoop�����ɷ��¼�(����TranslateMessage), ���е���Ϣ���ݸ�
        // Dispatcher��Dispatch����. �¼��Ƿ��ɷ�ȡ����Dispatcher.
        //
        // Ƕ�׵�ѭ��ͨ��postһ��quit��Ϣ����Dispatch����false�˳�.
        class Dispatcher
        {
        public:
            virtual ~Dispatcher() {}
            // �ɷ��¼�. ����true��ʾ����, ����falseʱǶ�׵�ѭ�������˳�.
            virtual bool Dispatch(const MSG& msg) = 0;
        };

        MessagePumpWin() : have_work_(0), state_(NULL) {}
        virtual ~MessagePumpWin() {}

        // ���һ��Observer, ����������֪ͨ��Ϣ.
        void AddObserver(Observer* observer);

        // �Ƴ�һ��Observer, �÷����ڵ�ǰObserver�ӵ�֪ͨ�ص���ʱ�����Ҳ�ǰ�ȫ��.
        void RemoveObserver(Observer* observer);

        // ֪ͨ��Ϣѭ���Ĺ۲��߽�Ҫ����һ����Ϣ.
        void WillProcessMessage(const MSG& msg);
        void DidProcessMessage(const MSG& msg);

        // ��MessagePump::Run����, ���dispatcher��Ϊ����MSG����ͨ����·��.
        void RunWithDispatcher(Delegate* delegate, Dispatcher* dispatcher);

        virtual void Run(Delegate* delegate) { RunWithDispatcher(delegate, NULL); }
        virtual void Quit();

    protected:
        struct RunState
        {
            Delegate* delegate;
            Dispatcher* dispatcher;

            // Run()���þ��췵�صı�־λ.
            bool should_quit;

            // Run()Ƕ�׵��ö�ջ�����.
            int run_depth;
        };

        virtual void DoRunLoop() = 0;
        int GetCurrentDelay() const;

        ObserverList<Observer> observers_;

        // �ӳ�����ִ�е�ʱ���.
        Time delayed_work_time_;

        // ���kMsgDoWork�Ƿ��Ѿ���Windows��Ϣ������. �����Ϣ���ֻ��һ��, �ڱ���
        // ��Ϣ�����е�ͬʱ���������ִ��.
        LONG have_work_;

        // ��ǰRun���õ�״̬.
        RunState* state_;
    };

    //-----------------------------------------------------------------------------
    // MessagePumpForUI������MessagePumpWin, �����TYPE_UI��ʼ����MessageLoop����.
    //
    // MessagePumpForUIʵ�ִ�ͳ��Windows��Ϣ��. ͨ��һ������ѭ��, ���ϵ�ȡ��Ϣ����
    // ��. ȡ��Ϣ��ͬʱ����DoWork����ȴ�������, ����DoDelayedWork����ȴ���ʱ��.
    // û���¼������ʱ��, ��Ϣ�ý���ȴ�״̬. �󲿷������, ���е��¼���������
    // MessagePumpForUI����.
    //
    // ����, ��һ����������¼���ջ�ϴ�����һ�����صĶԻ���, �������ֻ���ṩһ��
    // ���ص���Ϣ��. �����Ϣ�ý����Ǵ�Windows��Ϣ����ȡ��Ϣ, ��������Ϣ���ɷ�.
    // MessageLoop�Ա��ص���Ϣ�ý�������չ��֧���������, ֻ����һЩ����.
    //
    // ����Ϣ�õ���չʵ�ֵĻ����ṹ��һ���������ϢkMsgHaveWork, �������ϵ�ע�뵽
    // Windows��Ϣ����. ÿ�μ���kMsgHaveWork��Ϣ, �����һϵ���¼��ļ��, �����Ƿ�
    // �п����е�����.
    //
    // һ������ִ����ɺ�, kMsgHaveWork��Ϣ�ٴα�Ͷ�ݵ�Windows��Ϣ����, ȷ�������
    // �¼����ܻ��ʱ��Ƭ. Ϊ��ֹkMsgHaveWork̫�ർ��Windows��Ϣ�������, ��Ҫ��֤
    // Window����Ϣ�������ֻ����һ��kMsgHaveWork��Ϣ.
    //
    // ��û���������е�ʱ��, ϵͳ�ĸ������������ʹ������Ϣ�õ�������Ϣ��ֹͣ. ��Ϣ
    // ���ڶ����������ʱ����Զ���.
    //
    // �ڶ��������������������ϵ�Ͷ���������ֹWindows��Ϣ�ô���WM_PAINT����
    // WM_TIMER. ���ֻ�ͼ��ʱ����Ϣ������kMsgHaveWork��Ϣ���ȼ���. ����, ��ÿ��Ͷ��
    // kMsgHaveWork��Ϣ֮��(�����ڻ�ȡkMsgHaveWork��Ϣ֮����ٴ�Ͷ��֮ǰ)����һЩ��
    // ����Ϣ.
    //
    // ע��: ����ʹ����Ϣ����������ֹͣ����(���źŶ����෴)�����������, ���Ǳ���
    // ��Ϣ��ֻ����Ӧ��Ϣ, �����ͺ������. ����, ����һ���������ѡ��. ��Ҳ������
    // �������ڶ�����Ͷ�ݵ�������Ϣ����DoRunLoop.
    class MessagePumpForUI : public MessagePumpWin
    {
    public:
        // Ӧ�ó�����Ĵ��ݸ����ӹ��̵Ĵ���.
        static const int kMessageFilterCode = 0x5001;

        MessagePumpForUI();
        virtual ~MessagePumpForUI();

        virtual void ScheduleWork();
        virtual void ScheduleDelayedWork(const Time& delayed_work_time);

        // ������ô������еȴ���WM_PAINT��Ϣ. ���������ᴦ��Windows��Ϣ���������е�
        // ��ͼ��Ϣ, ����ȡ����һ���̶�����Ŀ(��������ѭ��).
        void PumpOutPendingPaintMessages();

    private:
        static LRESULT CALLBACK WndProcThunk(HWND hwnd, UINT message,
            WPARAM wparam, LPARAM lparam);
        virtual void DoRunLoop();
        void InitMessageWnd();
        void WaitForWork();
        void HandleWorkMessage();
        void HandleTimerMessage();
        bool ProcessNextWindowsMessage();
        bool ProcessMessageHelper(const MSG& msg);
        bool ProcessPumpReplacementMessage();

        // ������Ϣ�����ش���.
        HWND message_hwnd_;
    };

    //-----------------------------------------------------------------------------
    // MessagePumpForIO������MessagePumpWin, �����TYPE_IO��ʼ����MessageLoop����.
    // ����汾��MessagePump������Windows��Ϣ, ����ִ��ѭ���ǻ�����ɶ˿ڵ�, ����
    // �Ƚ��ʺ�IO����.
    class MessagePumpForIO : public MessagePumpWin
    {
    public:
        struct IOContext;

        // ������Ҫ����OS�첽IO�������֪ͨ, �����ʵ��IOHandler�ӿ�, �����Լ�ע�ᵽ
        // ��Ϣ��.
        //
        // �÷�1:
        //     // ʵ��IO��ǣ�����û�������ʱʹ��, ����������Ϣ���Զ����.
        //     class MyFile : public IOHandler {
        //       MyFile() {
        //         ...
        //         context_ = new IOContext;
        //         context_->handler = this;
        //         message_pump->RegisterIOHandler(file_, this);
        //       }
        //       ~MyFile() {
        //         if(pending_) {
        //           // ����handlerΪNULL, �ڽӵ�֪ͨʱ����ɾ��context, ���ٻص�.
        //           context_->handler = NULL;
        //         } else {
        //           delete context_;
        //         }
        //       }
        //       virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered,
        //         DWORD error) {
        //           pending_ = false;
        //       }
        //       void DoSomeIo() {
        //         ...
        //         // ������Ҫ��Ψһ��������overlapped�ṹ��.
        //         ConnectNamedPipe(file_, &context_->overlapped);
        //         pending_ = true;
        //       }
        //       bool pending_;
        //       IOContext* context_;
        //       HANDLE file_;
        //     };
        //
        // �÷�2:
        //     class MyFile : public IOHandler {
        //       MyFile() {
        //         ...
        //         message_pump->RegisterIOHandler(file_, this);
        //       }
        //       // ���һЩ����, ȷ���еȴ���IO����ʱ���󲻻ᱻ����.
        //       ~MyFile() {
        //       }
        //       virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered,
        //         DWORD error) {
        //         ...
        //         delete context;
        //       }
        //       void DoSomeIo() {
        //         ...
        //         IOContext* context = new IOContext;
        //         // ����ʲô������, ֻ�Ƿ�ֹcontext������.
        //         context->handler = this;
        //         ReadFile(file_, buffer, num_bytes, &read, &context->overlapped);
        //       }
        //       HANDLE file_;
        //     };
        //
        // �÷�3:
        // ��ǰ��һ��, ֻ����������ʱ�����WaitForIOCompletion�ȴ����е�IO���.
        //     ~MyFile() {
        //       while(pending_)
        //         message_pump->WaitForIOCompletion(INFINITE, this);
        //     }
        class IOHandler
        {
        public:
            virtual ~IOHandler() {}
            // һ��|context|������IO�������, �����ᱻ����. |error|��Win32��IO����
            // �Ĵ�����(ERROR_SUCCESS��ʾû����). ��������ʱ|bytes_transfered|Ϊ0.
            virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered,
                DWORD error) = 0;
        };

        // IOObserver�����MessagePump����IO֪ͨ��Ϣ.
        //
        // ע��: IOObserver��ʵ��Ӧ���Ƿǳ����!
        class IOObserver
        {
        public:
            IOObserver() {}

            virtual void WillProcessIOEvent() = 0;
            virtual void DidProcessIOEvent() = 0;

        protected:
            virtual ~IOObserver() {}
        };

        // �����ص�IO��������Ҫ�������������. ��������ʱ, ��Ҫ����|handler|Ϊ�ļ�ע��
        // ��IOHandler, �ڲ������ǰ��������|handler|ΪNULL��ʾ����Ҫ�ٵ��ô�����.
        // OS֪ͨ�������ʱ��Ҫɾ��IOContext. ��������IO����ǣ�����Ļ������ڻص�ִ��
        // ֮ǰһֱ��ռ��, ���ּ���ֻ�����ڲ���Ҫ���⻺������IO(�ص��ṹ�屾�����).
        struct IOContext
        {
            OVERLAPPED overlapped;
            IOHandler* handler;
        };

        MessagePumpForIO();
        virtual ~MessagePumpForIO() {}

        virtual void ScheduleWork();
        virtual void ScheduleDelayedWork(const Time& delayed_work_time);

        // ע�ᴦ���������ļ����첽IO����¼�. ֻҪ|file_handle|�Ϸ�, ע��һֱ��Ч,
        // ����ֻҪ�еȴ����ļ�IO, |handler|�ͱ���Ϸ�.
        void RegisterIOHandler(HANDLE file_handle, IOHandler* handler);

        // �ȴ���һ����|filter|�����IO����¼�, �ȴ���ʱΪ|timeout|����. ���IO����
        // ��ɷ���true, ��ʱ����false. �����ɶ˿��յ���Ϣ, ��IO������Ϊ|filter|,
        // �ص������ڴ��뷵��֮ǰִ��; �������������|filter|, �ص����ӻ�ִ��, ������
        // ���������������. ֻ�е������������߳���ͣ�����ɷ������ټ��������, �ſ�
        // ������ʹ�ñ�����.
        bool WaitForIOCompletion(DWORD timeout, IOHandler* filter);

        void AddIOObserver(IOObserver* obs);
        void RemoveIOObserver(IOObserver* obs);

    private:
        struct IOItem
        {
            IOHandler* handler;
            IOContext* context;
            DWORD bytes_transfered;
            DWORD error;
        };

        virtual void DoRunLoop();
        void WaitForWork();
        bool MatchCompletedIOItem(IOHandler* filter, IOItem* item);
        bool GetIOItem(DWORD timeout, IOItem* item);
        bool ProcessInternalIOItem(const IOItem& item);
        void WillProcessIOEvent();
        void DidProcessIOEvent();

        // �̹߳�������ɶ˿�.
        ScopedHandle port_;
        // ���б�����Ϊ��. ���ڶ�������ʱ�����л�δͶ�ݵ�IO����¼�.
        std::list<IOItem> completed_io_;

        ObserverList<IOObserver> io_observers_;
    };

} //namespace base

#endif //__message_pump_win_h__