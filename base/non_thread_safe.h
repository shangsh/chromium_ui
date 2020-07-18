
#ifndef __non_thread_safe_h__
#define __non_thread_safe_h__

#pragma once

#include "thread_checker.h"

// ������֤�෽�������Ƿ�����ͬ���߳���. �Ӹ���̳�, ����CalledOnValidThread()
// ������֤.
//
// ���������ڱ�֤��Щ�����̰߳�ȫ�����ܱ���ȫʹ��. ����, һ���������һ������
// ���õ���.
//
// ʾ��:
//     class MyClass : public NonThreadSafe {
//     public:
//       void Foo() {
//         DCHECK(CalledOnValidThread());
//         ... (do stuff) ...
//       }
//     }
//
// ��Releaseģʽ��, CalledOnValidThreadʼ�շ���true.

#ifndef NDEBUG
class NonThreadSafe
{
public:
    ~NonThreadSafe();

    bool CalledOnValidThread() const;

protected:
    // �޸�CalledOnValidThread����֤�߳�, ��һ�ε���CalledOnValidThread����฽��
    // ��һ���µ��߳�, �Ƿ��뱩¶�˹���ȡ����NonThreadSafe��������. ��һ��������
    // һ���߳��д�����ֻ����һ���߳��б�ʹ�õ�ʱ��, �����õĴ˷���.
    void DetachFromThread();

private:
    ThreadChecker thread_checker_;
};
#else
// releaseģʽ��ʲô������.
class NonThreadSafe
{
public:
    bool CalledOnValidThread() const
    {
        return true;
    }

protected:
    void DetachFromThread() {}
};
#endif //NDEBUG

#endif //__non_thread_safe_h__