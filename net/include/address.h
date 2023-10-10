#pragma once

#include "addressImpl.h"

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <memory>
#include <vector>
namespace lithe
{

class Address
{
public:
    Address()
    {
    }
    virtual ~Address()
    {
    }
private:

};
class IPAddressImpl;
class IPAddress  : Address
{
public:
    IPAddress();
    IPAddress(int family);
    IPAddress(int family, const char* addr, in_port_t port);
    virtual ~IPAddress();
    std::string toString();
    //判断是否是广播地址
    bool isBroadcast() const;   
    //判断是否是回环地址
    bool isLoopback() const;
    //判断是否是广播地址
    bool isMulticast() const;
    //判断是否是本地链路地址
    bool isLinkLocal() const;
    //判断是否是站点本地地址(如一个企业或组织)内有效的 IP 地址
    bool isSiteLocal() const;
    //判断是否是IPV4兼容地址
    bool isIPv4Compatible() const;
    //判断是否是IPV4映射地址 IPv4 映射地址是指在 IPv6 网络中使用的特殊 IPv6 地址，用于将 IPv4 地址映射到 IPv6 地址。
    bool isIPv4Mapped() const;
    //判断是否是知名的多播地址  知名多播地址是指预定义的多播地址，用于特定的目的，如 NTP、SNMP 等。
    bool isWellKnownMC() const;
    //判断是否是节点本地多播地址 节点本地多播地址是指仅在同一主机上有效的多播地址。
    bool isNodeLocalMC() const;
    //判断是否是链路本地多播地址 链路本地多播地址是指仅在同一链路上有效的多播地址。
    bool isLinkLocalMC() const;
    //判断是否是站点本地多播地址 站点本地多播地址是指仅在同一站点上有效的多播地址。
    bool isSiteLocalMC() const;
    //判断是否为组织本地多播地址 组织本地多播地址是指仅在同一组织内有效的多播地址。
    bool isOrgLocalMC() const;
    //判断是否为全局多播地址 全局多播地址是指在 Internet 上有效的多播地址。
    bool isGlobalMC() const;
    socklen_t getSocklen();
    sa_family_t getFamily();
    int getDomain();
    // IPAddress getNetAddress(const void* addr, in_port_t port);
    const void* addr();
    sockaddr* getSockaddr();
    void setAddr(sockaddr_in* addr);
    //TODO
    void mask(const IPAddress& mask);
    void mask(const IPAddress& mask, const IPAddress& set);
    //TODO
    bool isIPv4() const;
    bool isIPv6() const;
    IPAddress newIPv4Address();
    IPAddress newIPv6Address();

    // static std::shared_ptr<IPAddress> parse(const std::string& addr, in_port_t port);

    //TODO getInterfaceAddress, Lookup

private:
    IPAddressImpl* pImpl_;
};


// class UNIXAddress : Address
// {
// public:

// };



}   //lithe