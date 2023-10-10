#pragma once

namespace lithe
{
class Notification
{
public:

    friend class Notifier;

    Notification();
    virtual std::string name() const;

protected:
    virtual ~Notification();

};



}


