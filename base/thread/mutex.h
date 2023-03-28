#pragma once

#include <pthread.h>

namespace lithe
{


class Mutex
{
friend class Condition;
public:
    Mutex()
    {

        pthread_mutex_init(&mutex_, NULL);
    }
    ~Mutex()
    {

        pthread_mutex_destroy(&mutex_);
    }
    void lock()
    {
        pthread_mutex_lock(&mutex_);
    }
    void unlock()
    {
        pthread_mutex_unlock(&mutex_);
    }
private:
    Mutex(Mutex&&) = delete;
    Mutex(Mutex&)  = delete;
    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }
    pthread_mutex_t mutex_;
};

class MutexLockGuard
{
public:
    MutexLockGuard(Mutex& mutex) : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }
private:
    MutexLockGuard(MutexLockGuard&) = delete;
    MutexLockGuard(MutexLockGuard&&) = delete;
    Mutex& mutex_;

};


};  // namespace lithe
