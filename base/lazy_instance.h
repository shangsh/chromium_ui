
#ifndef __lazy_instance_h__
#define __lazy_instance_h__

#pragma once

#include "atomicops.h"

// LazyInstance<Type, Traits>�����Type�ĵ�һʵ��, �����ڵ�һ�η���ʱ������.
// һ��������Ҫ��������ľ�̬����ʱ, �����ǳ�����, ����Ҫ��֤�̰߳�ȫ��.
// Type�Ĺ��캯����ʹ���̳߳�ͻ�������Ҳֻ�ᱻ����һ��. Get()��Pointer()��
// �Ƿ�����ͬ����ȫ��ʼ����ʵ������. ���������ڳ����˳�ʱ����.
//
// ������󱻰�ȫ�Ĵ���, LazyInstanceҲ���̰߳�ȫ��. �����POD��ʼ��, ���Բ�
// ��Ҫ��̬�Ĺ��캯��. ͨ��base::LinkerInitialized����һ��ȫ�ֵ�LazyInstance
// �����ǱȽϺõ�����.
//
// LazyInstance���Ƶ���, ��û�е���������. ��ͬ�����Ϳ����ж��LazyInstance,
// ÿ��LazyInstanceά��һ��Ψһ��ʵ��. ����ΪTypeԤ����ռ�, �����ڶ��ϴ���
// ����. �����������ʵ������������, ���ٶ���Ƭ. ��ҪType�������������Ա�ȷ
// ����С.
//
// �÷�ʾ��:
//     static LazyInstance<MyClass> my_instance(base::LINKER_INITIALIZED);
//     void SomeMethod() {
//       my_instance.Get().SomeMethod(); // MyClass::SomeMethod()
//
//       MyClass* ptr = my_instance.Pointer();
//       ptr->DoDoDo();  // MyClass::DoDoDo
//     }

namespace base
{

    template<typename Type>
    struct DefaultLazyInstanceTraits
    {
        static Type* New(void* instance)
        {
            // ʹ��placement new��Ԥ����Ŀռ��ϳ�ʼ��ʵ��.
            // Բ���ź���Ҫ, ǿ��POD���ͳ�ʼ��.
            return new (instance) Type();
        }
        static void Delete(void* instance)
        {
            // ��ʽ������������.
            reinterpret_cast<Type*>(instance)->~Type();
        }
    };

    // ��ȡ���ַ�ģ�庯��, �������԰Ѹ��ӵĴ���Ƭ�η���.cpp�ļ�.
    class LazyInstanceHelper
    {
    protected:
        enum
        {
            STATE_EMPTY    = 0,
            STATE_CREATING = 1,
            STATE_CREATED  = 2
        };

        explicit LazyInstanceHelper(LinkerInitialized x) { /* state_Ϊ0 */ }

        // ����Ƿ���Ҫ����ʵ��. ����true��ʾ��Ҫ����, ����false��ʾ�����߳�
        // ���ڴ���, �ȴ�ʵ��������ɲ�����false.
        bool NeedsInstance();

        // ����ʵ��֮��, ע������˳��ǵ��õ���������, ������stateΪ
        // STATE_CREATED.
        void CompleteInstance(void* instance, void (*dtor)(void*));

        base::Atomic32 state_;

    private:
        DISALLOW_COPY_AND_ASSIGN(LazyInstanceHelper);
    };

    template<typename Type, typename Traits=DefaultLazyInstanceTraits<Type> >
    class LazyInstance : public LazyInstanceHelper
    {
    public:
        explicit LazyInstance(LinkerInitialized x) : LazyInstanceHelper(x) {}

        Type& Get()
        {
            return *Pointer();
        }

        Type* Pointer()
        {
            // ��������Ѿ�������, ϣ���ܿ��ٷ���.
            if((base::NoBarrier_Load(&state_)!=STATE_CREATED) &&
                NeedsInstance())
            {
                // ��|buf_|���ڵĿռ��ϴ���ʵ��.
                instance_ = Traits::New(buf_);
                CompleteInstance(instance_, Traits::Delete);
            }

            return instance_;
        }

    private:
        int8 buf_[sizeof(Type)]; // Typeʵ����Ԥ����ռ�.
        Type* instance_;

        DISALLOW_COPY_AND_ASSIGN(LazyInstance);
    };

} //namespace base

#endif //__lazy_instance_h__