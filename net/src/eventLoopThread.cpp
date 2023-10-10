#include "include/eventLoopThread.h"
#include "include/eventLoop.h"
namespace lithe
{

EventLoopThread::EventLoopThread(std::string name) : 
    loop_(new EventLoop()),
    thread_(new Thread(std::bind(&EventLoopThread::threadFunc, this), name)),
    mutex_()
{
}

EventLoopThread::~EventLoopThread()
{
    if(loop_)
    {
        delete loop_;
        loop_ = nullptr;
    }
}

void EventLoopThread::start()
{
    MutexLockGuard lock(mutex_);
    thread_->start();
}

void EventLoopThread::threadFunc()
{
    loop_->loop();
}


}