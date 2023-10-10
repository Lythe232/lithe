#pragma once

#include "epoll_poller.h"
#include "common/timestamp.h"
#include "timer.h"
#include "channel.h"
#include "thread/mutex.h"
#include <functional>

namespace lithe
{

class EventLoop
{
public:
    typedef std::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    void start();
    void stop();

    void loop();

    void runAt(Timestamp ts, const std::function<void()> timerFunc);
    void runAfter(uint64_t delay, const std::function<void()> timerFunc);
    void runEvery(uint64_t interval, const std::function<void()> timerFunc);

    void wakeup();

    void queueInLoop(const Functor& func);
    void runInLoop(const Functor& func);
    bool isInLoopThread() { return tid_ == CurrentThread::cacheTid(); }

    void assertNotInLoopThread();

    Poller* getPoller() { return poller_; }
private:

    void doPendingFunctor();
    void handleRead();

        int tid_;
    int wakeupFd_;
    bool stoped_;
    bool callingPendingFunctor_;
    Poller* poller_;
    Channel* wakeupChannel_;
    std::shared_ptr<TimerManager> tm_;
    std::vector<Functor> pendingFunctors_;

    Poller::ChannelList channelList_;
    Mutex mutex_;
};


}
