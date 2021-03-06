
#ifndef __observer_list_threadsafe_h__
#define __observer_list_threadsafe_h__

#pragma once

#include <algorithm>
#include <map>

#include "base/basic_types.h"
#include "base/logging.h"
#include "base/ref_counted.h"

#include "message_framework/callback.h"
#include "message_framework/message_loop.h"
#include "message_framework/observer_list.h"
#include "message_framework/task.h"

///////////////////////////////////////////////////////////////////////////////
//
// 概述:
//
// 线程安全的观察者容器. 与observer_list类似(见observer_list.h), 但在多线程
// 环境下更健壮.
//
// 支持以下用例:
//   * 观察者可以在任意线程的通知消息中注册. 回调观察者发生在观察者初始调用
//     AddObserver()的线程.
//   * 任意线程可以通过Notify()触发通知消息.
//   * 观察者可以在回调时从列表中移除自己.
//   * 如果一个线程正在通知观察者, 此时一个观察者正在从列表中移除自己, 通知
//     会被丢弃.
//
// 线程安全的观察者队列比非线程安全版本的不足就是通知消息的实时性差一些, 通
// 知消息都是通过PostTask()投递到其它线程, 非线程安全版本的通知消息是同步发
// 生的.
//
// 实现说明:
// ObserverListThreadSafe为每个线程维护一个ObserverList. 通知观察者时, 只是简单
// 的调用为每个注册的线程调用PostTask, 每个线程将通知自己的ObserverList.
//
///////////////////////////////////////////////////////////////////////////////
template<class ObserverType>
class ObserverListThreadSafe
    : public base::RefCountedThreadSafe<ObserverListThreadSafe<ObserverType> >
{
public:
    typedef typename ObserverList<ObserverType>::NotificationType
        NotificationType;

    ObserverListThreadSafe()
        : type_(ObserverListBase<ObserverType>::NOTIFY_ALL) {}
    explicit ObserverListThreadSafe(NotificationType type) : type_(type) {}

    ~ObserverListThreadSafe()
    {
        typename ObserversListMap::const_iterator it;
        for(it=observer_lists_.begin(); it!=observer_lists_.end(); ++it)
        {
            delete (*it).second;
        }
        observer_lists_.clear();
    }

    // 添加一个观察者到列表.
    void AddObserver(ObserverType* obs)
    {
        ObserverList<ObserverType>* list = NULL;
        MessageLoop* loop = MessageLoop::current();
        {
            base::AutoLock lock(list_lock_);
            if(observer_lists_.find(loop) == observer_lists_.end())
            {
                observer_lists_[loop] = new ObserverList<ObserverType>(type_);
            }
            list = observer_lists_[loop];
        }
        list->AddObserver(obs);
    }

    // 从列表中移除一个观察者.
    // 如果有发往观察者的通知消息, 它们将会被终止.
    // RemoveObserver必须在调用AddObserver的相同线程中被调用.
    void RemoveObserver(ObserverType* obs)
    {
        ObserverList<ObserverType>* list = NULL;
        MessageLoop* loop = MessageLoop::current();
        if(!loop)
        {
            return; // 关闭时, current()可能已经为空.
        }
        
        {
            base::AutoLock lock(list_lock_);
            list = observer_lists_[loop];
            if(!list)
            {
                NOTREACHED() << "RemoveObserver called on for unknown thread";
                return;
            }

            // 如果移除的是列表中最后一个观察者, 可以直接把整个列表移除.
            if(list->size() == 1)
            {
                observer_lists_.erase(loop);
            }
        }
        list->RemoveObserver(obs);

        // 如果RemoveObserver的调用来自一个通知消息, 列表的大小是非零的.
        // 这里不会删除, 而是在NotifyWrapper完成遍历以后删除.
        if(list->size() == 0)
        {
            delete list;
        }
    }

    // 通知方法.
    // 线程安全的回调到列表中的每个观察者.
    // 注意, 这些调用都是异步的. 你不能假设Notify调用完成时所有的观察者
    // 都已经被通知, 因为通知可能正在投递中.
    template<class Method>
    void Notify(Method m)
    {
        UnboundMethod<ObserverType, Method, Tuple0> method(m, MakeTuple());
        Notify<Method, Tuple0>(method);
    }

    template<class Method, class A>
    void Notify(Method m, const A &a)
    {
        UnboundMethod<ObserverType, Method, Tuple1<A> > method(m, MakeTuple(a));
        Notify<Method, Tuple1<A> >(method);
    }

private:
    template<class Method, class Params>
    void Notify(const UnboundMethod<ObserverType, Method, Params>& method)
    {
        base::AutoLock lock(list_lock_);
        typename ObserversListMap::iterator it;
        for(it=observer_lists_.begin(); it!=observer_lists_.end(); ++it)
        {
            MessageLoop* loop = (*it).first;
            ObserverList<ObserverType>* list = (*it).second;
            loop->PostTask(NewRunnableMethod(this,
                &ObserverListThreadSafe<ObserverType>::
                template NotifyWrapper<Method, Params>, list, method));
        }
    }

    // 为每个线程的ObserverList触发通知消息的调用封装. 函数调用必须在非安全
    // ObserverList所属的线程中被调用.
    template<class Method, class Params>
    void NotifyWrapper(ObserverList<ObserverType>* list,
        const UnboundMethod<ObserverType, Method, Params>& method)
    {

        // 检查列表是否还需要通知.
        {
            base::AutoLock lock(list_lock_);
            typename ObserversListMap::iterator it =
                observer_lists_.find(MessageLoop::current());

            // ObserverList可能已被移除. 实际上有可能是移除后又重新添加的! 如果
            // 主列表循环不符合这些情况, 我们没必要完成这个通知.
            if(it==observer_lists_.end() || it->second!=list)
            {
                return;
            }
        }

        {
            typename ObserverList<ObserverType>::Iterator it(*list);
            ObserverType* obs;
            while((obs=it.GetNext()) != NULL)
            {
                method.Run(obs);
            }
        }

        // 如果列表中没有观察者, 可以删除它.
        if(list->size() == 0)
        {
            {
                base::AutoLock lock(list_lock_);
                // 移除|list|, 如果还存在. 如果多个观察者在一个通知消息中同时移除
                // 时会发生这种情况. 见http://crbug.com/55725.
                typename ObserversListMap::iterator it =
                    observer_lists_.find(MessageLoop::current());
                if(it!=observer_lists_.end() && it->second==list)
                {
                    observer_lists_.erase(it);
                }
            }
            delete list;
        }
    }

    typedef std::map<MessageLoop*, ObserverList<ObserverType>*> ObserversListMap;

    base::Lock list_lock_; // 保护observer_lists_.
    ObserversListMap observer_lists_;
    const NotificationType type_;

    DISALLOW_COPY_AND_ASSIGN(ObserverListThreadSafe);
};

#endif //__observer_list_threadsafe_h__