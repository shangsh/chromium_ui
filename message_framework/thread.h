
#ifndef __thread_h__
#define __thread_h__

#pragma once

#include "message_loop.h"
#include "message_loop_proxy.h"

namespace base
{

    // 实现本接口, 在后台线程中执行代码. 新创建的线程会调用ThreadMain方法.
    class ThreadDelegate
    {
    public:
        virtual ~ThreadDelegate() {}
        virtual void ThreadMain() = 0;
    };

    // 一个简单的线程抽象, 在新线程中创建一个MessageLoop. 使用线程的MessageLoop可
    // 以让代码在这个线程中执行. 当对象销毁时, 线程也被终止. 线程消息循环中排队等
    // 待的任务在线程结束前都会被执行.
    //
    // 线程停止后, 析构顺序是:
    //
    //  (1) Thread::CleanUp()
    //  (2) MessageLoop::~MessageLoop
    //  (3.b)  MessageLoop::DestructionObserver::WillDestroyCurrentMessageLoop
    //  (4) Thread::CleanUpAfterMessageLoopDestruction()
    class Thread : ThreadDelegate
    {
    public:
        struct Options
        {
            // 指定线程分配的消息循环的类型.
            MessageLoop::Type message_loop_type;

            // 指定线程允许使用的最大栈空间, 和线程的初始栈空间不一定相同.
            // 0表示应该使用缺省的最大值.
            size_t stack_size;

            Options() : message_loop_type(MessageLoop::TYPE_DEFAULT),
                stack_size(0) {}
            Options(MessageLoop::Type type, size_t size)
                : message_loop_type(type), stack_size(size) {}
        };

        // 构造函数.
        // name是线程的字符串标识.
        explicit Thread(const char* name);

        // 线程的析构函数, 有必要会先停止线程.
        //
        // 注意: 如果从Thread派生, 希望自己的CleanUp函数被调用, 你需要在自己的析构
        // 函数中调用Stop().
        virtual ~Thread();

        // 设置对调试器可见的线程名字. 没有调试器的话, 不做任何事情.
        static void SetName(const char* name);

        // 创建一个新线程. |stack_size|参数可以为0, 表示使用缺省的栈空间大小. 如果
        // 成功, |*thread_handle|被赋值为新创建的线程句柄, |delegate|的ThreadMain
        // 将会在新线程中执行.
        // 注意: 线程句柄不再使用时, 必须调用Join方法释放线程相关的系统资源. 确保
        // Delegate对象在线程退出前存在.
        static bool Create(size_t stack_size, ThreadDelegate* delegate,
            HANDLE* thread_handle);

        // 结束一个通过Create方法创建的线程. 函数调用堵塞直到目标线程退出. 会导致
        // |thread_handle|非法.
        static void Join(HANDLE thread_handle);

        // 启动线程. 如果线程成功启动返回true, 否则返回false, message_loop()是否返
        // 回为空取决于这个返回值.
        //
        // 注意: 在Windows平台, 函数不能在加载器上锁的时候调用. 比如DllMain函数中,
        // 全局对象构造析构时, atexit()回调时.
        bool Start();

        // 启动线程. 缺省的options构造方式和Start()行为一致.
        //
        // 注意: 在Windows平台, 函数不能在加载器上锁的时候调用. 比如DllMain函数中,
        // 全局对象构造析构时, atexit()回调时.
        bool StartWithOptions(const Options& options);

        // 通知线程退出, 在线程退出后立即返回. 方法返回后, 线程对象完全重置, 可以当作
        // 新构造的对象使用(比如再次启动).
        //
        // Stop可以多次调用, 如果线程已经停止, 不做任何处理.
        //
        // 注意: 本方法是可选的, 因为Thread的析构函数会自动停止线程.
        void Stop();

        // 停止线程尽快退出.
        //
        // 警告: 不要轻易使用本函数, 会有风险. 调用本函数会导致后面message_loop()返
        // 回非法值. 引入这个函数是为了解决Windows平台下和打印机工作线程的死锁问题.
        // 任何其它情况下应该使用Stop().
        //
        // 不能多次调用StopSoon, 那样做很危险, 在访问message_loop()时会引起时序问题.
        // 一旦发现线程退出了, 调用Stop()重置线程对象.
        void StopSoon();

        // 返回本线程的MessageLoopProxy. 使用MessageLoopProxy的PostTask方法可以在
        // 这个线程执行代码. 只有线程调用Start成功后, 函数返回值才不为空. Stop线程
        // 之后再次调用, 方法会返回NULL.
        //
        // 注意: 不要直接调用返回的MessageLoop的Quit方法. 应该用线程的Stop方法替代.
        MessageLoop* message_loop() const { return message_loop_; }

        // 返回本线程的MessageLoopProxy. 使用MessageLoopProxy的PostTask方法可以在
        // 这个线程执行代码. 只有线程调用Start成功后, 函数返回值才不为空. 即使线程
        // 退出, 调用者还拥有这个对象.
        scoped_refptr<MessageLoopProxy> message_loop_proxy()
        {
            return message_loop_proxy_;
        }

        // 获取线程的名字(调试时显示用).
        const std::string& thread_name() { return name_; }

        // 本地线程句柄.
        HANDLE thread_handle() { return thread_; }

        // 线程ID.
        DWORD thread_id() const { return thread_id_; }

        // 如果线程已经启动还未停止返回true. 线程运行时, thread_id_不为0.
        bool IsRunning() const { return thread_id_ != 0; }

    protected:
        // 启动消息循环前调用.
        virtual void Init() {}

        // 启动消息循环.
        virtual void Run(MessageLoop* message_loop);

        // 消息循环结束后调用.
        virtual void CleanUp() {}

        // 删除消息循环后调用. 一般来说, 用户应优先使用CleanUp(). 当需要在所有的
        // MessageLoop::DestructionObservers完成之后执行代码, 需要实现这个虚函数.
        virtual void CleanUpAfterMessageLoopDestruction() {}

        static void SetThreadWasQuitProperly(bool flag);
        static bool GetThreadWasQuitProperly();

        void set_message_loop(MessageLoop* message_loop)
        {
            message_loop_ = message_loop;
        }

    private:
        // ThreadDelegate方法:
        virtual void ThreadMain();

        bool thread_was_started() const { return started_; }

        // 线程是否启动成功?
        bool started_;

        // true表示正在停止, 不应该再访问|message_loop_|, 可能是空值或者非法值.
        bool stopping_;

        // 传递数据给ThreadMain.
        struct StartupData;
        StartupData* startup_data_;

        // 线程的句柄.
        HANDLE thread_;

        // 线程的消息循环, 当线程运行时合法, 由被创建的线程设置.
        MessageLoop* message_loop_;

        // 本线程的MessageLoopProxy实现. 对象在线程退出后还可以存在.
        scoped_refptr<MessageLoopProxy> message_loop_proxy_;

        // 线程ID.
        DWORD thread_id_;

        // 线程名称, 用于调试.
        std::string name_;

        friend class ThreadQuitTask;

        DISALLOW_COPY_AND_ASSIGN(Thread);
    };

} //namespace base

#endif //__thread_h__