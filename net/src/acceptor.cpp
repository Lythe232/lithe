#include "include/acceptor.h"
#include "include/channel.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "include/hookSysCall.h"
#include <unistd.h>

namespace lithe
{

Acceptor::Acceptor(EventLoop* loop, std::shared_ptr<IPAddress> listenAddr) : 
    loop_(loop),
    listenAddr_(listenAddr),
    listenSocket_(new Socket(AF_INET, SOCK_STREAM, 0)),
    listenChannel_(new Channel(listenSocket_->getSocket(), loop_)),
    newConnectionCallback_(),
    idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
    listenning_(false)
{
        
    listenSocket_->setReuseAddr(true);
    listenSocket_->setReusePort(true);
    listenSocket_->bind(listenAddr_);
    listenChannel_->setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    listenChannel_->disableAllEvent();
    listenChannel_->remove();
    if(listenChannel_)
    {
        delete listenChannel_;
        listenChannel_ = nullptr;
    }
}

void Acceptor::listen()
{
    loop_->assertNotInLoopThread();
    listenning_ = true;
    listenSocket_->listen(5);
    listenChannel_->enableReading();

}

void Acceptor::handleRead()
{
    loop_->assertNotInLoopThread();
    std::shared_ptr<IPAddress> peerAddr;
    auto acceptSocket = listenSocket_->accept(peerAddr);
    int fd = acceptSocket->getSocket();
    int flags;
    socklen_t len = sizeof(flags);
    getsockopt(fd, SOL_SOCKET, SO_TYPE, &flags, &len);
    flags |= O_NONBLOCK;
    ::fcntl(fd, F_SETFL, flags);

    if(fd != -1)
    {
        if(newConnectionCallback_)
        {
            newConnectionCallback_(acceptSocket, peerAddr);   
        }
        else
        {
            acceptSocket->close();
        }
    }
    else
    {
        if(errno == EMFILE)
        {
            ::close(idleFd_);
            std::shared_ptr<IPAddress> addr;
            listenSocket_->accept(addr);
            listenSocket_->close();
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}

}