#pragma once

#include "include/common/noncopyable.h"

#include <pthread.h>
#include <stdio.h>
namespace lithe
{


class Mutex : public Noncopyable
{
friend class Condition;
public:
    Mutex() :
    mutex_()
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
    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }
    pthread_mutex_t mutex_;
};

class MutexLockGuard : public Noncopyable
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

    Mutex& mutex_;

};

class SpinLock
{
public:
    SpinLock()
    {
        pthread_spin_init(&lock_, PTHREAD_PROCESS_PRIVATE);
    }
    ~SpinLock()
    {
        pthread_spin_destroy(&lock_);
    }
    void tryLock()
    {
        pthread_spin_trylock(&lock_);
    }
    void lock()
    {
        pthread_spin_lock(&lock_);
    }
    void unlock()
    {
        pthread_spin_unlock(&lock_);
    }
private:
    pthread_spinlock_t lock_;
};

};  // namespace lithe
