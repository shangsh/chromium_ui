
#include "thread_checker.h"

#ifndef NDEBUG

ThreadChecker::ThreadChecker()
{
    EnsureThreadIdAssigned();
}

ThreadChecker::~ThreadChecker() {}

bool ThreadChecker::CalledOnValidThread() const
{
    EnsureThreadIdAssigned();
    return *valid_thread_id_ == GetCurrentThreadId();
}

void ThreadChecker::DetachFromThread()
{
    valid_thread_id_.reset();
}

void ThreadChecker::EnsureThreadIdAssigned() const
{
    if(valid_thread_id_.get())
    {
        return;
    }
    valid_thread_id_.reset(new DWORD(GetCurrentThreadId()));
}

#endif //NDEBUG