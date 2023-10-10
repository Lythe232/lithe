#include "include/addressImpl.h"
#include <string.h>
#include <assert.h>
namespace lithe
{
lithe::IPv4AddressImpl::IPv4AddressImpl() : 
    addr_((sockaddr_in*)malloc(sizeof(sockaddr_in)))
{
    memset(addr_, 0, sizeof(sockaddr_in));
}
lithe::IPv4AddressImpl::IPv4AddressImpl(const void *addr, in_port_t port)  : 
    addr_((sockaddr_in*)malloc(sizeof(sockaddr_in)))
{
    memset(addr_, 0, sizeof(sockaddr_in));
    addr_->sin_addr.s_addr = inet_addr((const char*)addr);
    addr_->sin_port = htons(port);
    addr_->sin_family = AF_INET;
}
IPv4AddressImpl::IPv4AddressImpl(const IPv4AddressImpl &addr)  : 
    addr_((sockaddr_in*)malloc(sizeof(sockaddr_in)))
{
    //TODO
    assert(0);
}
IPv4AddressImpl &lithe::IPv4AddressImpl::operator=(const IPv4AddressImpl &addr)
{
    if(&addr == this)
    {
        return *this;
    }
    ::memcpy(&addr_, &addr.addr_, sizeof(addr_));
    return *this;
}
IPv4AddressImpl::~IPv4AddressImpl()
{
    if(addr_)    
    {
        free(addr_);
        addr_ = nullptr;
    }
}
std::string IPv4AddressImpl::toString() const
{
    //TODO
    char ip[INET_ADDRSTRLEN];
    memset(ip, 0, INET_ADDRSTRLEN);
    assert(addr_);
    if(inet_ntop(AF_INET, &addr_->sin_addr, ip, INET_ADDRSTRLEN) == nullptr)
    {
        printf("IPv4AddressImpl::toString() fails. errno[%d], strerror=%s\n", errno, strerror(errno));
        assert(0);
    }
    return std::string(ip);
}

const void *IPv4AddressImpl::addr() const
{
    return &addr_->sin_addr.s_addr;
}

sa_family_t IPv4AddressImpl::getFamily() const
{
    return addr_->sin_family;
}

bool IPv4AddressImpl::isBroadcast() const
{
    return addr_->sin_addr.s_addr == 0xFFFFFFFF;
}
bool IPv4AddressImpl::isLoopback() const
{                                                //127.0.0.1 ~ 127.255.255.255
    return (htonl(addr_->sin_addr.s_addr) & 0xFF000000) == 0x7F000000;    
}
bool IPv4AddressImpl::isMulticast() const
{                                               //224.0.0.1 ~ 239.0.0.0
    return (htonl(addr_->sin_addr.s_addr) & 0xF0000000) == 0xE0000000;
}
bool IPv4AddressImpl::isLinkLocal() const
{
	return (ntohl(addr_->sin_addr.s_addr) & 0xFFFF0000) == 0xA9FE0000; // 169.254.0.0/16
}
bool IPv4AddressImpl::isSiteLocal() const
{
	uint32_t addr = ntohl(addr_->sin_addr.s_addr);
	return (addr & 0xFF000000) == 0x0A000000 ||     // 10.0.0.0/24
		(addr & 0xFFFF0000) == 0xC0A80000 ||        // 192.68.0.0/16
		(addr >= 0xAC100000 && addr <= 0xAC1FFFFF); // 172.16.0.0 to 172.31.255.255
}
bool IPv4AddressImpl::isIPv4Compatible() const
{
	return true;
}
bool IPv4AddressImpl::isIPv4Mapped() const
{
	return true;
}
bool IPv4AddressImpl::isWellKnownMC() const
{
	return (ntohl(addr_->sin_addr.s_addr) & 0xFFFFFF00) == 0xE0000000; // 224.0.0.0/8
}
bool IPv4AddressImpl::isNodeLocalMC() const
{
	return false;
}
bool IPv4AddressImpl::isLinkLocalMC() const
{
	return (ntohl(addr_->sin_addr.s_addr) & 0xFF000000) == 0xE0000000; // 244.0.0.0/24
}
bool IPv4AddressImpl::isSiteLocalMC() const
{
	return (ntohl(addr_->sin_addr.s_addr) & 0xFFFF0000) == 0xEFFF0000; // 239.255.0.0/16
}
bool IPv4AddressImpl::isOrgLocalMC() const
{
	return (ntohl(addr_->sin_addr.s_addr) & 0xFFFF0000) == 0xEFC00000; // 239.192.0.0/16
}
bool IPv4AddressImpl::isGlobalMC() const
{
	uint32_t addr = ntohl(addr_->sin_addr.s_addr);
	return addr >= 0xE0000100 && addr <= 0xEE000000; // 224.0.1.0 to 238.255.255.255
}
void IPv4AddressImpl::mask(const IPAddressImpl *pMask, const IPAddressImpl *pSet)
{
    assert(pMask->getFamily() == AF_INET && pSet->getFamily() == AF_INET);

    addr_->sin_addr.s_addr &= static_cast<const IPv4AddressImpl*>(pMask)->addr_->sin_addr.s_addr;
    addr_->sin_addr.s_addr |= static_cast<const IPv4AddressImpl*>(pSet)->addr_->sin_addr.s_addr & (~static_cast<const IPv4AddressImpl*>(pMask)->addr_->sin_addr.s_addr);
}

void IPv4AddressImpl::setAddr(sockaddr_in* addr)
{
    if(addr == nullptr)
    {
        return ;
    }
    if(addr_ != nullptr)
    {
        free(addr_);
    }
    addr_ = addr;
}
sockaddr* IPv4AddressImpl::getSockaddr()
{
    return (sockaddr*)addr_;
}
int IPv4AddressImpl::getDomain()
{
    return AF_INET;
}
socklen_t IPv4AddressImpl::getSocklen() const
{
    return sizeof(sockaddr);
}
std::shared_ptr<IPv4AddressImpl> IPv4AddressImpl::parse(const std::string &addr, in_port_t port)
{
    struct in_addr ia;
    if(inet_aton(addr.c_str(), &ia))
    {
        return std::make_shared<IPv4AddressImpl>(&ia, port);
    }
    else
    {
        assert(0);
        return std::make_shared<IPv4AddressImpl>();
    }
}


// lithe::IPv6AddressImpl::IPv6AddressImpl()
// {
//     ::memcpy(&addr_, 0, sizeof(addr_));
// }

// lithe::IPv6AddressImpl::IPv6AddressImpl(const void *addr)
// {
//     ::memcpy(&addr_, addr, sizeof(addr_));
// }

// IPv6AddressImpl &lithe::IPv6AddressImpl::operator=(const IPv6AddressImpl & addr)
// {
//     if(&addr == this)
//     {
//         return *this;
//     }
//     ::memcpy(&addr_, &addr.addr_, sizeof(addr_));
//     return *this;
// }
// std::string IPv6AddressImpl::toString() const
// {

// }
// __socklen_t IPv6AddressImpl::getSocklen() const
// {
//     return sizeof(addr_);
// }
// const void *IPv6AddressImpl::addr() const
// {
//     return &addr_;
// }
// IPAddressImpl::Family IPv6AddressImpl::getFamily() const
// {
//     return IPAddressImpl::IPv6;
// }
// bool IPv6AddressImpl::isBroadcast() const
// {
//     return false;
// }
// bool IPv6AddressImpl::isLoopback() const
// {                                                //127.0.0.1 ~ 127.255.255.255

// }
// bool IPv6AddressImpl::isMulticast() const
// {                                               //224.0.0.1 ~ 239.0.0.0
// }
// bool IPv6AddressImpl::isLinkLocal() const
// {
// }
// bool IPv6AddressImpl::isSiteLocal() const
// {

// }
// bool IPv6AddressImpl::isIPv4Compatible() const
// {
// 	return true;
// }
// bool IPv6AddressImpl::isIPv4Mapped() const
// {
// 	return true;
// }
// bool IPv6AddressImpl::isWellKnownMC() const
// {
// }
// bool IPv6AddressImpl::isNodeLocalMC() const
// {
// 	return false;
// }
// bool IPv6AddressImpl::isLinkLocalMC() const
// {
// }
// bool IPv6AddressImpl::isSiteLocalMC() const
// {
// }
// bool IPv6AddressImpl::isOrgLocalMC() const
// {
// }
// bool IPv6AddressImpl::isGlobalMC() const
// {
// }
// void IPv6AddressImpl::mask(const IPAddressImpl *pMask, const IPAddressImpl *pSet)
// {
// }
// IPv6AddressImpl IPv6AddressImpl::parse(const std::string &addr)
// {
// }



} // namespace lithe
