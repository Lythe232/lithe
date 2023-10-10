#pragma once

#include "address.h"
#include "fdmanager.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>

namespace lithe
{

class Socket
{
public:
    enum TYPE
    {
        TCP = SOCK_STREAM,
        UDP = SOCK_DGRAM
    };
    enum FAMILY
    {
        IPv4 = AF_INET,
        IPv6 = AF_INET6,
        UNIX = AF_UNIX
    };
    Socket();
    Socket(int domain, int type, int protocol);
    Socket(int fd, int domain, int type, int protocol);
    ~Socket();

    void initSock();
    bool getOption(int level, int optname, void* optval, socklen_t* optlen);
    bool setOption(int level, int optname, const void* optval, socklen_t optlen);
    static std::shared_ptr<Socket> createTCP();
    static std::shared_ptr<Socket> createUDP();

    bool close();
    bool shutdownOnWrite();
    bool bind(const std::shared_ptr<IPAddress> address);
    bool bind(const sockaddr* address);
    bool listen(int backlog);
    std::shared_ptr<Socket> accept(std::shared_ptr<IPAddress>& peeraddr);
    bool connect(std::shared_ptr<IPAddress> addr, uint64_t timeout);

    ssize_t send(const void* buf, size_t len, int flags);
    ssize_t send(const iovec* iov, int iovcnt, int flags);

    ssize_t sendto(const void* buf, size_t len, std::shared_ptr<IPAddress> dest, int flags);
    ssize_t sendto(const iovec* iov, int iovcnt, std::shared_ptr<IPAddress> dest, int flags);

    ssize_t recv(void* buf, size_t len, int flags);
    ssize_t recv(const iovec* iov, int iovcnt, int flags);

    ssize_t recvfrom(void* buf, size_t len, std::shared_ptr<IPAddress> src, int flags);
    ssize_t recvfrom(const iovec* iov, int iovcnt, std::shared_ptr<IPAddress> src, int flags);

    ssize_t read(void* buf, size_t count);
    ssize_t write(const void* buf, size_t count);

    std::shared_ptr<IPAddress> getLocalAddress();
    std::shared_ptr<IPAddress> getRemoteAddress();

    int getSocket() { return socket_; }
    sa_family_t getFamily() { return family_; }
    int getType() { return type_; }
    int getProtocol() { return protocol_; }


    bool isConnected() { return isConnected_; }

    bool setTcpNoDelay(bool flag);
    bool setReuseAddr(bool flag);
    bool setReusePort(bool flag);
    bool setKeepAlive(bool flag);

    void setRecvTimeout(uint64_t to);
    uint64_t getRecvTimeout();
    void setSendTimeout(uint64_t st);
    uint64_t getSendTimeout();
    void setUserNonblock(bool flag);
    void setSysNonblock(bool flag);
private:
    bool init(int socket);

private:
    int socket_;
    sa_family_t family_;
    int type_;
    int protocol_;
    bool isConnected_;
    static socklen_t socklen_;

    std::shared_ptr<lithe::Fd> fd_;

    std::shared_ptr<IPAddress> localAddress_;
    std::shared_ptr<IPAddress> remoteAddress_;
};

};
