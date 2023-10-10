#pragma once

#include "thread/mutex.h"
#include "thread/thread.h"
#include "coroutine/coroutine.h"
#include "common/blockingQueue.h"
#include "timer.h"
#include "poller/channel.h"
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

// class CoDeliverer 
// {
// public:
//     CoDeliverer(int count, int schedulerCount, std::string name);
//     ~CoDeliverer();

//     void start();

//     void run();

//     void schedule(std::function<void()> task);

//     std::shared_ptr<CoScheduler> getRelaxed();

//     Poller* getPoller(){ return poller_; }
// private:
//     std::vector<std::shared_ptr<CoScheduler>> loops_;

//     std::vector<std::shared_ptr<Thread>> threads_;
//     int threadCount_;
//     int schedulerCount_;
//     std::string name_;
//     std::shared_ptr<TaskQueue> queue_;
//     bool started_;
//     std::atomic<int> index_;
//     Mutex mutex_;
//     CountDownLatch latch_;
//     Poller* poller_;
// };

class CoScheduler : public Noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    CoScheduler();
    CoScheduler(std::string name, Poller* poller = nullptr);
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
    Poller* getPoller();

    static Coroutine* getMainCo();
    static CoScheduler* getThis();

    // int getWorkload() const { return cors_.size(); }
    static uint64_t getSwitchCount() { return switchCount_.load(); }

    static void yieldToHold();
	static void yieldToReady();


    // bool operator<(const CoScheduler& other) const
    // {
    //     return this->getWorkload() < other.getWorkload();
    // }
private:
    void run(); // TODO use nolock

    static thread_local Coroutine* t_scheduler_co_;
    std::string name_;
    int tid_;
    std::unique_ptr<Thread> thread_;
    bool started_;
    std::vector<std::shared_ptr<Coroutine>> cors_;  //TODO use pool
    std::shared_ptr<TimerManager> timerMgr_;
    int wakeUpFd_;
    bool isIdle_;
    Mutex mutex_;
    ChannelList channelList_;
    Poller* poller_;
    static std::atomic<uint64_t> switchCount_;
};


}
