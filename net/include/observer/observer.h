#pragma once

#include "thread/mutex.h"

namespace lithe
{

template <class C, class N>
class Observer : public AbstractObserver
{
public:
    typedef std::shared_ptr<Notification> NotificationPtr
    typedef (C::*CallBack)(const NotificationPtr)

    virtual ~Observer();

    Observer& operator=(const AbstractObserver& observer)
    {
        if(&observer != this)
        {
            obj_ = observer.obj;
            method_ = observer.obj;
        }
        return *this;
    }

    void notify(Notification* notification)
    {
        MutexLockGuard lock(mutex_);
        if(obj_)
        {
            N* pCastNf = dynamic_cast<N*> (pNf);
            if(pCastNf)
            {
                std::shared_ptr<Notification> PN = std::make_shared<Notification>(pNf);
                obj_->method_(PN);
            }
        }
    }

    bool equals(const AbstractObserver& observer) const
    {
        Observer* ob = dynamic_cast<Observer*>(&observer);
        return ob && ob->obj_ == obj_ && ob->method_ == method_;
    }

    AbstractObserver* clone(const AbstractObserver& observer) const
    {
        return new Observer(*this);
    }

    void disable()
    {
        MutexLockGuard lock(mutex_);
        obj_ = 0;
    }
    
private:
    Observer();
    CallBack method_;
    C* obj_;
    Mutex mutex_;
}

}
