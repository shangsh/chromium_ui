
#include "message_loop.h"

#include "base/lazy_instance.h"
#include "base/thread_local.h"

#include "message_pump_default.h"

namespace
{

    // ������(lazily created)��TLS, ���ڿ��ٷ����̵߳���Ϣѭ��(�������).
    // �൱��һ����ȫ������ľ�̬���캯��.
    base::LazyInstance<base::ThreadLocalPointer<MessageLoop> > lazy_tls_ptr(
        base::LINKER_INITIALIZED);

}

// ���߳�����SEH�쳣ʱ, �ָ��ɵ�δ�����쳣������.
static int SEHFilter(LPTOP_LEVEL_EXCEPTION_FILTER old_filter)
{
    ::SetUnhandledExceptionFilter(old_filter);
    return EXCEPTION_CONTINUE_SEARCH;
}

// ���ص�ǰ��δ�����쳣������ָ��, û�п�ֱ�ӻ�ȡ��API.
static LPTOP_LEVEL_EXCEPTION_FILTER GetTopSEHFilter()
{
    LPTOP_LEVEL_EXCEPTION_FILTER top_filter = NULL;
    top_filter = ::SetUnhandledExceptionFilter(0);
    ::SetUnhandledExceptionFilter(top_filter);
    return top_filter;
}

MessageLoop::TaskObserver::TaskObserver() {}

MessageLoop::TaskObserver::~TaskObserver() {}

MessageLoop::DestructionObserver::~DestructionObserver() {}

// static
MessageLoop* MessageLoop::current()
{
    return lazy_tls_ptr.Pointer()->Get();
}

MessageLoop::MessageLoop(Type type)
: type_(type),
nestable_tasks_allowed_(true),
exception_restoration_(false),
state_(NULL),
next_sequence_num_(0)
{
    DCHECK(!current()) << "should only have one message loop per thread";
    lazy_tls_ptr.Pointer()->Set(this);

#define MESSAGE_PUMP_UI new base::MessagePumpForUI()
#define MESSAGE_PUMP_IO new base::MessagePumpForIO()

    if(type_ == TYPE_UI)
    {
        pump_ = MESSAGE_PUMP_UI;
    }
    else if(type_ == TYPE_IO)
    {
        pump_ = MESSAGE_PUMP_IO;
    }
    else
    {
        DCHECK_EQ(TYPE_DEFAULT, type_);
        pump_ = new base::MessagePumpDefault();
    }
}

MessageLoop::~MessageLoop()
{
    DCHECK(this == current());

    // ��DestructionObservers������this�Ļ���.
    FOR_EACH_OBSERVER(DestructionObserver, destruction_observers_,
        WillDestroyCurrentMessageLoop());

    DCHECK(!state_);

    // ��������δ���������, ����ע��: ɾ��һ��������ܻᵼ������µ�����(����ͨ��
    // DeleteSoon). ���ǶԱ�ɾ����������������Ĵ�������һ���������. һ����˵, ѭ
    // ��ִ��һ���ξͻ��˳�. �������ʱ����ѭ�����������ֵ, ��������һ������һֱ��
    // ���������, ���Լ����п������µ���ʲô����.
    bool did_work;
    for(int i=0; i<100; ++i)
    {
        DeletePendingTasks();
        ReloadWorkQueue();
        // �������Ϊ��, ����ѭ��.
        did_work = DeletePendingTasks();
        if(!did_work)
        {
            break;
        }
    }
    DCHECK(!did_work);

    // ����ΪNULL, ���󲻻��ٱ����ʵ�.
    lazy_tls_ptr.Pointer()->Set(NULL);
}

void MessageLoop::AddDestructionObserver(DestructionObserver* destruction_observer)
{
    DCHECK(this == current());
    destruction_observers_.AddObserver(destruction_observer);
}

void MessageLoop::RemoveDestructionObserver(
    DestructionObserver* destruction_observer)
{
    DCHECK(this == current());
    destruction_observers_.RemoveObserver(destruction_observer);
}

void MessageLoop::AddTaskObserver(TaskObserver* task_observer)
{
    DCHECK_EQ(this, current());
    task_observers_.AddObserver(task_observer);
}

void MessageLoop::RemoveTaskObserver(TaskObserver* task_observer)
{
    DCHECK_EQ(this, current());
    task_observers_.RemoveObserver(task_observer);
}

