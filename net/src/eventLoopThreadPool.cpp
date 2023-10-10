#include "include/eventLoopThreadPool.h"
#include "include/eventLoop.h"
#include "include/eventLoopThread.h"

namespace lithe
{

EventLoopThreadPool::EventLoopThreadPool(int numThreads) : //numThread == 0 
    baseLoop_(nullptr),
    pool_(),
    started_(false),
    numThreads_(numThreads),
    nextLoop_(0)
{
    {
        baseLoop_ = new EventLoop();
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    if(baseLoop_)
    {
        delete baseLoop_;
        baseLoop_ = nullptr;
    }
    for(int i = 0; i < numThreads_; i++)
    {
        if(pool_[i])
        {
            delete pool_[i];
            pool_[i] = nullptr;
        }
    }
}

void EventLoopThreadPool::start(std::function<void(EventLoop*)> threadIninCallback)
{
    assert(!started_);
    started_ = true;

    pool_.resize(numThreads_);
    for(int i = 0; i < numThreads_; i++)
    {
        EventLoopThread* elt = new EventLoopThread("IOWorker");
        pool_[i] = (elt);
        pool_[i]->start();
    }
    if(threadIninCallback)
    {
        threadIninCallback(baseLoop_);
    }
    //main loop start
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = pool_[nextLoop_ % numThreads_]->getLoop();
    nextLoop_++;
    return loop;
}

EventLoop* EventLoopThreadPool::getMainLoop()
{
    assert(baseLoop_);
    return baseLoop_;
}

}
