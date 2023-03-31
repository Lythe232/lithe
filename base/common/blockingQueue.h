#pragma once
#include "thread/mutex.h"
#include "thread/condition.h"
#include "common/noncopyable.h"

#include <deque>
#include <string>

namespace lithe
{

template<typename T>

class BlockingQueue : public Noncopyable
{
public:
    BlockingQueue() : 
        mutex_(),
        cond_(mutex_),
        queue_()
    {
    }
    T take()
    {
        MutexLockGuard lock(mutex_);
        while(queue_.empty())
        {
            cond_.wait();
        }
        T front(std::move(queue_.front()));
        queue_.pop_front();
        return front;
    }
    std::deque<T> takeAll()
    {
        std::deque<T> queue;
        {
            MutexLockGuard lock(mutex_);
            queue = queue_;
        }
        return queue;
    }
    std::deque<T> takeAllAndClear()
    {
        std::deque<T> queue;
        {
            MutexLockGuard lock(mutex_);
            queue = std::move(queue_);
            assert(queue_.empty());
        }
        return queue;
    }
    void put(T& p)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(p);
        cond_.notify();
    }
    size_t size()
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }

private:
    int count_;
    Mutex mutex_;
    Condition cond_;
    std::deque<T> queue_;

};


}
