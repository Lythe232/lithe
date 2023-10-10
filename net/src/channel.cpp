#include "include/channel.h"
#include "include/eventLoop.h"
#include "include/log/log.h"
#include "include/coScheduler.h"
#include "include/poller.h"


namespace lithe
{

static auto g_logger = LOG_ROOT();

Channel::Channel(int fd, EventLoop* loop) : 
                readCallback_(nullptr),
                writeCallback_(nullptr),
                closeCallback_(nullptr),
                errorCallback_(nullptr),
                fd_(fd),
                events_(0),
                revents_(0),
                loop_(loop),
                poller_(loop_->getPoller()),
                mutex_()
{
    assert(poller_);
}
Channel::~Channel()
{
}

void Channel::setReadCallback(std::function<void()> func)
{
    readCallback_ = (func);
}
void Channel::setWriteCallback(std::function<void()> func)
{
    writeCallback_ = (func);
}
void Channel::setCloseCallback(std::function<void()> func)
{
    closeCallback_ = (func);
}
void Channel::setErrorCallback(std::function<void()> func)
{
    errorCallback_ = (func);
}

void Channel::handleEvent()
{
    int event = revents_;
    if((event & EPOLLHUP) && !(event & EPOLLIN))
    {
        if(closeCallback_)
        {
            closeCallback_();
        }
    }
    if(event & (EPOLLERR))
    {
        if(errorCallback_)
            errorCallback_();
    }
    if(event & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        if(readCallback_)
            readCallback_();
    }
    if(event & (EPOLLOUT))
    {
        if(writeCallback_)
            writeCallback_();
    }
}


}   //namespace lithe
