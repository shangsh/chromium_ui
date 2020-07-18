
#ifndef __task_queue_h__
#define __task_queue_h__

#pragma once

#include <deque>

#include "task.h"

// TaskQueue�ǵȴ�ִ�е��������. ִ���������Run����. TaskQueueҲ������, ����
// ���Է��õ���Ϣѭ������������Ϣ����.
class TaskQueue : public Task
{
public:
    TaskQueue();
    ~TaskQueue();

    // ִ�ж��������е�����. ִ�й�������ӵ������񽫻�����һ��|Run|����ʱִ��.
    virtual void Run();

    // ������񵽶���, ����ִ�е�ʱ����������ӵ�˳������ִ��.
    //
    // �ӹ�|task|����Ȩ, ����������ɺ�ᱻ�Զ�ɾ��(û��ִ�е��������TaskQueue����
    // ʱ����).
    void Push(Task* task);

    // �Ƴ������е���������, ����ɾ��.
    void Clear();

    // ���������û�����񷵻�true.
    bool IsEmpty() const;

private:
    // �ȴ�ִ�е�����.
    std::deque<Task*> queue_;
};

#endif //__task_queue_h__