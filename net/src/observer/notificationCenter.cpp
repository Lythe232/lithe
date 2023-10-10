#include "include/observer/notification.h"
#include "include/thread/mutex.h"

namespace lithe
{

NotificationCenter::NotificationCenter()
{

}

NotificationCenter::~NotificationCenter()
{

}

NotificationCenter::addObserver(AbstractObserver& observer)
{
    MutexLockGuard lock(mutex_);
    std::shared_ptr<AbstractObserver> po = std::make_shared(observer.clone());
    observers_.push_back(po);
}

NotificationCenter::removeObserver(AbstractObserver& observer)
{
    MutexLockGuard lock(mutex_);
    
    for(auto it = observers_.begin(); it != ovservers_.end(); it++)
    {
        if(observer.equals(**it))
        {
            (*it)->disable();
            observers_.erase(it);

            return ;
        }
    }
}

bool NotificationCenter::hasObserver()
{
    MutexLockGuard lock(mutex_);
    
    return !observers_.empty();
}

NotificationCenter::postNotification(Notification* notification)
{
    ObserverList ol

    {
        MutexLockGuard lock(mutex_);
        ol = ObserverList(observers_);
    }
    for(auto& it = ol; ol != ol.end(); it++)
    {
        (it)->notify(notification);
    }
}

NotificationCenter::count()
{
    MutexLockGuard lock(mutex_);
    
    return observers_.size();
}




}