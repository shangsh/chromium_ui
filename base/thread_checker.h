
#ifndef __thread_checker_h__
#define __thread_checker_h__

#pragma once

#include <windows.h>

#include "scoped_ptr.h"

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
class ThreadChecker
{
public:
    ThreadChecker();
    ~ThreadChecker();

    bool CalledOnValidThread() const;

    // �޸�CalledOnValidThread����֤�߳�, ��һ�ε���CalledOnValidThread����฽��
    // ��һ���µ��߳�, �Ƿ��뱩¶�˹���ȡ����NonThreadSafe��������. ��һ��������
    // һ���߳��д�����ֻ����һ���߳��б�ʹ�õ�ʱ��, �����õĴ˷���.
    void DetachFromThread();

private:
    void EnsureThreadIdAssigned() const;

    // ��ΪCalledOnValidThread�����ø�ֵ, ����ʹ��mutable.
    mutable scoped_ptr<DWORD> valid_thread_id_;
};
#else
// releaseģʽ��ʲô������.
class ThreadChecker
{
public:
    bool CalledOnValidThread() const
    {
        return true;
    }

    void DetachFromThread() {}
};
#endif //NDEBUG

#endif //__thread_checker_h__