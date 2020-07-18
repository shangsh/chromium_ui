
#ifndef __observer_list_h__
#define __observer_list_h__

#pragma once

#include <algorithm>
#include <limits>
#include <vector>

#include "base/logging.h"

///////////////////////////////////////////////////////////////////////////////
//
// ����:
//
//   ���ɹ۲��ߵ�����. ����һ���STL������б�, �����ڱ�����ʱ���޸�, ���ᵼ��
//   �������Ƿ�. ����, �۲����ڽӵ�֪ͨ��Ϣ��ʱ����Լ����б����Ƴ��ǰ�ȫ��.
//
// ���÷���:
//
//   class MyWidget {
//    public:
//     ...
//
//     class Observer {
//      public:
//       virtual void OnFoo(MyWidget* w) = 0;
//       virtual void OnBar(MyWidget* w, int x, int y) = 0;
//     };
//
//     void AddObserver(Observer* obs) {
//       observer_list_.AddObserver(obs);
//     }
//
//     void RemoveObserver(Observer* obs) {
//       observer_list_.RemoveObserver(obs);
//     }
//
//     void NotifyFoo() {
//       FOR_EACH_OBSERVER(Observer, observer_list_, OnFoo(this));
//     }
//
//     void NotifyBar(int x, int y) {
//       FOR_EACH_OBSERVER(Observer, observer_list_, OnBar(this, x, y));
//     }
//
//    private:
//     ObserverList<Observer> observer_list_;
//   };
//
///////////////////////////////////////////////////////////////////////////////

template<typename ObserverType>
class ObserverListThreadSafe;

template<class ObserverType>
class ObserverListBase
{
public:
    // �۲��߽���֪ͨ��ö������.
    enum NotificationType
    {
        // ָ������֪ͨʱ��ӵ����й۲��߶���֪ͨ. ���캯����ȱʡֵ.
        NOTIFY_ALL,

        // ָ������֪ͨʱ��ӵĹ۲��߲��ٱ�֪ͨ.
        NOTIFY_EXISTING_ONLY
    };

    // ��������, ���ڷ��ʹ۲����б�. �μ����涨��ĵ�FOR_EACH_OBSERVER��.
    class Iterator
    {
    public:
        Iterator(ObserverListBase<ObserverType>& list)
            : list_(list), index_(0), max_index_(list.type_==NOTIFY_ALL ?
            std::numeric_limits<size_t>::max() : list.observers_.size())
        {
            ++list_.notify_depth_;
        }

        ~Iterator()
        {
            if(--list_.notify_depth_ == 0)
            {
                list_.Compact();
            }
        }

        ObserverType* GetNext()
        {
            ListType& observers = list_.observers_;
            // �����ǰԪ��Ϊ��, ������ǰ.
            size_t max_index = __min(max_index_, observers.size());
            while(index_ < max_index && !observers[index_])
            {
                ++index_;
            }
            return index_<max_index ? observers[index_++] : NULL;
        }

    private:
        ObserverListBase<ObserverType>& list_;
        size_t index_;
        size_t max_index_;
    };

    ObserverListBase() : notify_depth_(0), type_(NOTIFY_ALL) {}
    explicit ObserverListBase(NotificationType type)
        : notify_depth_(0), type_(type) {}

    // ��ӹ۲��ߵ��б�.
    void AddObserver(ObserverType* obs)
    {
        DCHECK(find(observers_.begin(), observers_.end(), obs) == observers_.end())
            << "Observers can only be added once!";
        observers_.push_back(obs);
    }

    // �Ƴ��۲��ߵ��б�.
    void RemoveObserver(ObserverType* obs)
    {
        typename ListType::iterator it =
            std::find(observers_.begin(), observers_.end(), obs);
        if(it != observers_.end())
        {
            if(notify_depth_)
            {
                *it = 0;
            }
            else
            {
                observers_.erase(it);
            }
        }
    }

    bool HasObserver(ObserverType* observer) const
    {
        for(size_t i=0; i < observers_.size(); ++i)
        {
            if(observers_[i] == observer)
            {
                return true;
            }
        }
        return false;
    }

    void Clear()
    {
        if(notify_depth_)
        {
            for(typename ListType::iterator it=observers_.begin();
                it!=observers_.end(); ++it)
            {
                *it = 0;
            }
        }
        else
        {
            observers_.clear();
        }
    }

    size_t size() const { return observers_.size(); }

protected:
    void Compact()
    {
        typename ListType::iterator it = observers_.begin();
        while(it != observers_.end())
        {
            if(*it)
            {
                ++it;
            }
            else
            {
                it = observers_.erase(it);
            }
        }
    }

private:
    friend class ObserverListThreadSafe<ObserverType>;

    typedef std::vector<ObserverType*> ListType;

    ListType observers_;
    int notify_depth_;
    NotificationType type_;

    friend class ObserverListBase::Iterator;

    DISALLOW_COPY_AND_ASSIGN(ObserverListBase);
};

template<class ObserverType, bool check_empty=false>
class ObserverList : public ObserverListBase<ObserverType>
{
public:
    typedef typename ObserverListBase<ObserverType>::NotificationType
        NotificationType;

    ObserverList() {}
    explicit ObserverList(NotificationType type)
        : ObserverListBase<ObserverType>(type) {}

    ~ObserverList()
    {
        // ��check_emptyΪtrue, ����ʱ�����б�Ϊ��.
        if(check_empty)
        {
            ObserverListBase<ObserverType>::Compact();
            DCHECK_EQ(ObserverListBase<ObserverType>::size(), 0);
        }
    }
};

#define FOR_EACH_OBSERVER(ObserverType, observer_list, func) \
    do{ \
      ObserverListBase<ObserverType>::Iterator it(observer_list); \
      ObserverType* obs; \
      while((obs=it.GetNext()) != NULL) \
        obs->func; \
    } while(0)

#endif //__observer_list_h__