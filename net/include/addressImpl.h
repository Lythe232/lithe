#pragma once

#include "address.h"
#include "common/noncopyable.h"

#include <memory>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
namespace lithe
{
class IPAddress;

class IPAddressImpl : public Noncopyable
{
public:

    virtual std::string toString() const = 0;
    virtual socklen_t getSocklen() const = 0;
    virtual const void* addr() const = 0;
    virtual sa_family_t getFamily() const = 0;
    virtual bool isBroadcast() const = 0;
    virtual bool isLoopback() const = 0;
    virtual bool isMulticast() const = 0;
    virtual bool isLinkLocal() const = 0;
    virtual bool isSiteLocal() const = 0;
    virtual bool isIPv4Mapped() const = 0;
    virtual bool isIPv4Compatible() const = 0;
    virtual bool isWellKnownMC() const = 0;
    virtual bool isNodeLocalMC() const = 0; 
    virtual bool isLinkLocalMC() const = 0;
    virtual bool isSiteLocalMC() const = 0;
    virtual bool isOrgLocalMC() const = 0;
    virtual bool isGlobalMC() const = 0;
    virtual void mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet = 0) = 0;
    // virtual IPAddress getNetAddress(const void* addr, in_port_t port) = 0;
    virtual sockaddr* getSockaddr() = 0;
    virtual int getDomain() = 0;
    virtual void setAddr(sockaddr_in* addr) = 0;

    virtual ~IPAddressImpl()
    {
    }
protected:
    IPAddressImpl()
    {
    }

};

class IPv4AddressImpl : public IPAddressImpl
{
public:
    IPv4AddressImpl();
    IPv4AddressImpl(const void* addr, in_port_t port);
    IPv4AddressImpl(const IPv4AddressImpl& addr);
    IPv4AddressImpl& operator=(const IPv4AddressImpl&);
    ~IPv4AddressImpl();
    std::string toString() const override;
    __socklen_t getSocklen() const override;
    const void* addr() const override;
    sa_family_t getFamily() const override;
    bool isBroadcast() const override;
    bool isLoopback() const override;
    bool isMulticast() const override;
    bool isLinkLocal() const override;
    bool isSiteLocal() const override;
    bool isIPv4Compatible() const override;
    bool isIPv4Mapped() const override;
    bool isWellKnownMC() const override;
    bool isNodeLocalMC() const override;
    bool isLinkLocalMC() const override;
    bool isSiteLocalMC() const override;
    bool isOrgLocalMC() const override;
    bool isGlobalMC() const override;
    void mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet) override;
    // IPAddress getNetAddress(const void* addr, in_port_t port)
    // {   //TODO
    // }
    void setAddr(sockaddr_in* addr) override;
    sockaddr* getSockaddr();

    int getDomain() override;
    in_port_t getPort() { return addr_->sin_port; }
    void setPort(in_port_t port) { addr_->sin_port = port; };
	static std::shared_ptr<IPv4AddressImpl> parse(const std::string& addr, in_port_t port);

private:
    struct sockaddr_in* addr_;
};

// class IPv6AddressImpl : public IPAddressImpl
// {
// public:
//     IPv6AddressImpl();
//     IPv6AddressImpl(const void* addr);
//     IPv6AddressImpl(const IPv6AddressImpl& addr);
//     IPv6AddressImpl& operator=(const IPv6AddressImpl&);

//     std::string toString() const override;
//     socklen_t getSocklen() const override;
//     const void* addr() const override;
//     Family getFamily() const override;
//     bool isBroadcast() const override;
//     bool isLoopback() const override;
//     bool isMulticast() const override;
//     bool isLinkLocal() const override;
//     bool isSiteLocal() const override;
//     bool isIPv4Compatible() const override;
//     bool isIPv4Mapped() const override;
//     bool isWellKnownMC() const override;
//     bool isNodeLocalMC() const override;
//     bool isLinkLocalMC() const override;
//     bool isSiteLocalMC() const override;
//     bool isOrgLocalMC() const override;
//     bool isGlobalMC() const override;
//     void mask(const IPAddressImpl* pMask, const IPAddressImpl* pSet) override;
//     static IPv6AddressImpl parse(const std::string& addr);
// private:
//     struct in6_addr addr_;
// };
}