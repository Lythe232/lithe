#include "include/observer/socketNotifier.h"


namespace lithe
{

SocketNotifier::SocketNotifier(std::shared_ptr<Socket> socket) :
    socket_(socket),
    nc_(new NotificationCenter())
{
}

SocketNotifier::~SocketNotifier()
{
}

SocketNotifier::addObserver(AbstractObserver& observer)
{
    nc_->addObserver(observer);
}

SocketNotifier::removeObserver(AbstractObserver& observer)
{
    nc_->removeObserver(observer);
}

}