#include "include/tcpConnection.h"
#include "include/hookSysCall.h"
#include "include/fdmanager.h"
#include "include/eventLoop.h"
#include <fcntl.h>
namespace lithe
{

static auto g_logger = LOG_ROOT();

TcpConnection::TcpConnection(EventLoop* loop, int sockfd, std::shared_ptr<IPAddress> localAddr, std::shared_ptr<IPAddress> peerAddr_) :
    loop_(loop), 
    state_(Connecting),
    localAddr_(localAddr),
    peerAddr_(peerAddr_),
    socket_(new Socket(sockfd, AF_INET, SOCK_STREAM, 0)), 
    channel_(new Channel(sockfd, loop)),
    inputBuf_(new Buffer()),
    outputBuf_(new Buffer()),
    highWaterMark_(64 * 1024 * 1024),
    recvTimeout_(0),
    sendTimeout_(0)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}
TcpConnection::TcpConnection(EventLoop* loop, std::shared_ptr<Socket> socket, std::shared_ptr<IPAddress> peerAddr) : 
    loop_(loop), 
    state_(Connecting),
    peerAddr_(peerAddr),
    socket_(socket), 
    channel_(new Channel(socket->getSocket(), loop)),
    inputBuf_(new Buffer()),
    outputBuf_(new Buffer()),
    highWaterMark_(64 * 1024 * 1024),
    recvTimeout_(0),
    sendTimeout_(0)
{
    channel_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
}
TcpConnection::~TcpConnection()
{
}

void TcpConnection::sendInLoop(const char* data, size_t len)
{
    size_t n = 0;
    size_t nw = 0;
    if(!channel_->isWriting() && outputBuf_->readableSize() == 0)
    {
        // n = socket_->write(data, len);
        n = ::write(channel_->fd(), data, len);
        nw = len - n;
        if(n >= 0)
        {
            if(nw == 0 && writeCompleteCallback_)
            {
                loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                return;
            }
        }
        else
        {
            n = 0;
            if(errno != EAGAIN)
            {
                LOG_ERROR(g_logger) << "TcpConnection::send() error happened. errno[" << errno << "].";
            }
            assert(0);
        }
    }
    if(nw > 0)
    {
        outputBuf_->append(data + n, nw);
        if(!channel_->isWriting())
        {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::sendInLoop(const std::string& str)
{
    sendInLoop(str.c_str(), str.size());
}

void TcpConnection::send(const std::string& str)
{
    if(state_ == Connected)
    {
        if(loop_->isInLoopThread())
        {
            sendInLoop(str);
        }
        else
        {
            std::string newStr(str);
            loop_->runInLoop(std::bind((void(TcpConnection::*)(const std::string& str))&TcpConnection::sendInLoop, this, newStr));
        }
    }
}

void TcpConnection::send(const char* data, size_t len)
{
    if(state_ == Connected)
    {

        if(loop_->isInLoopThread())
        {
            sendInLoop(data, len);
        }
        else
        {
            std::string str(data, len);
            loop_->runInLoop(std::bind((void(TcpConnection::*)(const std::string& str))&TcpConnection::sendInLoop, this, str));
        }
    }
}

void TcpConnection::send(std::shared_ptr<Buffer> buffer)
{
    if(state_ == Connected)
    {
        std::string buf = buffer->fetchAllAsString();
        send(buf);
    }
}

void TcpConnection::shutdown()
{
    if(state_ == Connected)
    {
        setState(Disconnecting);
    }
    if(!channel_->isWriting())
    {
        socket_->shutdownOnWrite();
    }
}

void TcpConnection::forceClose()
{
    //TODO
    // if(state_ == Connected || state_ == Connecting)
    // {
    //     setState(Disconnecting);
    //     loop_->queueInLoop(std::bind(&TcpConnection::handleClose, this));
    // }
}

void TcpConnection::connectEstablished()
{
    assert(state_ == Connecting);
    setState(Connected);
    channel_->enableReading();
    connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if(state_ == Connected)
    {
        setState(Disconnected);
        channel_->disableAllEvent();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::handleRead()
{
    int savedError;
    ssize_t n = inputBuf_->readFd(channel_->fd(), &savedError);

    if(n <= 0)
    {
        if(n == 0)
        {
            handleClose();
        }
        else
        {
            errno = savedError;
            if(errno == ETIMEDOUT)
            {
                LOG_WARN(g_logger) << "TcpConnection::handleRead() timeout. errno[" << errno << "], strerror=" << strerror(errno);
                return ;
            }
            else
            {
                LOG_ERROR(g_logger) << "TcpConnection::handleRead() fails. errno[" << errno << "], strerror=" << strerror(errno);
                return ;
            }
        }
    }
    else
    {
        messageCallback_(shared_from_this(), inputBuf_, Timestamp::now());
    }

}
void TcpConnection::handleWrite()
{
    // printf("socket[%d] handleWrite()\n", socket_->getSocket());

    if(channel_->isWriting())
    {
        //TODO 使用了中间缓冲层效率低，需要修改Buffer类
        size_t readableSize = outputBuf_->readableSize();
        char buf[readableSize];
        ssize_t r = outputBuf_->fetch(buf, readableSize);
        if(r < 0)
        {
            if(errno != EWOULDBLOCK)
            {
                LOG_ERROR(g_logger) << "TcpConnection::handleWrite() fails.";
                channel_->disableWriting();
                return;
            }
        }
        ssize_t n = ::write(channel_->fd(), buf, r);
        if(n > 0)
        {
            if(outputBuf_->readableSize() == 0)
            {
                channel_->disableWriting();
                if(writeCompleteCallback_)
                {
                    loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
                }
                if(state_ == Disconnecting)
                {
                    loop_->queueInLoop(std::bind(&TcpConnection::shutdown, this));
                }
            }
            else
            {
                LOG_INFO(g_logger) << "write more data.";
            }
        }
    }
    else
    {
        LOG_INFO(g_logger) << ".";
    }
}
void TcpConnection::handleClose()
{
    loop_->assertNotInLoopThread();
    // printf("handleClose() socket = %d, state_ = %d, channel->events() = %d, currentID = %d\n", socket_->getSocket(), state_, channel_->getEvents(), CurrentThread::cacheTid());
    
    assert(state_ == Connected || state_ == Disconnecting);

    setState(Disconnected);
    channel_->disableAllEvent();
    connectionCallback_(shared_from_this());
    closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
    int optval;
    socklen_t len = sizeof(optval);
    if(socket_->getOption(SOL_SOCKET, SO_ERROR, &optval, &len))
    {
        LOG_ERROR(g_logger) << "sockfd[" << socket_->getSocket() << "], errno[" << optval << "], strerror = " << strerror(optval);
    }
}

}