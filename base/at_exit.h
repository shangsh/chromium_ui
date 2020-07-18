
#ifndef __at_exit_h__
#define __at_exit_h__

#pragma once

#include <stack>

#include "basic_types.h"
#include "lock.h"

namespace base
{

    // AtExitManager�ṩ������CRT��atexit()�Ĺ���, ��ͬ������������˺�ʱ����
    // �ص�����.��Windows�µ�DLL�������ش����ʱ��, ��������������лص�����.
    // AtExitManagerһ������Singleton��.
    //
    // ʹ�úܼ�, ֻ��Ҫ��main()����WinMain()����������ͷ����һ��ջ����:
    //     int main(...)
    //     {
    //         base::AtExitManager exit_manager;
    //     }
    // ��exit_manager�뿪�������ʱ��, ����ע��Ļص������͵�������������
    // �ᱻ����.
    class AtExitManager
    {
    public:
        typedef void (*AtExitCallbackType)(void*);

        AtExitManager();
        // ����������������ע��Ļص�, ����֮������ע��ص�.
        ~AtExitManager();

        // ע���˳�ʱ�Ļص�����. ����ԭ����void func().
        static void RegisterCallback(AtExitCallbackType func, void* param);
        // ��LIFO�����ע��Ļص�����, ��������֮�����ע���µĻص�.
        static void ProcessCallbacksNow();

    private:
        struct CallbackAndParam
        {
            CallbackAndParam(AtExitCallbackType func, void* param)
                : func_(func), param_(param) {}

            AtExitCallbackType func_;
            void* param_;
        };

        Lock lock_;
        std::stack<CallbackAndParam> stack_;

        DISALLOW_COPY_AND_ASSIGN(AtExitManager);
    };

} //namespace base

#endif //__at_exit_h__