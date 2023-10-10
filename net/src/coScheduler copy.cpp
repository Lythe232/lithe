#include "coScheduler.h"

#include "log/log.h"
#include "hookSysCall.h"
#include "poller/epoll_poller.h"
#include "fdmanager.h"

#include <fcntl.h>
#include <algorithm>
namespace lithe
{
thread_local Coroutine* CoScheduler::t_scheduler_co_ = nullptr;
thread_local CoScheduler* t_scheduler = nullptr;
std::atomic<uint64_t> CoScheduler::switchCount_ = {0};

auto g_logger = LOG_ROOT();

TaskQueue::TaskQueue() : 
                        mutex_()
{
}

TaskQueue::~TaskQueue()
{
}

std::shared_ptr<Coroutine> TaskQueue::take()
{
    std::shared_ptr<Coroutine> front;
    MutexLockGuard lock(mutex_);
    if(!tasksQueue_.empty())
    {
        front = tasksQueue_.front();
        tasksQueue_.pop_front();
    }
    return front;
}

std::deque<std::shared_ptr<Coroutine>> TaskQueue::takeAll()
{
    std::deque<std::shared_ptr<Coroutine>> all;
    MutexLockGuard lock(mutex_);
    if(!tasksQueue_.empty())
    {
        all.swap(tasksQueue_);
    }
    return all;
}

void TaskQueue::push(std::shared_ptr<Coroutine> task)
{
    MutexLockGuard lock(mutex_);
    tasksQueue_.push_back(task);
}

void TaskQueue::push(std::function<void()> task)
{
    std::shared_ptr<Coroutine> cor(new Coroutine(task));

    MutexLockGuard lock(mutex_);

    tasksQueue_.push_back(cor);
}

//----------------------------------------------------------------------------------------

static int createEventFd()
{
    int fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(fd < 0)
    {
        LOG_FATAL(g_logger) << "EventFd create failed.";
        exit(EXIT_FAILURE);        
    }
    lithe::FdMgr::getSingletonPtr()->get(fd, true);
    return fd;
}
CoScheduler::CoScheduler(std::string name, Poller *poller) : name_(name),
                                                             tid_(-1),
                                                             thread_(new Thread(std::bind(&CoScheduler::run, this), name_)),
                                                             started_(false),
                                                             timerMgr_(new TimerManager()),
                                                             wakeUpFd_(createEventFd()),
                                                             isIdle_(false),
                                                             mutex_(),
                                                             poller_(poller ? poller : new EpollPoller())
{
    epoll_create(5000);
}
CoScheduler::~CoScheduler()
{
    close(wakeUpFd_);
    if(poller_)
    {
        delete poller_;
    }
}

void lithe::CoScheduler::start()
{
    if(started_)
    {
        LOG_WARN(g_logger) << "Thread[" << tid_ << "] has been started."; 
        return ;
    }
    started_ = true;
    thread_->start();
}

Coroutine* CoScheduler::getMainCo()
{
    return t_scheduler_co_;
}
CoScheduler* CoScheduler::getThis()
{
    return t_scheduler;
}
void CoScheduler::run()
{
	set_hook_enable(true);
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.data.fd = wakeUpFd_;
    event.events = EPOLLIN | EPOLLET;
    Channel* channel = new Channel(wakeUpFd_, this);

    poller_->addEvent(channel, EPOLLIN | EPOLLET);

    std::shared_ptr<Coroutine> cor;
    std::shared_ptr<Coroutine> idle_cor(new Coroutine(std::bind(&CoScheduler::idle, this)));
    t_scheduler = this;
    t_scheduler_co_ = Coroutine::getRunCo().get();
    while(1)
    {
        if(!cors_.empty())
        {
            {
                MutexLockGuard lock(mutex_);
                cor.swap(cors_[0]);
                assert(cor);
                cors_.erase(cors_.begin());
            }
            if(cor->getState() != Coroutine::END && cor->getState() != Coroutine::EXCEP)
            {
                cor->swapIn();
                if(cor->getState() == Coroutine::READY)
                {
                    schedule(cor);
                    cor.reset();
                }
                else if(cor->getState() != Coroutine::END && cor->getState() != Coroutine::EXCEP)
                {
                    cor->setState(Coroutine::RUNNING);
                }
            }

            continue;
        }
        else
        {
            //IDLE
            if(idle_cor->getState() == Coroutine::END)
            {
                LOG_WARN(g_logger) << "Idle Coroutine has been terminate. Coroutine state: " << Coroutine::toString(idle_cor->getState());
                break;
            }
            isIdle_ = true;
            idle_cor->swapIn();
            isIdle_ = false;
            if(idle_cor->getState() != Coroutine::END && idle_cor->getState() != Coroutine::EXCEP)
            {
                idle_cor->setState(Coroutine::RUNNING);
            }
        }
    }
    LOG_INFO(g_logger) <<  "Scheduler " << name_ << "has been terminate.";
}

void CoScheduler::stop()
{
    //TODO
    thread_->join();
}

void CoScheduler::idle()
{
    std::vector<std::function<void()>> timerList;
    while(1)
    {
        channelList_.clear();
        static const int MAX_TIMEOUT = 10000;
        uint64_t timeout = timerMgr_->getNextTimer();
        int rt = 0;
        do{
            if(timeout != (uint64_t) -1)
            {
                timeout = (MAX_TIMEOUT < timeout) ? MAX_TIMEOUT : timeout;
            }
            else
            {
                timeout = MAX_TIMEOUT;
            }
            rt = poller_->poll(timeout, &channelList_);
            if(rt < 0 && errno == EINTR)
            {
            }
            else
            {
                break;
            }

        }while(1);
        timerList = timerMgr_->getExpired(Timestamp::now().milliSecond());
        for(auto it : timerList)
        {
            schedule(it);
        }

        timerList.clear();
        for(auto it = channelList_.begin(); it != channelList_.end(); it++)
        {
            if((*it)->fd() == wakeUpFd_)
            {
                uint64_t buf;
                while(sys_read_fun(wakeUpFd_, &buf, sizeof(uint64_t)) > 0);
                continue;
            }
            // MutexLockGuard lock((*it)->mutex_);

            // int realEvent = (*it)->revents();
            // int callEvent = 0;
            // if(realEvent & (EPOLLERR | EPOLLHUP))
            // {
            //     (*it)->setEvents(realEvent | (POLLIN | EPOLLOUT) & (*it)->events());
            // }
            // if(realEvent & EPOLLIN)
            // {
            //     callEvent |= EPOLLIN;
            // }
            // if(realEvent & EPOLLOUT)
            // {
            //     callEvent |= EPOLLOUT;
            // }
            // if(((*it)->events() & callEvent) == 0)
            // {
            //     continue;
            // }
            // int remainEvent = ((*it)->events() & ~callEvent);
            // int op = remainEvent ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            // (*it)->setEvents(EPOLLET | remainEvent);
            // if(poller_->updateEvent(op, realEvent, (*it))) 
            // {
            //     continue;
            // }
            (*it)->handleEvent();
        }

        std::shared_ptr<Coroutine> cor = Coroutine::getRunCo();
        Coroutine* pc = cor.get();
        cor.reset();
        pc->swapOut();
    }
}

void CoScheduler::schedule(std::shared_ptr<Coroutine> cor)
{
    MutexLockGuard lock(mutex_);
    cors_.push_back(cor);
}

void CoScheduler::schedule(std::function<void()> func)
{
    if(func)
    {
        std::shared_ptr<Coroutine> cor(new Coroutine(std::bind(func)));
        schedule(cor);
    }
}

void CoScheduler::schedule(std::vector<std::function<void()>> funcs)
{
    if(!funcs.empty())
    {
        std::shared_ptr<Coroutine> cor;
        for(int i = 0 ; i < (int)funcs.size(); i++)
        {
            cor.reset(new Coroutine(std::bind(funcs[i])));
            schedule(cor);
        }
    }
} 

void CoScheduler::schedule(std::vector<std::shared_ptr<Coroutine>> cors)
{
    if(!cors.empty())
    {
        for(int i = 0; i < (int)cors.size(); i++)
        {
            schedule(cors[i]);
        }
    }
}

void CoScheduler::scheduleAndWakeUp(std::shared_ptr<Coroutine> cor)
{
    MutexLockGuard lock(mutex_);
    cors_.push_back(cor);
    wakeUp();
}

void CoScheduler::scheduleAndWakeUp(std::function<void()> func)
{
    std::shared_ptr<Coroutine> cor = std::make_shared<Coroutine>(func);
    scheduleAndWakeUp(cor);
}

bool CoScheduler::hasExpiredTimer()
{
    assert(timerMgr_);
    if(timerMgr_->getNextTimer() > 0)
    {
        return true;
    }
    return false;
}

void CoScheduler::wakeUp()
{
    if(isIdle_)
    {
        uint64_t value = 1;
        if(::sys_write_fun(wakeUpFd_, &value, sizeof(value)) != sizeof(value))
        {
            LOG_ERROR(g_logger) << name_ << "CoScheduler::wakeUp fails. errno[" << errno << "].";
        }
    }

}

std::shared_ptr<TimerManager> CoScheduler::getTimerManager()
{
    return timerMgr_;
}

Poller* CoScheduler::getPoller()
{
    return poller_;
}
void CoScheduler::yieldToHold()
{
	auto cor = Coroutine::getRunCo();
	assert(cor->getState() == Coroutine::RUNNING);
    switchCount_++;
	cor->swapOut();
}
void CoScheduler::yieldToReady()
{
	auto cor = Coroutine::getRunCo();
	assert(cor->getState() == Coroutine::RUNNING);
	cor->setState(Coroutine::READY);
    switchCount_++;
	cor->swapOut();
}
//------------------------------------------------------------------------------------

// CoDeliverer::CoDeliverer(int count, int schedulerCount, std::string name) : 
//     threadCount_(count),
//     schedulerCount_(schedulerCount),
//     name_(name),
//     queue_(new TaskQueue()),
//     started_(false),
//     index_(0),
//     mutex_(),
//     latch_(1),
//     poller_(new EpollPoller())
// {
// }

// CoDeliverer::~CoDeliverer()
// {
//     if(poller_)
//     {
//         delete poller_;
//     }
// }

// void CoDeliverer::start()
// {
//     MutexLockGuard lock(mutex_);
//     started_ = true;
//     threads_.resize(threadCount_);
//     for(int i = 0; i < threadCount_; i++)
//     {
//         threads_[i].reset(new Thread(std::bind(&CoDeliverer::run, this), name_ + std::to_string(i)));
//         threads_[i]->start();
//     }
//     loops_.resize(schedulerCount_);
//     for(int i = 0; i < schedulerCount_; i++)
//     {
//         std::string name = name_ + "_" + std::to_string(i);
//         loops_[i].reset(new CoScheduler(name, poller_));
//         loops_[i]->start();
//     }
//     latch_.countDown();

// }

// void CoDeliverer::run()
// {
//     latch_.wait();
//     std::shared_ptr<Coroutine> task;
//     while(started_)
//     {
//         assert(queue_);
//         task = queue_->take();
//         if(task)
//         {
//             index_ = index_ % schedulerCount_;
//             loops_[index_]->schedule(task);
//             index_ ++;
//         }
//         for(int i = 0; i < schedulerCount_; i++)
//         {
//             assert(loops_[i]);
//             if(loops_[i]->hasExpiredTimer())
//             {
//                 loops_[i]->wakeUp();
//             }
//         }
//     }
// }

// void CoDeliverer::schedule(std::function<void()> task)
// {
//     if(task)
//     {
//         queue_->push(task);
//     }
// }
// std::shared_ptr<CoScheduler> CoDeliverer::getRelaxed()
// {
//     MutexLockGuard lock(mutex_);
//     if(!loops_.empty())
//     {
//         return loops_[index_ ++];
//     }   
//     assert(0);
//     return nullptr; 
// }

} // namespace lithe