void MessageLoop::Run()
{
    AutoRunState save_state(this);
    RunHandler();
}

void MessageLoop::RunAllPending()
{
    AutoRunState save_state(this);
    state_->quit_received = true; // ����ֱ������ʱ�˳�, ��������ȴ�״̬.
    RunHandler();
}

// ������SEHģʽ��ִ��ѭ��:
// exception_restoration_ = false: ����δ�����쳣������������
// SetUnhandledExceptionFilter()���õĹ�����.
// exception_restoration_ = true: ����δ�����쳣������ѭ��ִ��ǰ�Ĺ�����.
void MessageLoop::RunHandler()
{
    if(exception_restoration_)
    {
        RunInternalInSEHFrame();
        return;
    }

    RunInternal();
}

__declspec(noinline) void MessageLoop::RunInternalInSEHFrame()
{
    LPTOP_LEVEL_EXCEPTION_FILTER current_filter = GetTopSEHFilter();
    __try
    {
        RunInternal();
    }
    __except(SEHFilter(current_filter))
    {
    }
    return;
}

void MessageLoop::RunInternal()
{
    DCHECK(this == current());

    if(state_->dispatcher && type()==TYPE_UI)
    {
        static_cast<base::MessagePumpForUI*>(pump_.get())->
            RunWithDispatcher(this, state_->dispatcher);
        return;
    }

    pump_->Run(this);
}

bool MessageLoop::ProcessNextDelayedNonNestableTask()
{
    if(state_->run_depth != 1)
    {
        return false;
    }

    if(deferred_non_nestable_work_queue_.empty())
    {
        return false;
    }

    Task* task = deferred_non_nestable_work_queue_.front().task;
    deferred_non_nestable_work_queue_.pop();

    RunTask(task);
    return true;
}

void MessageLoop::Quit()
{
    DCHECK(current() == this);
    if(state_)
    {
        state_->quit_received = true;
    }
    else
    {
        NOTREACHED() << "Must be inside Run to call Quit";
    }
}

void MessageLoop::QuitNow()
{
    DCHECK(current() == this);
    if(state_)
    {
        pump_->Quit();
    }
    else
    {
        NOTREACHED() << "Must be inside Run to call Quit";
    }
}

void MessageLoop::PostTask(Task* task)
{
    PostTask_Helper(task, 0, true);
}

void MessageLoop::PostDelayedTask(Task* task, int64 delay_ms)
{
    PostTask_Helper(task, delay_ms, true);
}

void MessageLoop::PostNonNestableTask(Task* task)
{
    PostTask_Helper(task, 0, false);
}

void MessageLoop::PostNonNestableDelayedTask(Task* task, int64 delay_ms)
{
    PostTask_Helper(task, delay_ms, false);
}

// ���ܱ���̨�̵߳���!
void MessageLoop::PostTask_Helper(Task* task, int64 delay_ms, bool nestable)
{
    PendingTask pending_task(task, nestable);

    if(delay_ms > 0)
    {
        pending_task.delayed_run_time = base::Time::Now() +
            base::TimeDelta::FromMilliseconds(delay_ms);

        if(high_resolution_timer_expiration_.is_null())
        {
            // Windowsʱ�ӵľ�����15.6ms. ���ֻ��С��15.6ms��ʱ�����ø߾���ʱ
            // ��, ��ô18ms��ʱ�Ӵ�Լ����32msʱ����, �������ǳ���. �����Ҫ
            // ������С��2��15.6ms��ʱ�����ø߾���ʱ��. ���Ǿ��Ⱥ͵�Դ����֮��
            // ��ƽ��.
            bool needs_high_res_timers =
                delay_ms < (2 * base::Time::kMinLowResolutionThresholdMs);
            if(needs_high_res_timers)
            {
                base::Time::ActivateHighResolutionTimer(true);
                high_resolution_timer_expiration_ = base::TimeTicks::Now() +
                    base::TimeDelta::FromMilliseconds(kHighResolutionTimerModeLeaseTimeMs);
            }
        }
    }
    else
    {
        DCHECK_EQ(delay_ms, 0) << "delay should not be negative";
    }

    if(!high_resolution_timer_expiration_.is_null())
    {
        if(base::TimeTicks::Now() > high_resolution_timer_expiration_)
        {
            base::Time::ActivateHighResolutionTimer(false);
            high_resolution_timer_expiration_ = base::TimeTicks();
        }
    }

    // ����: ��Ҫ�ڵ�ǰ�߳���ֱ��ѭ��ִ������, �������ܻ�����ⲿ�߳�.
    // �����е����񶼷������.

    scoped_refptr<base::MessagePump> pump;
    {
        base::AutoLock locked(incoming_queue_lock_);

        bool was_empty = incoming_queue_.empty();
        incoming_queue_.push(pending_task);
        if(!was_empty)
        {
            return; // �ڱ�����������Ϣ��.
        }

        pump = pump_;
    }
    // ��Ϊincoming_queue_�п��������ٱ���Ϣѭ��������, ���Ժ������֮ǰ����
    // ���������. ʹ��һ��ջ����������Ϣ��, �������ǿ�����incoming_queue_lock_
    // ���������ScheduleWork.

    pump->ScheduleWork();
}

