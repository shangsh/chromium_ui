
#ifndef __singleton_h__
#define __singleton_h__

#pragma once

#include "at_exit.h"
#include "atomicops.h"

namespace base
{

    // Singleton<Type>缺省的traits. 调用new和delete创建销毁对象.
    // kRegisterAtExit注册进程退出时是否自动销毁.
    // 如果需要其它参数或者不同的内存分配方式可以进行重载.
    template<typename Type>
    struct DefaultSingletonTraits
    {
        // 分配对象.
        static Type* New()
        {
            // ()在这里非常重要, 强制POD类型初始化.
            return new Type();
        }

        // 销毁对象.
        static void Delete(Type* x)
        {
            delete x;
        }

        // true: 进程退出时自动删除对象.
        static const bool kRegisterAtExit = true;
    };


    // Singleton<Type>另一个可选的traits, 指明程序退出时不清除对象.
    template<typename Type>
    struct LeakySingletonTraits : public DefaultSingletonTraits<Type>
    {
        static const bool kRegisterAtExit = false;
    };


    // Singleton<Type, Traits>类在第一次使用的时候创建
    // 一个实例, 程序正常退出的时候销毁. 非正常退出不会调用Trait::Delete.
    //
    // 使用示例: (几种方式都一样)
    //   1. FooClass* ptr = Singleton<FooClass>::get();
    //      ptr->Bar();
    //   2. Singleton<FooClass>()->Bar();
    //   3. Singleton<FooClass>::get()->Bar();
    //
    // Singleton<>没有非静态成员也不需要实例化. 当然实例化为类成员或者全局对象
    // 是没有问题的, 因为它是POD类型的.
    //
    // 这个类是线程安全的, 如果想并发使用那么Type类也必须是线程安全的,
    // 用户根据设计需要保证两部分配合.
    //
    // 术语:
    //   RAE = kRegisterAtExit
    //
    // 如果Traits::RAE是true, 单件在进程退出的时候会销毁, 准确的说调用AtExitManager
    // 需要实例化一个这种类型的对象. AtExitManager模拟了atexit()的LIFO序的语义但是
    // 调用上更安全一些. 更多内容参见at_exit.h.
    //
    // 如果Traits::RAE是false, 单件在进程退出的时候不会释放, 因此会有泄漏(假如已创建).
    // Traits::RAE非必要一般不要设置为false, 因为创建的对象可能会被CRT销毁.
    //
    // 如果你想设计一个单件, 将类构造函数private, 并设置DefaultSingletonTraits<>友元.
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
    // 注意:
    // (a) 每次调用get(),operator->()和operator*()都有损耗(16ns on my P4/2.8GHz)用于
    //     判断对象是否已经初始化. 可以缓存get()结果, 因为指针不会变化.
    //
    // (b) 对象实例化函数不要抛出异常, 因为类不是异常安全(exception-safe)的.
    template<typename Type, typename Traits=DefaultSingletonTraits<Type> >
    class Singleton
    {
    public:
        // 拷贝构造和赋值构造是安全的, 因为没有任何成员.

        // 返回Type类的实例指针
        static Type* get()
        {
            // AtomicWord也起到自旋锁的作用, kBeingCreatedMarker表示自旋锁等待创建.
            static const AtomicWord kBeingCreatedMarker = 1;

            AtomicWord value = NoBarrier_Load(&_instance);
            if(value!=0 && value!=kBeingCreatedMarker)
            {
                return reinterpret_cast<Type*>(value);
            }

            // 对象还没被创建, 尝试创建.
            if(Acquire_CompareAndSwap(&_instance, 0, kBeingCreatedMarker) == 0)
            {
                // _instance为NULL且已设置kBeingCreatedMarker. 只可能有一个线程在这里,
                // 其它线程只能循环等待(spinning).
                Type* newval = Traits::New();

                Release_Store(&_instance, reinterpret_cast<AtomicWord>(newval));

                if(newval!=NULL && Traits::kRegisterAtExit)
                {
                    AtExitManager::RegisterCallback(OnExit, NULL);
                }

                return newval;
            }

            // 冲突, 其它线程已经:
            // - 正在创建
            // - 已经创建完成
            // value != NULL时可能是kBeingCreatedMarker或者合法指针.
            // 一般不会冲突, 除非实例对象构造非常耗时, 此时不断循环切换到其它线程直至
            // 对象创建成功.
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
        // AtExit()的适配函数. 应该在单线程中调用, 这个前提可能不是必须的.
        static void OnExit(void* unused)
        {
            // 单件实例创建之后才会在AtExit中注册. _instance指针非法则不会调用.
            Traits::Delete(reinterpret_cast<Type*>(
                NoBarrier_AtomicExchange(&_instance, 0)));
        }
        static AtomicWord _instance;
    };

    template<typename Type, typename Traits>
    AtomicWord Singleton<Type, Traits>::_instance = 0;

} //namespace base

#endif //__singleton_h__