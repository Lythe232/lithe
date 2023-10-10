#include "include/socket.h"
#include "include/log/log.h"
#include "include/fdmanager.h"
#include "include/hookSysCall.h"

#include <fcntl.h>
namespace lithe
{

static auto g_logger = LOG_ROOT();

Socket::Socket() : 
    socket_(-1),
    family_(0),
    type_(0),
    protocol_(0),
    isConnected_(false),
    localAddress_(nullptr),
    remoteAddress_(nullptr)
{
}

Socket::Socket(int domain, int type, int protocol) : 
    socket_(-1),
    family_(domain),
    type_(type),
    protocol_(protocol),
    isConnected_(false),
    localAddress_(nullptr),
    remoteAddress_(nullptr)
{
    initSock();
}
Socket::Socket(int fd, int domain, int type, int protocol) : 
    socket_(fd),
    family_(domain),
    type_(type),
    protocol_(protocol),
    isConnected_(false),
    localAddress_(nullptr),
    remoteAddress_(nullptr)
{
    init(socket_);
}

Socket::~Socket()
{
    close();
}
void lithe::Socket::initSock()
{
    socket_ = socket(family_, type_, protocol_);
    fd_ = lithe::FdMgr::getSingletonPtr()->get(socket_);
    if(socket_ != -1)
    {
        if(type_ == SOCK_STREAM)
        {
            setTcpNoDelay(true);
        }
    }
    else
    {
        LOG_ERROR(g_logger) << "socket(" << family_ << ", " << type_ << ", " 
            << protocol_ << ") errno[" << errno << "], errstr: " << strerror(errno);
    }
}
bool Socket::init(int socket)
{
    if(socket > 0)
    {
        if(type_ == SOCK_STREAM)
        {
            setTcpNoDelay(true);
        }
        auto newfd = FdMgr::getSingletonPtr()->get(socket);
        if(newfd && newfd->isSocket() && !newfd->isClosed())
        {
            socket_ = socket;
            fd_ = newfd;
            isConnected_ = true;
            getLocalAddress();
            // getRemoteAddress();
            return true;
        }
    }
    LOG_ERROR(g_logger) << "socket init fails. socket[" << socket << "].";
    assert(0);
    return false;
}
bool lithe::Socket::getOption(int level, int optname, void* optval, socklen_t* optlen)
{
    if(::getsockopt(socket_, level, optname, optval, optlen) == -1)
    {
        LOG_ERROR(g_logger) << "setsockopt(" << socket_ << ", " << optname << ", " << optval << ", " 
                        << optlen << ") errno = " << errno << "errstr = " << strerror(errno);
        return false;
    }
    return true;
}

bool lithe::Socket::setOption(int level, int optname, const void *optval, socklen_t optlen)
{
    if(::setsockopt(socket_, level, optname, optval, optlen) == -1)
    {
        LOG_FATAL(g_logger) << "setsockopt(" << socket_ << ", " << optname << ", " << optval << ", " 
                                << optlen << ") errno[" << errno << "], errstr: " << strerror(errno);
        return false;
    }
    return true;
}

std::shared_ptr<Socket> lithe::Socket::createTCP()
{
    std::shared_ptr<Socket> newSock(new Socket(IPv4, TCP, 0));
    return newSock;
}

std::shared_ptr<Socket> lithe::Socket::createUDP()
{
    std::shared_ptr<Socket> newSock(new Socket(IPv4, UDP, 0));
    return newSock;
}

bool Socket::close()
{
    if(socket_ == -1 && !isConnected_)
    {
        return false;
    }
    isConnected_ = false;
    if(socket_ != -1)
    {   
        ::close(socket_);
        socket_ = -1;
    }
    return true;
}

bool Socket::shutdownOnWrite()
{
    if(socket_ == -1 && !isConnected_)
    {
        return false;
    }
    isConnected_ = false;
    if(socket_ != -1)
    {
        ::shutdown(socket_, SHUT_WR);
        socket_ = -1;
    }
    return true;
}

bool Socket::bind(const std::shared_ptr<IPAddress> addr)
{
    if(socket_ == -1)
    {
        initSock();
    }
    if(!addr)
    {
        assert(0);
    }
    if(addr->getFamily() != family_)
    {
        LOG_ERROR(g_logger) << "bind(" << " domain not equal.";
        return false;
    }
    if(::bind(socket_, addr->getSockaddr(), addr->getSocklen()) == -1)
    {
        LOG_ERROR(g_logger) << "Socket::bind() error. errno[" << errno << "].";
        return false;
    }

    return true;
}

bool Socket::bind(const sockaddr *address)
{
    if(socket_ == -1)
    {
        initSock();
    }
    if(address == nullptr)
    {
        assert(0);
    }
    if(address->sa_family != family_)
    {
        LOG_ERROR(g_logger) << "bind(" << " domain not equal.";
        return false;
    }
    if(::bind(socket_, address, sizeof(sockaddr)) == -1)
    {
        LOG_ERROR(g_logger) << "Socket::bind() error. errno[" << errno << "].";
        return false;
    }

    getLocalAddress();
    assert(localAddress_);
    return true;
}

bool Socket::listen(int backlog)
{
    if(socket_ == -1)
    {
        LOG_ERROR(g_logger) << "listen(" << " socket_ is uninitialised value";
        return false;
    }
    if(::listen(socket_, backlog) < 0)
    {
        LOG_ERROR(g_logger) << "Socket::listen(" << socket_ << ", " << backlog << ") error, errno[" << errno << "].";
        return false;
    }
    return true;
}

std::shared_ptr<Socket> Socket::accept(std::shared_ptr<IPAddress>& peeraddr)
{
    if(socket_ == -1)
    {
        LOG_FATAL(g_logger) << "Socket::accept fails. " << " socket_ is uninitialised value.";
        return std::make_shared<Socket>();
    }
    if(!peeraddr)
    {
        peeraddr = std::make_shared<IPAddress>();
    }
    int rt = 0;
    struct sockaddr_in* addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    socklen_t len = sizeof(sockaddr);
    while(1)
    {
        if((rt = ::accept(socket_, (struct sockaddr*)addr, &len)) < 0)
        {
            if(rt == -1 && errno == EAGAIN)
            {
                continue;
            }
            LOG_FATAL(g_logger) << "Socket::accept " << "errno[" << errno << "].";
            printf("Socket::accpet errno[%d].\n", errno);
            return std::make_shared<Socket>();
        }
        break;
    }
    if(peeraddr)
    {
        peeraddr->setAddr(addr);
    }
    std::shared_ptr<Socket> newSock(new Socket(rt, family_, type_, protocol_));
    return newSock;
}

bool Socket::connect(std::shared_ptr<IPAddress> addr, uint64_t timeout)
{
    remoteAddress_ = addr;
    if(socket_ == -1)
    {
        initSock();
    }
    if(family_ != addr->getFamily())
    {
        LOG_ERROR(g_logger) << "connect(" << "family not equal!";
        return false;
    }
    if(timeout == (uint64_t)-1)
    {
        if(::connect(socket_, remoteAddress_->getSockaddr(), remoteAddress_->getSocklen()) == -1)
        {
            LOG_ERROR(g_logger) << "connect(" << socket_ << ", " 
            << remoteAddress_->getSockaddr() << ", " << remoteAddress_->getSocklen() << ") errno[" << errno << "].";
            close();
            return false;
        }
    }
    else
    {
        if(::connect_with_timeout(socket_, remoteAddress_->getSockaddr(), remoteAddress_->getSocklen(), timeout))
        {
            LOG_ERROR(g_logger) << "connect_with_timeout(" << socket_ << ", " 
                << remoteAddress_->getSockaddr() << ", " << remoteAddress_->getSocklen() 
                << ", " << timeout << "errno[" << errno << "].";
            close();
            return false;
        }
    }
    isConnected_ = true;
    getLocalAddress();
    getRemoteAddress();
    return true;
}

ssize_t Socket::send(const void *buf, size_t len, int flags)
{
    if(isConnected())
    {
        return ::send(socket_, buf, len, flags);
    }
    return -1;
}

ssize_t Socket::send(const iovec *iov, int iovcnt, int flags)
{
    if(isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msg));
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = iovcnt;
        return ::sendmsg(socket_, &msg, flags);
    }
    return -1;
}

