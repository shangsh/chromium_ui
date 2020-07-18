
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
// ����:
//
// �̰߳�ȫ�Ĺ۲�������. ��observer_list����(��observer_list.h), ���ڶ��߳�
// �����¸���׳.
//
// ֧����������:
//   * �۲��߿����������̵߳�֪ͨ��Ϣ��ע��. �ص��۲��߷����ڹ۲��߳�ʼ����
//     AddObserver()���߳�.
//   * �����߳̿���ͨ��Notify()����֪ͨ��Ϣ.
//   * �۲��߿����ڻص�ʱ���б����Ƴ��Լ�.
//   * ���һ���߳�����֪ͨ�۲���, ��ʱһ���۲������ڴ��б����Ƴ��Լ�, ֪ͨ
//     �ᱻ����.
//
// �̰߳�ȫ�Ĺ۲��߶��бȷ��̰߳�ȫ�汾�Ĳ������֪ͨ��Ϣ��ʵʱ�Բ�һЩ, ͨ
// ֪��Ϣ����ͨ��PostTask()Ͷ�ݵ������߳�, ���̰߳�ȫ�汾��֪ͨ��Ϣ��ͬ����
// ����.
//
// ʵ��˵��:
// ObserverListThreadSafeΪÿ���߳�ά��һ��ObserverList. ֪ͨ�۲���ʱ, ֻ�Ǽ�
// �ĵ���Ϊÿ��ע����̵߳���PostTask, ÿ���߳̽�֪ͨ�Լ���ObserverList.
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

    // ���һ���۲��ߵ��б�.
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

    // ���б����Ƴ�һ���۲���.
    // ����з����۲��ߵ�֪ͨ��Ϣ, ���ǽ��ᱻ��ֹ.
    // RemoveObserver�����ڵ���AddObserver����ͬ�߳��б�����.
    void RemoveObserver(ObserverType* obs)
    {
        ObserverList<ObserverType>* list = NULL;
        MessageLoop* loop = MessageLoop::current();
        if(!loop)
        {
            return; // �ر�ʱ, current()�����Ѿ�Ϊ��.
        }
        
        {
            base::AutoLock lock(list_lock_);
            list = observer_lists_[loop];
            if(!list)
            {
                NOTREACHED() << "RemoveObserver called on for unknown thread";
                return;
            }

            // ����Ƴ������б������һ���۲���, ����ֱ�Ӱ������б��Ƴ�.
            if(list->size() == 1)
            {
                observer_lists_.erase(loop);
            }
        }
        list->RemoveObserver(obs);

        // ���RemoveObserver�ĵ�������һ��֪ͨ��Ϣ, �б�Ĵ�С�Ƿ����.
        // ���ﲻ��ɾ��, ������NotifyWrapper��ɱ����Ժ�ɾ��.
        if(list->size() == 0)
        {
            delete list;
        }
    }

    // ֪ͨ����.
    // �̰߳�ȫ�Ļص����б��е�ÿ���۲���.
    // ע��, ��Щ���ö����첽��. �㲻�ܼ���Notify�������ʱ���еĹ۲���
    // ���Ѿ���֪ͨ, ��Ϊ֪ͨ��������Ͷ����.
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

    // Ϊÿ���̵߳�ObserverList����֪ͨ��Ϣ�ĵ��÷�װ. �������ñ����ڷǰ�ȫ
    // ObserverList�������߳��б�����.
    template<class Method, class Params>
    void NotifyWrapper(ObserverList<ObserverType>* list,
        const UnboundMethod<ObserverType, Method, Params>& method)
    {

        // ����б��Ƿ���Ҫ֪ͨ.
        {
            base::AutoLock lock(list_lock_);
            typename ObserversListMap::iterator it =
                observer_lists_.find(MessageLoop::current());

            // ObserverList�����ѱ��Ƴ�. ʵ�����п������Ƴ�����������ӵ�! ���
            // ���б�ѭ����������Щ���, ����û��Ҫ������֪ͨ.
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

        // ����б���û�й۲���, ����ɾ����.
        if(list->size() == 0)
        {
            {
                base::AutoLock lock(list_lock_);
                // �Ƴ�|list|, ���������. �������۲�����һ��֪ͨ��Ϣ��ͬʱ�Ƴ�
                // ʱ�ᷢ���������. ��http://crbug.com/55725.
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

    base::Lock list_lock_; // ����observer_lists_.
    ObserversListMap observer_lists_;
    const NotificationType type_;

    DISALLOW_COPY_AND_ASSIGN(ObserverListThreadSafe);
};

#endif //__observer_list_threadsafe_h__