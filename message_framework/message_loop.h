
#ifndef __message_loop_h__
#define __message_loop_h__

#pragma once

#include <queue>
#include <string>

#include "base/lock.h"

#include "message_pump_win.h"
#include "task.h"

// MessageLoop���ڴ����ض��̵߳��¼�. һ���߳����ֻ����һ��MessageLoop.
//
// �¼����ٰ���PostTask�ύ��Task����TimerManager�����DelayTask. �Ƿ�ᴦ��
// UI��Ϣȡ����MessageLoopʹ�õ���Ϣ������. Windows�ϵ��첽���̵���(���ʱ��
// ����)��һϵ��HANDLEs���ź�Ҳ���ܱ�����.
//
// ע��: �����ر�ǿ��, MessageLoop�ķ���ֻ����ִ��Run�������߳��е���.
//
// ע��: MessageLoop���������뱣��. Ҳ��һ�����񱻴����ʱ��, �ڶ������񲻻�
// ��ʼ, һֱ����һ���������. ����һ������ʱ�������ڲ���Ϣ��, ���ܻᷢ������,
// �ڲ��ô�������Ϣʱ�п�������һ���ڲ�������. �ڲ���Ϣ��ͨ���Ի���(
// DialogBox)��ͨ�öԻ���(GetOpenFileName)��OLE����(DoDragDrop)����ӡ����(
// StartDoc)�ȴ���.
//
//
// ��Ҫ�����ڲ�������ʱ����:
//     bool old_state = MessageLoop::current()->NestableTasksAllowed();
//     MessageLoop::current()->SetNestableTasksAllowed(true);
//     HRESULT hr = DoDragDrop(...); // �ڲ�ִ����һ��ģ̬����Ϣѭ��.
//     MessageLoop::current()->SetNestableTasksAllowed(old_state);
//     // ����DoDragDrop()�ķ���ֵhr.
//
// �ڵ���SetNestableTasksAllowed(true)ǰ, ��ȷ����������ǿ������, ��
// �е�ȫ�ֱ������ȶ����ҿɷ���.
class MessageLoop : public base::MessagePump::Delegate
{
public:
    // TaskObserver�����MessageLoop��������֪ͨ.
    //
    // ע��: TaskObserver��ʵ��Ӧ���Ƿǳ����!
    class TaskObserver
    {
    public:
        TaskObserver();

        // ��������ǰ����.
        virtual void WillProcessTask() = 0;

        // ������������.
        virtual void DidProcessTask() = 0;

    protected:
        virtual ~TaskObserver();
    };

    // DestructionObserver�ڵ�ǰMessageLoop����ǰ��֪ͨ, ��ʱMessageLoop::current()
    // ��δ���NULL. ����MessageLoop�������������һ������.
    //
    // ע��: ����۲��߱�֪ͨ�Ĺ�����Ͷ�ݵ�MessageLoop���������񶼲�������, ����
    // �ᱻɾ��.
    class DestructionObserver
    {
    public:
        virtual void WillDestroyCurrentMessageLoop() = 0;

    protected:
        virtual ~DestructionObserver();
    };

    // ���һ��DestructionObserver, ��ʼ����֪ͨ.
    void AddDestructionObserver(DestructionObserver* destruction_observer);

    // �Ƴ�һ��DestructionObserver. ��DestructionObserver����֪ͨ�ص�ʱ���ñ�
    // �����ǰ�ȫ.
    void RemoveDestructionObserver(DestructionObserver* destruction_observer);

