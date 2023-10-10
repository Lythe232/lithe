#pragma once

#include "thread/thread.h"
#include "thread/mutex.h"

#include <memory>

namespace lithe
{
class EventLoop;
class EventLoopThread
{
public:
    EventLoopThread(std::string name);
    ~EventLoopThread();

    void start();
    EventLoop* getLoop() { return loop_; };
private:
    void threadFunc();

    EventLoop* loop_;
    std::shared_ptr<Thread> thread_;
    Mutex mutex_;

};

}