void MessageLoop::SetNestableTasksAllowed(bool allowed)
{
    if(nestable_tasks_allowed_ != allowed)
    {
        nestable_tasks_allowed_ = allowed;
        if(!nestable_tasks_allowed_)
        {
            return;
        }
        // ����������Ϣ��.
        pump_->ScheduleWork();
    }
}

bool MessageLoop::NestableTasksAllowed() const
{
    return nestable_tasks_allowed_;
}

bool MessageLoop::IsNested()
{
    return state_->run_depth > 1;
}

void MessageLoop::RunTask(Task* task)
{
    DCHECK(nestable_tasks_allowed_);
    // ִ������, ��ȡ���ϸ�ʽ: ��������.
    nestable_tasks_allowed_ = false;

    FOR_EACH_OBSERVER(TaskObserver, task_observers_, WillProcessTask());
    task->Run();
    FOR_EACH_OBSERVER(TaskObserver, task_observers_, DidProcessTask());
    delete task;

    nestable_tasks_allowed_ = true;
}

bool MessageLoop::DeferOrRunPendingTask(const PendingTask& pending_task)
{
    if(pending_task.nestable || state_->run_depth==1)
    {
        RunTask(pending_task.task);
        // ����ִ����һ������(ע��: �����ܻᵽ��һ��������!).
        return true;
    }

    // ���ڲ���ִ������, ��Ϊ��Ƕ����Ϣѭ��������������Ƕ��ִ��.
    deferred_non_nestable_work_queue_.push(pending_task);
    return false;
}

void MessageLoop::AddToDelayedWorkQueue(const PendingTask& pending_task)
{
    // �Ƶ��ӳ��������. ����delayed_work_queue_����ǰ��ʼ�����. ����ڶ�
    // ��ͬ�ӳ�ִ��ʱ���������������ʱ���õ�, ��֤�Ƚ��ȳ�.
    PendingTask new_pending_task(pending_task);
    new_pending_task.sequence_num = next_sequence_num_++;
    delayed_work_queue_.push(new_pending_task);
}

void MessageLoop::ReloadWorkQueue()
{
    // ��work_queue_Ϊ�յ�ʱ��Ŵ�incoming_queue_��������, �������Լ��ٶ�
    // incoming_queue_��������, �������.
    if(!work_queue_.empty())
    {
        return; // �б�Ҫʱ, �Ŷ�incoming_queue_lock_���������м�������.
    }

    // ���������̳߳�ͻ.
    {
        base::AutoLock lock(incoming_queue_lock_);
        if(incoming_queue_.empty())
        {
            return;
        }
        incoming_queue_.Swap(&work_queue_); // ����ʱ��.
        DCHECK(incoming_queue_.empty());
    }
}

bool MessageLoop::DeletePendingTasks()
{
    bool did_work = !work_queue_.empty();
    while(!work_queue_.empty())
    {
        PendingTask pending_task = work_queue_.front();
        work_queue_.pop();
        if(!pending_task.delayed_run_time.is_null())
        {
            // �ӳ������ɾ��˳�������ɾ��˳�򱣳�һ��. �ӳ�����֮����ܴ���������ϵ.
            AddToDelayedWorkQueue(pending_task);
        }
        else
        {
            delete pending_task.task;
        }
    }
    did_work |= !deferred_non_nestable_work_queue_.empty();
    while(!deferred_non_nestable_work_queue_.empty())
    {
        Task* task = NULL;
        task = deferred_non_nestable_work_queue_.front().task;
        deferred_non_nestable_work_queue_.pop();
        if(task)
        {
            delete task;
        }
    }
    did_work |= !delayed_work_queue_.empty();
    while(!delayed_work_queue_.empty())
    {
        Task* task = delayed_work_queue_.top().task;
        delayed_work_queue_.pop();
        delete task;
    }
    return did_work;
}