    // "PostTask"ϵ�к�������Ϣѭ���н�����ĳ��ʱ����첽ִ�������Run����.
    //
    // ʹ��PostTask�汾, �����ǰ����Ƚ��ȳ���˳�����, ��������UI��Ϣ��IO�¼�
    // ͬʱ��ִ��. ʹ��PostDelayedTask�汾, �����Լ'delay_ms'�����ű�ִ��.
    //
    // NonNestable�汾����, ����֤��Ƕ�׵��õ�MessageLoop::Run�дӲ��ɷ�����,
    // �����ӳ����񵽶���MessageLoop::Runִ��ʱ�ɷ�.
    //
    // MessageLoop�ӹ�Task������Ȩ, ����ִ��Run()����֮��ᱻɾ��.
    //
    // ע��: ��Щ���������������߳��е���. Taskֻ����ִ��MessageLoop::Run()
    // ���߳��е���.
    void PostTask(Task* task);
    void PostDelayedTask(Task* task, int64 delay_ms);
    void PostNonNestableTask(Task* task);
    void PostNonNestableDelayedTask(Task* task, int64 delay_ms);

    // һ��ɾ��ָ�������PostTask, ��������Ҫ��MessageLoop����һ��ѭ��ʱ��
    // �õ�(������IPC�ص�ʱֱ��ɾ��RenderProcessHost������).
    //
    // ע��: ���������������߳��е���. ������ִ��MessageLoop::Run()���߳���
    // ��ɾ��, ����͵���PostDelayedTask()���̲߳���ͬһ�߳�, ��ôT����̳���
    // RefCountedThreadSafe<T>!
    template<class T>
    void DeleteSoon(T* object)
    {
        PostNonNestableTask(new DeleteTask<T>(object));
    }

    // һ���ͷ�ָ���������ü���(ͨ������Release����)��PostTask, ��������Ҫ���
    // ��MessageLoop����һ��ѭ��ʱ����߶�����Ҫ���ض��߳����ͷ�ʱ���õ�.
    //
    // ע��: ���������������߳��е���. ������ִ��MessageLoop::Run()���߳���
    // �ͷ�����(���ܻᱻɾ��), ����͵���PostDelayedTask()���̲߳���ͬһ�߳�,
    // ��ôT����̳���RefCountedThreadSafe<T>!
    template<class T>
    void ReleaseSoon(T* object)
    {
        PostNonNestableTask(new ReleaseTask<T>(object));
    }

    // ִ����Ϣѭ��.
    void Run();

    // �������еȴ�������Windows��Ϣ��, ����ȴ�/����. ��������п�ִ�е���Ŀ
    // ֮����������.
    void RunAllPending();

    // ֪ͨRun���������еȴ���Ϣ�������֮�󷵻�, ֻ���ڵ���Run����ͬ�߳���ʹ��,
    // ��Run���뻹��ִ��.
    //
    // �����Ҫֹͣ��һ���̵߳�MessageLoop, ����ʹ��QuitTask, ������Ҫע��, ���
    // Ŀ���߳�Ƕ�׵�����MessageLoop::Run, ��ô���൱Σ��.
    void Quit();

    // Quit����һ����ʽ, ����ȴ��������Ϣ������, Runֱ�ӷ���.
    void QuitNow();

    // ��ǰ���е�MessageLoop�е���Quit. ��������MessageLoop��Quit����.
    class QuitTask : public Task
    {
    public:
        virtual void Run()
        {
            MessageLoop::current()->Quit();
        }
    };

    // MessageLoop������ָ�����������ʱ��֮�⻹�ܴ�����첽�¼�����.
    //
    // TYPE_DEFAULT
    //   ֻ֧�������ʱ��.
    //
    // TYPE_UI
    //   ��֧�ֱ���UI��Ϣ(����Windows��Ϣ).
    //   �μ�MessageLoopForUI.
    //
    // TYPE_IO
    //   ��֧���첽IO.  �μ�MessageLoopForIO.
    enum Type
    {
        TYPE_DEFAULT,
        TYPE_UI,
        TYPE_IO
    };

    // һ����˵, ����Ҫʵ����MessageLoop����, �������õ�ǰ�̵߳�MessageLoopʵ��.
    explicit MessageLoop(Type type=TYPE_DEFAULT);
    ~MessageLoop();

