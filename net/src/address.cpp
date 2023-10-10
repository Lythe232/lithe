#include "include/address.h"
#include <assert.h>

namespace lithe
{

IPAddress::IPAddress() : 
    pImpl_(new IPv4AddressImpl())
{
}

IPAddress::IPAddress(int family) : 
    pImpl_(family == AF_INET ? new IPv4AddressImpl() : nullptr)
{
}

IPAddress::IPAddress(int family, const char *addr, in_port_t port) : 
    pImpl_(family == AF_INET ? new IPv4AddressImpl(addr, port) : nullptr)
{
}

IPAddress::~IPAddress()
{
    if(pImpl_)
    {
        delete pImpl_;
        pImpl_ = nullptr;
    }
}

bool IPAddress::isBroadcast() const
{
    return pImpl_->isBroadcast();
}

bool IPAddress::isLoopback() const
{
    return pImpl_->isLoopback();
}
bool IPAddress::isMulticast() const
{
    return isMulticast();
}
bool IPAddress::isLinkLocal() const
{
    return pImpl_->isLinkLocal();
}
bool IPAddress::isSiteLocal() const
{
    return pImpl_->isSiteLocal();
}
bool IPAddress::isIPv4Compatible() const
{
    return pImpl_->isIPv4Compatible();
}
bool IPAddress::isIPv4Mapped() const
{
    return pImpl_->isIPv4Mapped();
}
bool IPAddress::isWellKnownMC() const
{
    return pImpl_->isWellKnownMC();
}
bool IPAddress::isNodeLocalMC() const
{
    return pImpl_->isNodeLocalMC();
}
bool IPAddress::isLinkLocalMC() const
{
    return pImpl_->isLinkLocalMC();
}
bool IPAddress::isSiteLocalMC() const
{
    return pImpl_->isSiteLocalMC();
}
bool IPAddress::isOrgLocalMC() const
{
    return pImpl_->isOrgLocalMC();
}
bool IPAddress::isGlobalMC() const
{
    return pImpl_->isGlobalMC();
}

socklen_t IPAddress::getSocklen()
{
    return pImpl_->getSocklen();
}

sa_family_t IPAddress::getFamily()
{
    return pImpl_->getFamily();
}

int IPAddress::getDomain()
{
    return pImpl_->getDomain();
}

// std::shared_ptr<IPAddress> IPAddress::parse(const std::string& addr, in_port_t port)
// {
//     return pImpl_->parse(addr, port);
// }

bool IPAddress::isIPv4() const
{
    return true;
}
bool IPAddress::isIPv6() const
{
    return false;
}

const void* IPAddress::addr()
{
    return pImpl_->addr();
}

sockaddr *IPAddress::getSockaddr()
{
    return pImpl_->getSockaddr();
}

std::string IPAddress::toString()
{
    return pImpl_->toString();
}
void IPAddress::setAddr(sockaddr_in* addr)
{
    pImpl_->setAddr(addr);
}

}
