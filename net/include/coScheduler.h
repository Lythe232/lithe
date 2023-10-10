#pragma once

#include "thread/mutex.h"
#include "thread/thread.h"
#include "coroutine.h"
#include "common/blockingQueue.h"
#include "timer.h"
#include "coEpoll.h"
#include "coEpollItem.h"

#include "common/countDownLatch.h"


#include <string>
#include <memory>
#include <deque>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <atomic>
namespace lithe
{
class CoScheduler;
class Poller;

class TaskQueue //TODO use nolock
{
public:
    TaskQueue();
    ~TaskQueue();

    std::shared_ptr<Coroutine> take();
    std::deque<std::shared_ptr<Coroutine>> takeAll();
    void push(std::shared_ptr<Coroutine> task);
    void push(std::function<void()> task);
    ssize_t size()
    {
        MutexLockGuard lock(mutex_);
        return tasksQueue_.size();
    }
    
private:
    std::deque<std::shared_ptr<Coroutine>> tasksQueue_;
    Mutex mutex_;
};


class CoScheduler : public Noncopyable
{
public:
    typedef std::vector<CoEpollItem*> EpollItemLists;

    CoScheduler();
    CoScheduler(std::string name, CoEpoll* poller = nullptr);
    ~CoScheduler();
    void start();
    void stop();
    void idle();
    void schedule(std::shared_ptr<Coroutine> cor);
    void schedule(std::function<void()> func);
    void schedule(std::vector<std::function<void()>> tasks);
    void schedule(std::vector<std::shared_ptr<Coroutine>> cors);
    void scheduleAndWakeUp(std::shared_ptr<Coroutine> cor);
    void scheduleAndWakeUp(std::function<void()> func);
    bool hasExpiredTimer();
    void wakeUp();

    std::shared_ptr<TimerManager> getTimerManager();
    CoEpoll* getPoller();
    static Coroutine* getMainCo();
    static CoScheduler* getThis();

    static uint64_t getSwitchCount() { return switchCount_.load(); }

    static void yieldToHold();
	static void yieldToReady();

private:
    void run(); // TODO use nolock

    std::string name_;
    int tid_;
    std::unique_ptr<Thread> thread_;
    bool started_;
    std::shared_ptr<TimerManager> timerMgr_;
    int wakeUpFd_;
    bool isIdle_;
    Mutex mutex_;
    CoEpoll* poller_;
    EpollItemLists itemLists_;
    std::vector<std::shared_ptr<Coroutine>> cors_;  //TODO use pool
    static std::atomic<uint64_t> switchCount_;
};


}