ssize_t Socket::sendto(const void *buf, size_t len, std::shared_ptr<IPAddress> dest, int flags)
{
    if(isConnected())
    {
        return ::sendto(socket_, buf, len, flags, dest->getSockaddr(), dest->getSocklen());
    }
    return -1;
}

ssize_t Socket::sendto(const iovec *iov, int iovcnt, std::shared_ptr<IPAddress> dest, int flags)
{
    if(isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msghdr));
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = iovcnt;
        msg.msg_name = dest->getSockaddr();
        msg.msg_namelen = dest->getSocklen();
        return ::sendmsg(socket_, &msg, flags);
    }
    return -1;
}

ssize_t Socket::recv(void *buf, size_t len, int flags)
{
    if(isConnected())
    {
        return ::recv(socket_, buf, len, flags);
    }
    return -1;
}

ssize_t Socket::recv(const iovec *iov, int iovcnt, int flags)
{
    if(isConnected())
    {
        msghdr msg;
        memset(&msg, 0, iovcnt);
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = iovcnt;
        return ::recvmsg(socket_, &msg, flags);
    }
    return -1;
}

ssize_t Socket::recvfrom(void *buf, size_t len, std::shared_ptr<IPAddress> src, int flags)
{
    if(isConnected())
    {
        socklen_t addrlen = src->getSocklen();
        return ::recvfrom(socket_, buf, len, flags, src->getSockaddr(), &addrlen);
    }
    return -1;
}

