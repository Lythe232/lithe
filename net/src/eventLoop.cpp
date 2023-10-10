#include "include/eventLoop.h"
#include "include/fdmanager.h"
#include "include/timer.h"
#include "include/thread/currentThread.h"
#include <sys/eventfd.h>

namespace lithe
{
static auto g_logger  = LOG_ROOT();

static int createEventFd()
{
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(fd < 0)
    {
        LOG_FATAL(g_logger) << "createEventFd fails.";
        assert(EXIT_FAILURE);
    }   
    return fd;
}
EventLoop::EventLoop() : 
    tid_(-1),
    wakeupFd_(createEventFd()),
    stoped_(true),
    callingPendingFunctor_(false),
    poller_(new EpollPoller()),
    wakeupChannel_(nullptr),
    tm_(new TimerManager()),
    pendingFunctors_(),
    channelList_()
{
}

EventLoop::~EventLoop()
{
    ::close(wakeupFd_);
    if(wakeupChannel_)
    {
        wakeupChannel_->remove();
        delete wakeupChannel_;
        wakeupChannel_ = nullptr;
    }
    if(poller_)
    {
        delete poller_;
        poller_ = nullptr;
    }
}

void EventLoop::stop()
{
    stoped_ = true;
}

void EventLoop::loop()
{
    tid_ = CurrentThread::cacheTid();
    if(wakeupChannel_ == nullptr)
    {
        wakeupChannel_ = new Channel(wakeupFd_, this);
    }
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading();
    static const int MAX_TIMEOUT = 3000;
    std::vector<std::function<void()> > timerList;
    stoped_ = false;

    LOG_INFO(g_logger) << "EventLoop " << this << " start loop.";
    while(!stoped_)
    {
        channelList_.clear();
        uint64_t timeout = tm_->getNextTimer();
        if(timeout != (uint64_t)-1)
        {
            timeout = (timeout > MAX_TIMEOUT) ? MAX_TIMEOUT : timeout;
        }
        else
        {
            timeout = MAX_TIMEOUT;  
        }
        int rt = poller_->poll(3000, &channelList_);
        for(int i = 0; i < rt; i++)
        {
            Channel* channel = channelList_[i];
            assert(channel);
            channel->handleEvent();
        }
        timerList = tm_->getExpired(Timestamp::now().milliSecond());
        for(auto it : timerList)
        {
            (it)();
        }
        doPendingFunctor();        
    }
    LOG_INFO(g_logger) << "EventLoop " << this << " stop loop.";

}

void EventLoop::runAt(Timestamp ts, std::function<void()> timerFunc)
{
    uint64_t now = Timestamp::now().milliSecond();
    if(ts.milliSecond() < now)
    {
        LOG_WARN(g_logger) << "EventLoop::runAt set time is less than the current time.";
        return ;
    }
    tm_->addTimer(ts.milliSecond() - now, timerFunc);
}

void EventLoop::runAfter(uint64_t delay, std::function<void()> timerFunc)
{
    tm_->addTimer(delay, timerFunc);
}

void EventLoop::runEvery(uint64_t interval, std::function<void()> timerFunc)
{
    tm_->addTimer(Timestamp::now().milliSecond(), timerFunc, interval);
}

void EventLoop::wakeup()
{
    uint64_t msg = 1;
    if(::write(wakeupFd_, &msg, sizeof(msg)) != sizeof(msg))
    {
        LOG_FATAL(g_logger) << "EventLoop::wakeup() fails.";
    }
}

void EventLoop::queueInLoop(const Functor& functor)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(functor);
    }
    
    if(!(isInLoopThread()) || callingPendingFunctor_)
    {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor& functor)
{
    if(isInLoopThread())
    {
        functor();
    }
    else
    {
        queueInLoop(functor);   
    }
}

void EventLoop::assertNotInLoopThread()
{
    if(!isInLoopThread())
    {
        LOG_FATAL(g_logger) << "EventLoop::assertNotInLoopThread() " << 
        this << "was created in threadId_ = " << tid_ <<
        ", current thread id = " << CurrentThread::cacheTid();
        printf("EventLoop::assertNotInLoopThread() %lx, was created in threadId_ = %d, current thread id = \n", this, tid_, CurrentThread::cacheTid());
        assert(0);
    }
}

void EventLoop::doPendingFunctor()
{
    callingPendingFunctor_ = true;
    std::vector<Functor> functor;

    {
        MutexLockGuard lock(mutex_);
        functor.swap(pendingFunctors_);
    }

    for(auto it : functor)
    {
        (it)();
    }
    callingPendingFunctor_ = true;
}

void EventLoop::handleRead()
{
    uint64_t buf = 1;
    ssize_t n = ::read(wakeupFd_,  &buf, sizeof(buf));
    if(n != sizeof(buf))
    {
        LOG_ERROR(g_logger) << "EventLoop::handleRead() error. reads " << n <<  "bytes instead of 8.";
    }

}

}