
#ifndef __thread_h__
#define __thread_h__

#pragma once

#include "message_loop.h"
#include "message_loop_proxy.h"

namespace base
{

    // ʵ�ֱ��ӿ�, �ں�̨�߳���ִ�д���. �´������̻߳����ThreadMain����.
    class ThreadDelegate
    {
    public:
        virtual ~ThreadDelegate() {}
        virtual void ThreadMain() = 0;
    };

    // һ���򵥵��̳߳���, �����߳��д���һ��MessageLoop. ʹ���̵߳�MessageLoop��
    // ���ô���������߳���ִ��. ����������ʱ, �߳�Ҳ����ֹ. �߳���Ϣѭ�����Ŷӵ�
    // �����������߳̽���ǰ���ᱻִ��.
    //
    // �߳�ֹͣ��, ����˳����:
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
            // ָ���̷߳������Ϣѭ��������.
            MessageLoop::Type message_loop_type;

            // ָ���߳�����ʹ�õ����ջ�ռ�, ���̵߳ĳ�ʼջ�ռ䲻һ����ͬ.
            // 0��ʾӦ��ʹ��ȱʡ�����ֵ.
            size_t stack_size;

            Options() : message_loop_type(MessageLoop::TYPE_DEFAULT),
                stack_size(0) {}
            Options(MessageLoop::Type type, size_t size)
                : message_loop_type(type), stack_size(size) {}
        };

        // ���캯��.
        // name���̵߳��ַ�����ʶ.
        explicit Thread(const char* name);

        // �̵߳���������, �б�Ҫ����ֹͣ�߳�.
        //
        // ע��: �����Thread����, ϣ���Լ���CleanUp����������, ����Ҫ���Լ�������
        // �����е���Stop().
        virtual ~Thread();

        // ���öԵ������ɼ����߳�����. û�е������Ļ�, �����κ�����.
        static void SetName(const char* name);

        // ����һ�����߳�. |stack_size|��������Ϊ0, ��ʾʹ��ȱʡ��ջ�ռ��С. ���
        // �ɹ�, |*thread_handle|����ֵΪ�´������߳̾��, |delegate|��ThreadMain
        // ���������߳���ִ��.
        // ע��: �߳̾������ʹ��ʱ, �������Join�����ͷ��߳���ص�ϵͳ��Դ. ȷ��
        // Delegate�������߳��˳�ǰ����.
        static bool Create(size_t stack_size, ThreadDelegate* delegate,
            HANDLE* thread_handle);

        // ����һ��ͨ��Create�����������߳�. �������ö���ֱ��Ŀ���߳��˳�. �ᵼ��
        // |thread_handle|�Ƿ�.
        static void Join(HANDLE thread_handle);

        // �����߳�. ����̳߳ɹ���������true, ���򷵻�false, message_loop()�Ƿ�
        // ��Ϊ��ȡ�����������ֵ.
        //
        // ע��: ��Windowsƽ̨, ���������ڼ�����������ʱ�����. ����DllMain������,
        // ȫ�ֶ���������ʱ, atexit()�ص�ʱ.
        bool Start();

        // �����߳�. ȱʡ��options���췽ʽ��Start()��Ϊһ��.
        //
        // ע��: ��Windowsƽ̨, ���������ڼ�����������ʱ�����. ����DllMain������,
        // ȫ�ֶ���������ʱ, atexit()�ص�ʱ.
        bool StartWithOptions(const Options& options);

        // ֪ͨ�߳��˳�, ���߳��˳�����������. �������غ�, �̶߳�����ȫ����, ���Ե���
        // �¹���Ķ���ʹ��(�����ٴ�����).
        //
        // Stop���Զ�ε���, ����߳��Ѿ�ֹͣ, �����κδ���.
        //
        // ע��: �������ǿ�ѡ��, ��ΪThread�������������Զ�ֹͣ�߳�.
        void Stop();

        // ֹͣ�߳̾����˳�.
        //
        // ����: ��Ҫ����ʹ�ñ�����, ���з���. ���ñ������ᵼ�º���message_loop()��
        // �طǷ�ֵ. �������������Ϊ�˽��Windowsƽ̨�ºʹ�ӡ�������̵߳���������.
        // �κ����������Ӧ��ʹ��Stop().
        //
        // ���ܶ�ε���StopSoon, ��������Σ��, �ڷ���message_loop()ʱ������ʱ������.
        // һ�������߳��˳���, ����Stop()�����̶߳���.
        void StopSoon();

        // ���ر��̵߳�MessageLoopProxy. ʹ��MessageLoopProxy��PostTask����������
        // ����߳�ִ�д���. ֻ���̵߳���Start�ɹ���, ��������ֵ�Ų�Ϊ��. Stop�߳�
        // ֮���ٴε���, �����᷵��NULL.
        //
        // ע��: ��Ҫֱ�ӵ��÷��ص�MessageLoop��Quit����. Ӧ�����̵߳�Stop�������.
        MessageLoop* message_loop() const { return message_loop_; }

        // ���ر��̵߳�MessageLoopProxy. ʹ��MessageLoopProxy��PostTask����������
        // ����߳�ִ�д���. ֻ���̵߳���Start�ɹ���, ��������ֵ�Ų�Ϊ��. ��ʹ�߳�
        // �˳�, �����߻�ӵ���������.
        scoped_refptr<MessageLoopProxy> message_loop_proxy()
        {
            return message_loop_proxy_;
        }

        // ��ȡ�̵߳�����(����ʱ��ʾ��).
        const std::string& thread_name() { return name_; }

        // �����߳̾��.
        HANDLE thread_handle() { return thread_; }

        // �߳�ID.
        DWORD thread_id() const { return thread_id_; }

        // ����߳��Ѿ�������δֹͣ����true. �߳�����ʱ, thread_id_��Ϊ0.
        bool IsRunning() const { return thread_id_ != 0; }

    protected:
        // ������Ϣѭ��ǰ����.
        virtual void Init() {}

        // ������Ϣѭ��.
        virtual void Run(MessageLoop* message_loop);

        // ��Ϣѭ�����������.
        virtual void CleanUp() {}

        // ɾ����Ϣѭ�������. һ����˵, �û�Ӧ����ʹ��CleanUp(). ����Ҫ�����е�
        // MessageLoop::DestructionObservers���֮��ִ�д���, ��Ҫʵ������麯��.
        virtual void CleanUpAfterMessageLoopDestruction() {}

        static void SetThreadWasQuitProperly(bool flag);
        static bool GetThreadWasQuitProperly();

        void set_message_loop(MessageLoop* message_loop)
        {
            message_loop_ = message_loop;
        }

    private:
        // ThreadDelegate����:
        virtual void ThreadMain();

        bool thread_was_started() const { return started_; }

        // �߳��Ƿ������ɹ�?
        bool started_;

        // true��ʾ����ֹͣ, ��Ӧ���ٷ���|message_loop_|, �����ǿ�ֵ���߷Ƿ�ֵ.
        bool stopping_;

        // �������ݸ�ThreadMain.
        struct StartupData;
        StartupData* startup_data_;

        // �̵߳ľ��.
        HANDLE thread_;

        // �̵߳���Ϣѭ��, ���߳�����ʱ�Ϸ�, �ɱ��������߳�����.
        MessageLoop* message_loop_;

        // ���̵߳�MessageLoopProxyʵ��. �������߳��˳��󻹿��Դ���.
        scoped_refptr<MessageLoopProxy> message_loop_proxy_;

        // �߳�ID.
        DWORD thread_id_;

        // �߳�����, ���ڵ���.
        std::string name_;

        friend class ThreadQuitTask;

        DISALLOW_COPY_AND_ASSIGN(Thread);
    };

} //namespace base

#endif //__thread_h__