#include "include/thread/thread.h"
#include "include/thread/currentThread.h"
#include "include/log/log.h"

#include <assert.h>
#include <iostream>

namespace CurrentThread
{
thread_local pid_t t_cacheTid_ = 0;
thread_local char tidString[32];
pid_t cacheTid()
{
    if(t_cacheTid_ == 0)
    {
        t_cacheTid_ = CurrentThread::gettid();
    }
    return t_cacheTid_;
}
char* getThreadName()
{
    return tidString;
}
}
namespace lithe
{

Thread::Thread(ThreadFunc func, std::string name) :
                pthreadId_(0),
                tid_(0),
                name_(name),
                started_(false),
                joined_(false),
                func_(std::move(func)),
                numCreated_(0)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    if(pthread_create(&pthreadId_, nullptr, (void* (*)(void*))&Thread::run, this) != 0)
    {
        started_ = false;
        assert(0);
    }
}
int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, nullptr);
}
void *Thread::run(void* args)
{
    Thread* fun = static_cast<Thread*>(args);
    fun->tid_ = CurrentThread::cacheTid();
    memcpy(CurrentThread::tidString, fun->name_.c_str(), 32);
    try
    {
        /* code */
        fun->func_();
        fun->started_ = false;
    }
    catch(const std::exception& e)
    {
        printf("%s\n", e.what());
    }
    return nullptr;
}

int Thread::gettid()
{
    if(tid_ == 0)    // not start
    {
        assert(0);
    }
    return tid_;
}
void Thread::setDefaultName()
{
    ++numCreated_;
    if(name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", numCreated_.load());
        name_ = buf;
    }
    
}

}