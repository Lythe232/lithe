#include "thread.h"
#include "log/logger.h"
#include "log/logEvent.h"

#include <assert.h>
#include <iostream>
namespace lithe
{

namespace details
{
struct ThreadData
{
    ThreadData(Thread::ThreadFunc func, std::string name) : func_(func), threadName_(name)
    {
    }
    std::string threadName_;
    Thread::ThreadFunc func_;
    ~ThreadData()
    {
    }
};

void* threadFunc(void* args)
{   

    ThreadData* datas = static_cast<ThreadData*> (args);    
    datas->func_();

    delete  datas;
    return nullptr;
}
}   // namespace details

extern LoggerManager g_loggerMgr;
Thread::Thread(ThreadFunc func, std::string name)
                :
                func_(std::move(func)),
                name_(name),
                started_(false),
                joined_(false),
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
    details::ThreadData* datas = new details::ThreadData(func_, std::string());

    if(pthread_create(&pthreadId_, nullptr, &details::threadFunc, datas))
    {
        started_ = false;
        delete [] datas;
        auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
        LOG_FATAL(logger) << "pthread_create fail";
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
void Thread::setDefaultName()
{
    ++numCreated_;
    char buf[32];
    snprintf(buf, sizeof buf, "Thread%d", numCreated_.load());
    name_ = buf;
}

}