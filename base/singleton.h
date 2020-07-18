
#ifndef __singleton_h__
#define __singleton_h__

#pragma once

#include "at_exit.h"
#include "atomicops.h"

namespace base
{

    // Singleton<Type>ȱʡ��traits. ����new��delete�������ٶ���.
    // kRegisterAtExitע������˳�ʱ�Ƿ��Զ�����.
    // �����Ҫ�����������߲�ͬ���ڴ���䷽ʽ���Խ�������.
    template<typename Type>
    struct DefaultSingletonTraits
    {
        // �������.
        static Type* New()
        {
            // ()������ǳ���Ҫ, ǿ��POD���ͳ�ʼ��.
            return new Type();
        }

        // ���ٶ���.
        static void Delete(Type* x)
        {
            delete x;
        }

        // true: �����˳�ʱ�Զ�ɾ������.
        static const bool kRegisterAtExit = true;
    };


    // Singleton<Type>��һ����ѡ��traits, ָ�������˳�ʱ���������.
    template<typename Type>
    struct LeakySingletonTraits : public DefaultSingletonTraits<Type>
    {
        static const bool kRegisterAtExit = false;
    };


    // Singleton<Type, Traits>���ڵ�һ��ʹ�õ�ʱ�򴴽�
    // һ��ʵ��, ���������˳���ʱ������. �������˳��������Trait::Delete.
    //
    // ʹ��ʾ��: (���ַ�ʽ��һ��)
    //   1. FooClass* ptr = Singleton<FooClass>::get();
    //      ptr->Bar();
    //   2. Singleton<FooClass>()->Bar();
    //   3. Singleton<FooClass>::get()->Bar();
    //
    // Singleton<>û�зǾ�̬��ԱҲ����Ҫʵ����. ��Ȼʵ����Ϊ���Ա����ȫ�ֶ���
    // ��û�������, ��Ϊ����POD���͵�.
    //
    // ��������̰߳�ȫ��, ����벢��ʹ����ôType��Ҳ�������̰߳�ȫ��,
    // �û����������Ҫ��֤���������.
    //
    // ����:
    //   RAE = kRegisterAtExit
    //
    // ���Traits::RAE��true, �����ڽ����˳���ʱ�������, ׼ȷ��˵����AtExitManager
    // ��Ҫʵ����һ���������͵Ķ���. AtExitManagerģ����atexit()��LIFO������嵫��
    // �����ϸ���ȫһЩ. �������ݲμ�at_exit.h.
    //
    // ���Traits::RAE��false, �����ڽ����˳���ʱ�򲻻��ͷ�, ��˻���й©(�����Ѵ���).
    // Traits::RAE�Ǳ�Ҫһ�㲻Ҫ����Ϊfalse, ��Ϊ�����Ķ�����ܻᱻCRT����.
    //
    // ����������һ������, ���๹�캯��private, ������DefaultSingletonTraits<>��Ԫ.
    //
    //   #include "singleton.h"
    //   class FooClass {
    //    public:
    //     void Bar() { ... }
    //    private:
    //     FooClass() { ... }
    //     friend struct DefaultSingletonTraits<FooClass>;
    //
    //     DISALLOW_COPY_AND_ASSIGN(FooClass);
    //   };
    //
    // ע��:
    // (a) ÿ�ε���get(),operator->()��operator*()�������(16ns on my P4/2.8GHz)����
    //     �ж϶����Ƿ��Ѿ���ʼ��. ���Ի���get()���, ��Ϊָ�벻��仯.
    //
    // (b) ����ʵ����������Ҫ�׳��쳣, ��Ϊ�಻���쳣��ȫ(exception-safe)��.
    template<typename Type, typename Traits=DefaultSingletonTraits<Type> >
    class Singleton
    {
    public:
        // ��������͸�ֵ�����ǰ�ȫ��, ��Ϊû���κγ�Ա.

        // ����Type���ʵ��ָ��
        static Type* get()
        {
            // AtomicWordҲ��������������, kBeingCreatedMarker��ʾ�������ȴ�����.
            static const AtomicWord kBeingCreatedMarker = 1;

            AtomicWord value = NoBarrier_Load(&_instance);
            if(value!=0 && value!=kBeingCreatedMarker)
            {
                return reinterpret_cast<Type*>(value);
            }

            // ����û������, ���Դ���.
            if(Acquire_CompareAndSwap(&_instance, 0, kBeingCreatedMarker) == 0)
            {
                // _instanceΪNULL��������kBeingCreatedMarker. ֻ������һ���߳�������,
                // �����߳�ֻ��ѭ���ȴ�(spinning).
                Type* newval = Traits::New();

                Release_Store(&_instance, reinterpret_cast<AtomicWord>(newval));

                if(newval!=NULL && Traits::kRegisterAtExit)
                {
                    AtExitManager::RegisterCallback(OnExit, NULL);
                }

                return newval;
            }

            // ��ͻ, �����߳��Ѿ�:
            // - ���ڴ���
            // - �Ѿ��������
            // value != NULLʱ������kBeingCreatedMarker���ߺϷ�ָ��.
            // һ�㲻���ͻ, ����ʵ��������ǳ���ʱ, ��ʱ����ѭ���л��������߳�ֱ��
            // ���󴴽��ɹ�.
            while(true)
            {
                value = NoBarrier_Load(&_instance);
                if(value != kBeingCreatedMarker)
                {
                    break;
                }
                Sleep(0);
            }

            return reinterpret_cast<Type*>(value);
        }

        Type& operator*()
        {
            return *get();
        }

        Type* operator->()
        {
            return get();
        }

    private:
        // AtExit()�����亯��. Ӧ���ڵ��߳��е���, ���ǰ����ܲ��Ǳ����.
        static void OnExit(void* unused)
        {
            // ����ʵ������֮��Ż���AtExit��ע��. _instanceָ��Ƿ��򲻻����.
            Traits::Delete(reinterpret_cast<Type*>(
                NoBarrier_AtomicExchange(&_instance, 0)));
        }
        static AtomicWord _instance;
    };

    template<typename Type, typename Traits>
    AtomicWord Singleton<Type, Traits>::_instance = 0;

} //namespace base

#endif //__singleton_h__