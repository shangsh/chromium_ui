
#ifndef __waitable_event_h__
#define __waitable_event_h__

#pragma once

#include <windows.h>

#include "basic_types.h"

namespace base
{

    // �滻Win32�µ�INFINITE.
    static const int kNoTimeout = -1;

    class TimeDelta;

    // WaitableEvent��Ϊ�߳�ͬ������, ����һ���̵߳ȴ���һ���߳����һ������.
    // ���ڷ�Windowsϵͳ, ֻ����ͬһ��ַ�ռ���ʹ��.
    //
    // ʹ��WaitableEvent����һ��booleanֵ, �����Ҫʹ��Lock+ConditionVariable.
    // ���ǵ��㷢��ʹ��WaitableEvent+Lock�ȴ�һ�������ӵ�״̬�仯ʱ(����ȴ�һ
    // ��Ԫ����ӵ�������), ��ʱ������Ҫʹ��ConditionVariable������WaitableEvent.
    //
    // ע��: ��Windows��, �������ṩ��һЩevent����Ĺ���. �����дWindowsƽ̨
    // ����ʱ��Ҫevent������һЩ����, ���ֱ��ʹ��Windows��event����.
    class WaitableEvent
    {
    public:
        // ���manual_resetΪtrue, �����¼������ź�״̬, �û������ֶ�����Reset.
        // �������Ϊfalse, �ڵȴ��߳��ͷź�, ϵͳ���Զ������¼������ź�״̬.
        WaitableEvent(bool manual_reset, bool initially_signaled);

        // ͨ���Ѿ�������Event�������WaitableEvent. ����ӹܾ��������Ȩ, ��
        // ��ʱ��رվ��.
        explicit WaitableEvent(HANDLE event_handle);

        // �Ӷ������ͷž��������Ȩ.
        HANDLE Release();

        ~WaitableEvent();

        // ����Event�����ź�״̬.
        void Reset();

        // ����Event���ź�״̬. �������еȴ��߳�.
        void Signal();

        // ����¼������ź�״̬����true, ���򷵻�false. ��������ֶ������¼�,
        // �������ûᵼ���¼�����.
        bool IsSignaled();

        // �����ڵصȴ�ֱ���¼������ź�״̬. ����¼������ź�״̬, ����true,
        // ���򷵻�false��ʾ�ȴ�ʧ��.
        bool Wait();

        // �ȴ�ֱ���¼������ź�״̬, max_timeΪ��ʱʱ��. ����¼������ź�״̬,
        // ����true. ��������false, ����һ����ζ�ų�ʱ, �п��ܵȴ�ʧ��.
        bool TimedWait(const TimeDelta& max_time);

        HANDLE handle() const { return handle_; }

        // ͬ���ȴ�����¼�.
        //   waitables: һ��WaitableEventָ��
        //   count: @waitables��Ԫ�ظ���
        //
        // ����ֵ: �����ź�״̬��WaitableEvent����.
        //
        // �ڵȴ���ʱ����ɾ���κ�WaitableEvent����.
        static size_t WaitMany(WaitableEvent** waitables, size_t count);

        // �첽�ȴ�, �μ�WaitableEventWatcher.

    private:
        friend class WaitableEventWatcher;

        HANDLE handle_;

        DISALLOW_COPY_AND_ASSIGN(WaitableEvent);
    };

} //namespace base

#endif //__waitable_event_h__