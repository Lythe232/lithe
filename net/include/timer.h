#pragma once

#include "common/timestamp.h"
#include "thread/mutex.h"

#include <memory>
#include <set>
#include <vector>
#include <functional>
#include <atomic>

namespace lithe
{

class TimerManager;
class Timer
{
public:
    Timer(uint64_t when, std::function<void()> func, uint32_t ms);
    void reset();
    bool reset(uint64_t when, std::function<void()> func);
    void run()
    {
        if(func_)
        {
            func_();
        }
    }
private:
    class Comparator
    {
    public:
        bool operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs) const;
    };
public:
    friend class TimerManager;
private:
    //TimerManager use
    Timer(uint64_t when) : expiration_(when)
    {
    }

    uint64_t expiration_;
    std::function<void()> func_;
    bool repeat_;
    uint32_t interval_;

    Timestamp stamp_;
    static std::atomic<uint64_t> sequence_;

};

class TimerManager
{
public:
    TimerManager();

    bool cancelTimer(std::shared_ptr<Timer> timer);
    std::shared_ptr<Timer> addTimer(uint64_t when, std::function<void()> func, uint32_t interval = 0);
    static void onTimer(std::function<void()> func, std::weak_ptr<void> wcond);
    std::shared_ptr<Timer> addConditionTimer(uint64_t when, std::function<void()> func, std::weak_ptr<void> wcond, uint32_t interval = 0);
    std::vector<std::function<void()>> getExpired(uint64_t now);
    bool checkClockRoll(uint64_t now);
    uint64_t getNextTimer();
private:
    Timestamp stamp_;
    std::set<std::shared_ptr<Timer>, Timer::Comparator> timers_;
    std::set<std::shared_ptr<Timer>, uint64_t> activeTimers_;
    uint64_t lastCallTime_ = 0;
    Mutex mutex_;
};

}   //namepsace lithe