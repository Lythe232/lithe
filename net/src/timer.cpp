#include "include/timer.h"

namespace lithe
{
bool lithe::Timer::Comparator::operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs) const
{
    if(!lhs && !rhs)
    {
        return false;
    }
    if(!lhs)
    {
        return true;
    }
    if(!rhs)
    {
        return false;
    }
    if(lhs->expiration_ < rhs->expiration_)
    {
        return true;
    }
    if(lhs->expiration_ > rhs->expiration_)
    {
        return false;
    }
    return (lhs.get() < rhs.get());
}

std::atomic<uint64_t> Timer::sequence_ = {0};
Timer::Timer(uint64_t when, std::function<void()> func, uint32_t interval) : 
            expiration_(stamp_.now().milliSecond() + when), 
            func_(func),
            repeat_(interval > 0),
            interval_(interval)
{
    sequence_++;
}
void Timer::reset()
{
    if(func_)
    {
        func_ = nullptr;
    }
}

TimerManager::TimerManager()
{
    lastCallTime_ = stamp_.now().milliSecond();
}

bool TimerManager::cancelTimer(std::shared_ptr<Timer> timer)
{
    MutexLockGuard lock(mutex_);
    auto it = timers_.find(timer);
    if(it != timers_.end())
    {
        timers_.erase(it);
        return true;
    }
    return false;
}

std::shared_ptr<Timer> TimerManager::addTimer(uint64_t when, std::function<void()> func, uint32_t interval)
{
    MutexLockGuard lock(mutex_);
    std::shared_ptr<Timer> timer(new Timer(when, func, interval));
    timers_.insert(timer);
    return timer;
}
void TimerManager::onTimer(std::function<void()> func, std::weak_ptr<void> wcond)
{
    if(wcond.lock())
    {
        func();
    }
}

std::shared_ptr<Timer> TimerManager::addConditionTimer(uint64_t when, std::function<void()> func, std::weak_ptr<void> wcond, uint32_t interval)
{
    return addTimer(when, std::bind(&TimerManager::onTimer, func, wcond), interval);
}

std::vector<std::function<void()>> TimerManager::getExpired(uint64_t now)
{
    //TODO FIX std::vector<shared_ptr<Timer>>
    std::vector<std::shared_ptr<Timer>> result;
    std::vector<std::function<void()>> funcLists;
    // std::shared_ptr<Timer> tn(new Timer(now));
    std::shared_ptr<Timer> tn = std::make_shared<Timer>(now, nullptr, 0);
    MutexLockGuard lock(mutex_);

    if(timers_.empty())
    {
        return std::vector<std::function<void()>>();
    }
    bool flag = checkClockRoll(now);
    if(!flag && now < (*timers_.begin())->expiration_)
    {
        return std::vector<std::function<void()>>();
    }
    auto end = flag ? timers_.end() : timers_.lower_bound(tn);
    std::copy(timers_.begin(), end, back_inserter(result));
    timers_.erase(timers_.begin(), end);   
    for(auto& it : result)
    {
        funcLists.push_back(it->func_);
        if(it->repeat_)
        {
            it->expiration_ = it->expiration_ + it->interval_;
            timers_.insert(it);
        }
    } 
    return funcLists;  //RVO
}

bool TimerManager::checkClockRoll(uint64_t now)
{
    if(now < lastCallTime_ && now < (lastCallTime_ - 60 * 60 * 1000))
    {
        return true;
    }
    lastCallTime_ = now;
    return false;
}

uint64_t TimerManager::getNextTimer()
{
    MutexLockGuard lock(mutex_);
    uint64_t now = stamp_.now().milliSecond();
    if(!timers_.empty())
    {
        auto it = timers_.begin();
        if(now >= (*it)->expiration_)
        {
            return 0;
        }
        else
        {
            return (*it)->expiration_ - now;
        }
    }
    return -1;
}

} // namespace lithe
