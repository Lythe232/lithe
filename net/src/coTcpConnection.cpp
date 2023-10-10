#include "include/coTcpConnection.h"

namespace lithe
{

static auto g_logger = LOG_ROOT();

CoTcpConnection::CoTcpConnection(std::shared_ptr<Socket> socket, CoScheduler* scheduler) :
    fd_(socket->getSocket()),
    state_(Connecting),
    socket_(socket),
    scheduler_(scheduler),
    poller_(scheduler_->getPoller()),
    channel_(new CoEpollItem(fd_, scheduler)),
    localAddr_(socket_->getLocalAddress()),
    remoteAddr_(socket_->getRemoteAddress()),
    recvBuffer_(new Buffer()),
    sendBuffer_(new Buffer())
{
    printf("CoTcpConnection(%d)\n", fd_);
}

CoTcpConnection::~CoTcpConnection()
{
    printf("~CoTcpConnection(%d)\n", fd_);
    poller_->removeItem(fd_);
}

ssize_t CoTcpConnection::send(const char* data, int len)
{

}

ssize_t CoTcpConnection::send(std::string& str)
{
    return send(str.c_str(), str.size());
}

ssize_t CoTcpConnection::send(std::shared_ptr<Buffer> buffer)
{
    ssize_t rsize = buffer->readableSize();
    char* data = new char[rsize];
    int rt = buffer->fetch(data, rsize);
    assert(rt == rsize);

    return send(data, rsize);
}

ssize_t CoTcpConnection::recv(char* data, int len)
{
    int nr = len;
    int ar = 0;
    char* buf = data;
    while(ar < len)
    {
        int n = ::read(fd_, buf, nr);
        if(n <= 0)
        {
            if(n == 0)
            {
                return 0;
            }
            LOG_ERROR(g_logger) << "CoTcpConnection::recv() fails. errno[" << errno << "].";
            return -1;
        }
        else
        {
            ar += n;
            buf += n;
            nr -= n;
        }
    }
    return ar;
}

void CoTcpConnection::setTcpNoDelay(bool flag)
{
    socket_->setTcpNoDelay(flag);
}



void CoTcpConnection::input()
{
    int n = 0;
    if(state_ == Connected)
    {
        while(true)
        {
            int err = 0;
            int count = 65536;
            n = recvBuffer_->recvfrom(fd_, count, &err);  
            if(n <= 0)
            {
                if(n == 0)
                {
                    destoryConnect();
                    return ;
                }
                errno = err;
                if(errno == ETIMEDOUT)
                {
                    LOG_WARN(g_logger) << "CoTcpConnection::recv fails. errno[" << errno << "].";
                }
                destoryConnect();
                return ;
            }
            if(n == count)
            {
                continue;
            }
            else
            {
                if(recvBuffer_->readableSize() < 1457)
                {
                    continue;
                }
                break;
            }
        }
    }
}

void CoTcpConnection::output()
{
    size_t len = 0;
    if(state_ == Connected)
    {
        len = sendBuffer_->readableSize();
        if(len == 0)
        {
            // LOG_WARN(g_logger ) << "CoTcpConnection::output() warn. sendBuffer_ no data.";
            return ;
        }
        int err = 0;
        int n = sendBuffer_->sendto(fd_, &err);
        if(n < 0)
        {
            errno = err;
            LOG_ERROR(g_logger) << "CoTcpConnection::send() fails. errno[" << errno << "].";
            return ;
        }
        else
        {
            LOG_INFO(g_logger) << n << "bytes. has been send to " << fd_;
            return ;
        }
    }
}

void CoTcpConnection::process()
{
    // if(recvBuffer_->readableSize() <= 1457)
    // {
    //     return ;
    // }
    if(state_ == Connected)
    {
        ssize_t rsize = recvBuffer_->readableSize();
        if(!rsize)
        {
            return ;
        }
        // if(rsize < 1457)
        // {
        //     return ;
        // }
        char* buf = new char[rsize];
        recvBuffer_->fetch(buf, rsize);

        printf("(%d)recv data[%d]: %.*s\n", fd_, rsize, rsize, buf);
        delete [] buf;

        sendBuffer_->append(recvBuffer_);
    }

}

void CoTcpConnection::establishingConnect()
{
    assert(state_ == DisConnected || state_ == Connecting);
    setState(Connected);
    poller_->addItem(channel_);

}

void CoTcpConnection::destoryConnect()
{
    assert(state_ == Connected || state_ == DisConnecting);
    setState(DisConnected);
    poller_->cancelAllEvent(fd_);
    
}

}