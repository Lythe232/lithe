#pragma once


#include <vector>
namespace lithe
{

class NotificationCenter
{
public:
    NotificationCenter();
    ~NotificationCenter();

    void addObserver(AbstractObserver& observer);
    void removeObserver(AbstractObserver& observer);

    bool hasObserver();

    void postNotification();

    void count() const;


private:
    typedef std::vector<std::shared_ptr<AbstractObserver>> ObserverList
    ObserverList observers_;

    Mutex mutex_;

};


}