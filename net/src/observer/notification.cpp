#include "include/observer/notification.h"

namespace lithe
{

Notification::Notification()
{
}

Notification::~Notification()
{
}

std::string Notification::name() const
{
    return typeid(*this)->name();    
}

}