ssize_t Socket::recvfrom(const iovec *iov, int iovcnt, std::shared_ptr<IPAddress> src, int flags)
{
    if(isConnected())
    {
        msghdr msg;
        memset(&msg, 0, sizeof(msghdr));
        msg.msg_iov = (iovec*)iov;
        msg.msg_iovlen = iovcnt;
        msg.msg_name = src->getSockaddr();
        msg.msg_namelen = src->getSocklen();
        return ::recvmsg(socket_, &msg, flags);
    }
    return -1;
}

ssize_t Socket::read(void *buf, size_t count)
{
    return ::read(socket_, buf, count);
}

ssize_t Socket::write(const void *buf, size_t count)
{
    return ::write(socket_, buf, count);
}

std::shared_ptr<IPAddress> Socket::getLocalAddress()
{
    if(localAddress_)
    {
        return localAddress_;
    }

    std::shared_ptr<IPAddress> result;
    switch(family_)
    {
        case AF_INET:
            result.reset(new IPAddress(AF_INET));
            break;
        case AF_INET6: 
            assert(0);
            break;
        default:
            assert(0);
            return nullptr;
    }
    socklen_t len = sizeof(sockaddr);
    if(getsockname(socket_, result->getSockaddr(), &len)  < 0)
    {
        LOG_ERROR(g_logger) << "getsockname(" << socket_ << ", " 
            << result->getSockaddr() << ", " << &len << ") errno[" << errno << "].";
        return nullptr; 
    }
    localAddress_ = result;
    return result;
}

std::shared_ptr<IPAddress> Socket::getRemoteAddress()
{
    if(remoteAddress_)
    {
        return remoteAddress_;
    }
    std::shared_ptr<IPAddress> result;
    switch(family_)
    {
        case AF_INET:
            result.reset(new IPAddress(AF_INET));
            break;
        case AF_INET6:
            assert(0);
            break;
        default:
            assert(0);
            return nullptr;
    }
    socklen_t len = sizeof(sockaddr);           ////////////////////////////////////////////////////////    
    if(getpeername(socket_, result->getSockaddr(), &len) < 0) //ERROR;
    {
        LOG_ERROR(g_logger) << "getpeername failed, socket_[" << socket_ << "]," << 
            " errno[" << errno << "].";
    }
    remoteAddress_ = result;
    return result;
}

bool Socket::setTcpNoDelay(bool flag)
{
    int op = flag ? 1 : 0;
    return setOption(IPPROTO_TCP, TCP_NODELAY, &op, (socklen_t)sizeof(op));
}

bool Socket::setReuseAddr(bool flag)
{
    int op = flag ? 1 : 0;
    return setOption(SOL_SOCKET, SO_REUSEADDR, &op, (socklen_t)sizeof(op));
}

bool Socket::setReusePort(bool flag)
{
#ifdef SO_REUSEPORT
    int op = flag ? 1 : 0;
    return setOption(SOL_SOCKET, SO_REUSEPORT, &op, (socklen_t)sizeof(op));
#else
    if(flag)
    {
        LOG_ERROR(g_logger) << "Socket::setReusePort SO_REUSEPORT is not support.";
    }
#endif
}

bool Socket::setKeepAlive(bool flag)
{
    int op = flag ? 1 : 0;
    return setOption(SOL_SOCKET, SO_KEEPALIVE, &op, (socklen_t)sizeof(op));
}

void Socket::setRecvTimeout(uint64_t to)
{
    // struct timeval tv{ int(to / 1000), int(to % 1000 * 1000) };
    // setOption(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    lithe::FdMgr::getSingletonPtr()->get(socket_)->setTimeout(SO_RCVTIMEO, to);
}

uint64_t Socket::getRecvTimeout()
{
    auto channal = FdMgr::getSingletonPtr()->get(socket_);
    if(channal)
    {
        return channal->getTimeout(SO_RCVTIMEO);
    }
    return -1;
}

void Socket::setSendTimeout(uint64_t to)
{
    // struct timeval tv{ int(to / 1000), int(to % 1000 * 1000) };
    // setOption(SOL_SOCKET, SO_SNDTIMEO, &tv,sizeof(tv));
    lithe::FdMgr::getSingletonPtr()->get(socket_)->setTimeout(SO_SNDTIMEO, to);
}

uint64_t Socket::getSendTimeout()
{
    auto channal = FdMgr::getSingletonPtr()->get(socket_);
    if(channal)
    {
        return channal->getTimeout(SO_SNDTIMEO);
    }
    return -1;
}

void Socket::setUserNonblock(bool flag)
{
    fd_->setUserNonblock(flag);
}

void Socket::setSysNonblock(bool flag)
{
    fd_->setSysNonblock(flag);
}

}   //namespace lithe