bool MessageLoop::DoWork()
{
    if(!nestable_tasks_allowed_)
    {
        // ������������ִ��.
        return false;
    }

    for(;;)
    {
        ReloadWorkQueue();
        if(work_queue_.empty())
        {
            break;
        }

        // ִ�����������.
        do
        {
            PendingTask pending_task = work_queue_.front();
            work_queue_.pop();
            if(!pending_task.delayed_run_time.is_null())
            {
                AddToDelayedWorkQueue(pending_task);
                // �����������������仯, ��Ҫ���µ���ʱ��.
                if(delayed_work_queue_.top().task == pending_task.task)
                {
                    pump_->ScheduleDelayedWork(pending_task.delayed_run_time);
                }
            }
            else
            {
                if(DeferOrRunPendingTask(pending_task))
                {
                    return true;
                }
            }
        } while(!work_queue_.empty());
    }

    // ʲô��û����.
    return false;
}

bool MessageLoop::DoDelayedWork(base::Time* next_delayed_work_time)
{
    if(!nestable_tasks_allowed_ || delayed_work_queue_.empty())
    {
        *next_delayed_work_time = base::Time();
        return false;
    }

    if(delayed_work_queue_.top().delayed_run_time > base::Time::Now())
    {
        *next_delayed_work_time = delayed_work_queue_.top().delayed_run_time;
        return false;
    }

    PendingTask pending_task = delayed_work_queue_.top();
    delayed_work_queue_.pop();

    if(!delayed_work_queue_.empty())
    {
        *next_delayed_work_time = delayed_work_queue_.top().delayed_run_time;
    }

    return DeferOrRunPendingTask(pending_task);
}

bool MessageLoop::DoIdleWork()
{
    if(ProcessNextDelayedNonNestableTask())
    {
        return true;
    }

    if(state_->quit_received)
    {
        pump_->Quit();
    }

    return false;
}

MessageLoop::AutoRunState::AutoRunState(MessageLoop* loop) : loop_(loop)
{
    previous_state_ = loop_->state_;
    if(previous_state_)
    {
        run_depth = previous_state_->run_depth + 1;
    }
    else
    {
        run_depth = 1;
    }
    loop_->state_ = this;

    quit_received = false;
    dispatcher = NULL;
}

MessageLoop::AutoRunState::~AutoRunState()
{
    loop_->state_ = previous_state_;
}

bool MessageLoop::PendingTask::operator<(const PendingTask& other) const
{
    // ��Ϊ���ȼ������������Ԫ�����ȼ����, ����ıȽ���Ҫ������. ʱ����С��
    // ��������.
    if(delayed_run_time < other.delayed_run_time)
    {
        return false;
    }

    if(delayed_run_time > other.delayed_run_time)
    {
        return true;
    }

    // ���ʱ��պ����, ͨ����ž���. �ȶ���ŵĲ�ֵ, ��������֧�����������.
    return (sequence_num - other.sequence_num) > 0;
}


void MessageLoopForUI::DidProcessMessage(const MSG& message)
{
    pump_win()->DidProcessMessage(message);
}

void MessageLoopForUI::AddObserver(Observer* observer)
{
    pump_ui()->AddObserver(observer);
}

void MessageLoopForUI::RemoveObserver(Observer* observer)
{
    pump_ui()->RemoveObserver(observer);
}

void MessageLoopForUI::Run(Dispatcher* dispatcher)
{
    AutoRunState save_state(this);
    state_->dispatcher = dispatcher;
    RunHandler();
}


void MessageLoopForIO::RegisterIOHandler(HANDLE file, IOHandler* handler)
{
    pump_io()->RegisterIOHandler(file, handler);
}

bool MessageLoopForIO::WaitForIOCompletion(DWORD timeout, IOHandler* filter)
{
    return pump_io()->WaitForIOCompletion(timeout, filter);
}