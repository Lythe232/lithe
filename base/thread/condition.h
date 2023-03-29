#pragma once

#include "mutex.h"


namespace lithe
{

class Condition : public Noncopyable
{
public:
    explicit Condition(Mutex& mutex) : 
                        mutex_(mutex) 
    {
        pthread_cond_init(&cond_, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond_);
    }
    void wait()
    {
        pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
    }
    bool waitForSeconds(double seconds);

    void notify()
    {
        pthread_cond_signal(&cond_);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&cond_);
    }
private:
    Mutex& mutex_;
    pthread_cond_t cond_;

};


};  //namespace lithe