    // ���ش��ݸ����캯������Ϣѭ������.
    Type type() const { return type_; }

    // ���̵߳����ֺ���Ϣѭ��������(ѡ���Ե���).
    void set_thread_name(const std::string& thread_name)
    {
        DCHECK(thread_name_.empty()) << "Should not rename this thread!";
        thread_name_ = thread_name;
    }
    const std::string& thread_name() const { return thread_name_; }

    // ���ص�ǰ�̵߳�MessageLoop����, û�з���NULL.
    static MessageLoop* current();

    // ������߽�ֹ����ݹ������. �ڵݹ����Ϣѭ���з���. ��ʹ��ͨ�ÿؼ�����
    // ��ӡ������ʱ��, �����һЩ�������Ϣѭ��. ȱʡ����½�ֹ����ݹ������.
    //
    // 
    // �����Ŷӵ��ض����:
    // - �߳�����ִ����Ϣѭ��.
    // - ��������#1��ִ��.
    // - ����#1Ҳ������һ����Ϣѭ��, ����MessageBox��StartDoc����GetSaveFileName.
    // - �ڵڶ�����Ϣѭ��֮ǰ����ִ����, �߳̽��յ�����#2.
    // - ��NestableTasksAllowed����Ϊtrue, ����#2������ִ��. ����, ��������#1���
    //   ��Ϣѭ��֮��Żᱻִ��.
    void SetNestableTasksAllowed(bool allowed);
    bool NestableTasksAllowed() const;

    // ��һ��������������|loop|ִ��Ƕ������.
    class ScopedNestableTaskAllower
    {
    public:
        explicit ScopedNestableTaskAllower(MessageLoop* loop)
            : loop_(loop), old_state_(loop_->NestableTasksAllowed())
        {
            loop_->SetNestableTasksAllowed(true);
        }
        ~ScopedNestableTaskAllower()
        {
            loop_->SetNestableTasksAllowed(old_state_);
        }

    private:
        MessageLoop* loop_;
        bool old_state_;
    };

    // ������߽�ֹʹ�õ���Run()ǰ���õ�δ�����쳣�����������쳣. ���������
    // ���������SetUnhandledExceptionFilter()���Ǵ�û�ָ�, ������������.
    void set_exception_restoration(bool restore)
    {
        exception_restoration_ = restore;
    }

    // �����ǰ������Ƕ�׵���Ϣѭ������true.
    bool IsNested();

    // ֻ���ڵ�ǰ��Ϣѭ�����е��߳��е���.
    void AddTaskObserver(TaskObserver* task_observer);
    void RemoveTaskObserver(TaskObserver* task_observer);

    typedef base::MessagePumpWin::Dispatcher Dispatcher;
    typedef base::MessagePumpForUI::Observer Observer;

    // �����Ϣѭ�������˸߾���ʱ�ӷ���true. ���ڲ���.
    bool high_resolution_timers_enabled()
    {
        return !high_resolution_timer_expiration_.is_null();
    }

    // ���ø߾���ʱ��ģʽʱ, ����ά��1s.
    static const int kHighResolutionTimerModeLeaseTimeMs = 1000;

protected:
    struct RunState
    {
        // Run()����ջ����.
        int run_depth;

        // ��¼�Ƿ������Quit(), һ����Ϣ�ÿ���, ѭ������ֹͣ.
        bool quit_received;

        Dispatcher* dispatcher;
    };

    class AutoRunState : RunState
    {
    public:
        explicit AutoRunState(MessageLoop* loop);
        ~AutoRunState();

    private:
        MessageLoop* loop_;
        RunState* previous_state_;
    };

    // �ṹ�尴ֵ����.
    struct PendingTask
    {
        Task* task;                   // ִ�е�����.
        base::Time delayed_run_time;  // ʲôʱ��ִ��.
        int sequence_num;             // ���ڼ򻯰���ʱ������.
        bool nestable;                // ����Ƕ��ѭ���ɷ�ʱΪtrue.

