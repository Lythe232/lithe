#pragma once

#include "thread/mutex.h"
#include "coEpoll.h"
#include "coroutine.h"

namespace lithe
{

class CoEpollItem
{
public:
    CoEpollItem(int fd, CoScheduler* scheduler);
    ~CoEpollItem();
    
    int fd() { return fd_; }
    void processEvent();
    
    void setCoroutine(std::shared_ptr<Coroutine> cor);
    void setRevents(int events) { revents_  = events; }
    void setEvents(int events) { events_ = events; }
    void setScheduler(CoScheduler* scheduler) { scheduler_ = scheduler; }
    void disableReading();
    void enableReading();
    void disableWriting();
    void enableWriting();

    int getRevents() { return revents_; }
    int getEvents() { return events_; }
    std::shared_ptr<Coroutine> getCoroutine() { return cor_; }
    CoScheduler* getScheduler() { return scheduler_; }
    void reset();
private:
    int fd_;
    int events_;    //关注的事件
    int revents_;   //发生的事件
    CoScheduler* scheduler_;
    CoEpoll* poller_;
    std::shared_ptr<Coroutine> cor_;
    Mutex mutex_;
};


}