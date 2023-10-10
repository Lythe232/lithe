#pragma once

#include "thread/mutex.h"
#include "common/noncopyable.h"
#include "include/poller.h"
#include "include/epoll_poller.h"

#include <assert.h>
#include <functional>
#include <vector>
#include <sys/epoll.h>
namespace lithe
{
class EventLoop;

class Channel : Noncopyable
{
public:
    typedef std::function<void()> EventCallback;
    

    Channel(int fd, EventLoop* loop);
    ~Channel();

    void setReadCallback(std::function<void()> func);
    void setWriteCallback(std::function<void()> func);
    void setCloseCallback(std::function<void()> func);
    void setErrorCallback(std::function<void()> func);

    int fd() { return fd_; }
    uint32_t getEvents() { return events_; }
    uint32_t getRevents() { return revents_; }
    void setEvents(uint32_t events) { events_ = events; }
    void setRevents(uint32_t events) { revents_ = events; }
    void handleEvent();     //only used by loop_;
    bool isWriting() { return events_ & EPOLLOUT; }
    void enableWriting()   { poller_->addEvent(this, EPOLLOUT); }
    void enableReading()   { poller_->addEvent(this, EPOLLIN); }
    void disableWriting()  { poller_->updateEvent(this, EPOLL_CTL_MOD, events_ & ~EPOLLOUT); }
    void disableAllEvent() { poller_->delEvent(this); }
    void remove() { /*disableAllEvent();*/ poller_->removeChannel(fd_); }   //需要手动删除channel对象
    EventLoop* getLoop(){ return loop_; }

private:
    Channel() = delete;
    
    EventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback closeCallback_;
    EventCallback errorCallback_;

    int fd_;
    uint32_t events_;
    uint32_t revents_;
    EventLoop* loop_;
    Poller* poller_;
public:
    Mutex mutex_;
};

} // namespace lithe