        PendingTask(Task* task, bool nestable)
            : task(task), sequence_num(0), nestable(nestable) {}

        // ����֧������.
        bool operator<(const PendingTask& other) const;
    };

    class TaskQueue : public std::queue<PendingTask>
    {
    public:
        void Swap(TaskQueue* queue)
        {
            c.swap(queue->c); // ����std::deque::swap
        }
    };

    typedef std::priority_queue<PendingTask> DelayedTaskQueue;

    base::MessagePumpWin* pump_win()
    {
        return static_cast<base::MessagePumpWin*>(pump_.get());
    }

    // ������װ������Ϣѭ��ִ�ж�ջ�������쳣�Ĵ�������. �Ƿ���SEH��try����
    // ִ����Ϣѭ��������set_exception_restoration()���õı�־λ, �ֱ����
    // RunInternalInSEHFrame()��RunInternal().
    void RunHandler();

    __declspec(noinline) void RunInternalInSEHFrame();

    // ����Ϣѭ��ִ�е���Χ���һЩջ��Ϣ, ֧��״̬�ı���ͻָ����ڵݹ����.
    void RunInternal();

    // ���ô��������ӳٵķ�Ƕ������.
    bool ProcessNextDelayedNonNestableTask();

    //----------------------------------------------------------------------------
    // ִ��work_queue_���������new_task, ��ɾ��(���ͨ��PostTask�����). ���
    // ���Ŷӵ�����, ִ�������һ�����Ŷ�new_task. new_task�ǿ�ѡ��, ��ΪNULL.
    // ����ΪNULLʱ, ��ִ��һ���ȴ�������(�������). ���ִ�������񷵻غ���true.
    // ֻ�е�һ����Ƕ�׵���������ִ��ʱ, new_task�Ż��ۻ���work_queue_����. ����
    // ����һ������������������ִ�б�����Ϣ��(���ضԻ���)ʱ����.
    bool QueueOrRunTask(Task* new_task);

    // ִ���ض�������ɾ��.
    void RunTask(Task* task);

    // ����RunTask, �����������ִ�������pending_task���ӳٶ���. ����ִ�з�
    // ��true.
    bool DeferOrRunPendingTask(const PendingTask& pending_task);

    // ���pending_task��delayed_work_queue_.
    void AddToDelayedWorkQueue(const PendingTask& pending_task);

    // ���work_queue_Ϊ��, ��incoming_queue_��������work_queue_. ����
    // incoming_queue_��Ҫ����, work_queue_���ڱ��߳�ֱ�ӷ���.
    void ReloadWorkQueue();

    // ɾ����û�����е�����, ��Щ���񲻱�ִ��. ��������������ȷ�����е�����
    // ���ܱ�����. �����������������, ����true.
    bool DeletePendingTasks();

    // ����һ������incoming_queue_.
    void PostTask_Helper(Task* task, int64 delay_ms, bool nestable);

    virtual bool DoWork();
    virtual bool DoDelayedWork(base::Time* next_delayed_work_time);
    virtual bool DoIdleWork();

    Type type_;

    // ��Ҫ����������������б�. ע�����ֻ���ڵ�ǰ�̷߳���(push/pop).
    TaskQueue work_queue_;

    // �洢�ӳ�����, ����'delayed_run_time'��������.
    DelayedTaskQueue delayed_work_queue_;

    // ��Ƕ�׵���Ϣѭ����ִ�з�Ƕ�׵�����, ��Щ��������Ŷ����ӳ�ִ��. ���뿪
    // Ƕ����Ϣѭ��, ��������ִ��.
    TaskQueue deferred_non_nestable_work_queue_;

    scoped_refptr<base::MessagePump> pump_;

    ObserverList<DestructionObserver> destruction_observers_;

    // ��һ��Ƕ�׵���Ϣ������ֹ����ִ��.
    bool nestable_tasks_allowed_;

    bool exception_restoration_;

