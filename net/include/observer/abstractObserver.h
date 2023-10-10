#pragma once
#include "notification.h"


class AbstractObserver
{
public:
    AbstractObserver();

    ~AbstractObserver();

    virtual AbstractObserver& operator=(const AbstractObserver& observer);

    virtual void notify(Notification* notification) const = 0;

    virtual bool equals(const AbstractObserver& observer) const = 0;

    virtual AbstractObserver* clone() const = 0;

    virtual void disable() = 0;

};