    std::string thread_name_;

    // ���ڽ��յ���������, ��Щ����δ��������work_queue_.
    TaskQueue incoming_queue_;
    // incoming_queue_���ʱ���.
    base::Lock incoming_queue_lock_;

    RunState* state_;

    base::TimeTicks high_resolution_timer_expiration_;

    // �ӳ�����ʹ�õ���һ�����.
    int next_sequence_num_;

    ObserverList<TaskObserver> task_observers_;

    DISALLOW_COPY_AND_ASSIGN(MessageLoop);
};

//-----------------------------------------------------------------------------
// MessageLoopForUI��չMessageLoop, ��TYPE_UI��ʼ��MessageLoop����UI��Ϣ��.
//
// ����ĳ����÷�:
//     MessageLoopForUI::current()->...����ĳ������...
class MessageLoopForUI : public MessageLoop
{
public:
    MessageLoopForUI() : MessageLoop(TYPE_UI) {}

    // ���ص�ǰ�̵߳�MessageLoopForUI.
    static MessageLoopForUI* current()
    {
        MessageLoop* loop = MessageLoop::current();
        DCHECK_EQ(MessageLoop::TYPE_UI, loop->type());
        return static_cast<MessageLoopForUI*>(loop);
    }

    void DidProcessMessage(const MSG& message);

    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);
    void Run(Dispatcher* dispatcher);

protected:
    base::MessagePumpForUI* pump_ui()
    {
        return static_cast<base::MessagePumpForUI*>(pump_.get());
    }
};
// ��Ҫ��MessageLoopForUI����κγ�Ա! ��ΪMessageLoopForUI�ķ���һ����ͨ��
// MessageLoop(TYPE_UI), �κζ�������ݶ�Ӧ�ô洢��MessageLoop��pump_ʵ����.
COMPILE_ASSERT(sizeof(MessageLoop)==sizeof(MessageLoopForUI),
               MessageLoopForUI_should_not_have_extra_member_variables);

//-----------------------------------------------------------------------------
// MessageLoopForUI��չMessageLoop, ��TYPE_IO��ʼ��MessageLoop����IO��Ϣ��.
//
// ����ĳ����÷�:
//     MessageLoopForIO::current()->...����ĳ������...
class MessageLoopForIO : public MessageLoop
{
public:
    typedef base::MessagePumpForIO::IOHandler IOHandler;
    typedef base::MessagePumpForIO::IOContext IOContext;
    typedef base::MessagePumpForIO::IOObserver IOObserver;

    MessageLoopForIO() : MessageLoop(TYPE_IO) {}

    // ���ص�ǰ�̵߳�MessageLoopForIO.
    static MessageLoopForIO* current()
    {
        MessageLoop* loop = MessageLoop::current();
        DCHECK_EQ(MessageLoop::TYPE_IO, loop->type());
        return static_cast<MessageLoopForIO*>(loop);
    }

    void AddIOObserver(IOObserver* io_observer)
    {
        pump_io()->AddIOObserver(io_observer);
    }

    void RemoveIOObserver(IOObserver* io_observer)
    {
        pump_io()->RemoveIOObserver(io_observer);
    }

    void RegisterIOHandler(HANDLE file_handle, IOHandler* handler);
    bool WaitForIOCompletion(DWORD timeout, IOHandler* filter);

protected:
    base::MessagePumpForIO* pump_io()
    {
        return static_cast<base::MessagePumpForIO*>(pump_.get());
    }
};
// ��Ҫ��MessageLoopForIO����κγ�Ա! ��ΪMessageLoopForIO�ķ���һ����ͨ��
// MessageLoop(TYPE_IO), �κζ�������ݶ�Ӧ�ô洢��MessageLoop��pump_ʵ����.
COMPILE_ASSERT(sizeof(MessageLoop)==sizeof(MessageLoopForIO),
               MessageLoopForIO_should_not_have_extra_member_variables);

#endif //__message_loop_h__