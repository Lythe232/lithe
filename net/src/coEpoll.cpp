#include "include/coEpoll.h"
#include "include/coEpollItem.h"
#include "include/coScheduler.h"


#include <errno.h>
#include <string.h>
namespace lithe
{
    
static auto g_logger = LOG_ROOT();
CoEpoll::CoEpoll() :
    epfd_(epoll_create(5000)),
    scheduler_(nullptr),
    epevents_(InitEventSize),
    mutex_()
{

}

CoEpoll::CoEpoll(CoScheduler* scheduler) : 
    epfd_(epoll_create(5000)),
    scheduler_(scheduler),
    epevents_(InitEventSize),
    mutex_()
{
}

CoEpoll::~CoEpoll()
{
    ::close(epfd_);
}

int CoEpoll::poll(uint64_t timeout, std::vector<CoEpollItem*>* items)
{
    int rt = 0;
    while(1)
    {
        rt = ::epoll_wait(epfd_, &*epevents_.begin(), static_cast<int>(epevents_.size()), static_cast<int>(timeout));   //FIXME uint64 -> int
        if(rt >= 0)
        {
            if(rt == 0)
            {
                LOG_INFO(g_logger) << "Nothing happended.";
                return rt;
            }
            if(rt == epevents_.size())
            {
                epevents_.resize(epevents_.size() * 1.5);
            }
            break;
        }
        else
        {
            if(errno == EINTR)
            {
                continue;
            }
            LOG_ERROR(g_logger) << "CoEpoll::poll() fails. errno[" << errno << "], errstr=" << strerror(errno);
            return rt;
        }
    }
    for(int i = 0; i < rt; i ++)
    {
        CoEpollItem* item = static_cast<CoEpollItem*>(epevents_[i].data.ptr);
        items->push_back(item);
        item->setRevents(epevents_[i].events);
    }
    LOG_INFO(g_logger) << rt << " events happended.";
    return rt;
}

int CoEpoll::addEvent(int fd, int events)
{
    printf("addEvent(%d)\n", fd);
    assert(events &(EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP));
    CoEpollItem* item = nullptr;
    epoll_event event;
    int rt = 0;
    MutexLockGuard lock(mutex_);
    auto it = itemMaps_.find(fd);
    if(it == itemMaps_.end())
    {
        item = new CoEpollItem(fd, scheduler_);
        event.data.ptr = static_cast<void*>(item);
        event.events = events | EPOLLET;
        itemMaps_[fd] = item;
        rt = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event);
    }
    else
    {
        item = (it->second);
        int oldEvents = item->getEvents();
        int opt = oldEvents ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
        int newEvent = oldEvents | events | EPOLLET;
        event.data.ptr = static_cast<void*>(item);
        event.events = newEvent;
        rt = epoll_ctl(epfd_, opt, fd, &event);
    }
    if(rt < 0)
    {
        LOG_ERROR(g_logger) << "CoEpoll::addEvent(" << fd << ") epoll_ctl fails. errno[" << errno << "].";  
        return -1; 
    }
    item->setEvents(event.events);
    item->setCoroutine(Coroutine::getRunCo());
    return 0;
}

void CoEpoll::delEvent(int fd)
{
    printf("delEvent(%d)\n", fd);
    int rt = 0;
    MutexLockGuard lock(mutex_);
    auto it = itemMaps_.find(fd);
    if(it == itemMaps_.end())
    {
        LOG_WARN(g_logger) << "CoEpoll::delEvent() fd not find.";
    }
    else
    {
        CoEpollItem* item = it->second;
        // itemMaps_.erase(fd);
        // delete item;
        // item = nullptr;
        rt = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
        if(rt < 0)
        {
            LOG_ERROR(g_logger) << "CoEpoll::delEvent() epoll_ctl fails. errno[" << errno << "].";
            return ;
        }
        item->setEvents(0);
        scheduler_->schedule(item->getCoroutine());
    }
}

void CoEpoll::cancelEvent(int fd, int events)
{
    MutexLockGuard lock(mutex_);
    auto it = itemMaps_.find(fd);
    if(it == itemMaps_.end())
    {
        LOG_WARN(g_logger) << "CoEpoll::cancelEvent warn. not find fd.";
        return ;
    }
    else
    {
        CoEpollItem* item = it->second;
        int newEvent = EPOLLET | item->getEvents() & ~events;
        int opt = newEvent ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
        epoll_event event;
        event.events = newEvent;
        event.data.ptr = item;

        int rt = epoll_ctl(epfd_, opt, fd, &event);
        if(rt < 0)
        {
            LOG_ERROR(g_logger) << "CoEpoll::cancelEvent() epoll_ctl fails. errno[" << errno << "].";
        }
        item->setEvents(newEvent);
    }
}

void CoEpoll::cancelAllEvent(int fd)
{
    MutexLockGuard lock(mutex_);
    auto it = itemMaps_.find(fd);
    if(it == itemMaps_.end())
    {
        LOG_WARN(g_logger) << "CoEpoll::cancelAllEvent warn. not find fd.";
        return ;
    }
    else
    {
        CoEpollItem* item = it->second;
        int oldEvents = item->getEvents();
        if(oldEvents == 0)
        {
            return ;
        }
        int rt = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
        if(rt < 0)
        {
            LOG_ERROR(g_logger) << "CoEpoll::cancelAllEvent() epoll_ctl fails. errno[" << errno << "].";
        }
        item->setEvents(0);
    }
}

int CoEpoll::updateEvent(int opt, int events, int fd)
{
    if(opt == EPOLL_CTL_DEL)
    {
        delEvent(fd);
    }
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.data.fd = fd;
    event.events = events;
    CoEpollItem* item = nullptr;

    MutexLockGuard lock(mutex_);

    auto it = itemMaps_.find(fd);
    int rt = 0;
    if(it == itemMaps_.end())
    {
        LOG_ERROR(g_logger) << "CoEpoll::updataEvent() fd not find.";
        return -1;
    }
    else
    {
        item = it->second;
        rt = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event);
        return 0;
    }
    if(rt < 0)
    {
        LOG_ERROR(g_logger) << "CoEpoll::update() epoll_ctl fails. errno[" << errno << "].";
        return -1;
    }
}

void CoEpoll::addItem(CoEpollItem* item)
{
    assert(item);
    int fd = item->fd();
    MutexLockGuard lock(mutex_);
    auto it = itemMaps_.find(fd);
    if(it != itemMaps_.end())
    {
        LOG_WARN(g_logger) << "CoEpoll::addItem(). fd already exists.";
        return ;
    }
    else
    {
        itemMaps_.insert({fd, item});
    }
    return ;
}

void CoEpoll::removeItem(int fd)
{
    printf("removeItem(%d)\n", fd);
    MutexLockGuard lock(mutex_);
    auto it = itemMaps_.find(fd);
    if(it == itemMaps_.end())
    {
        LOG_WARN(g_logger) << "CoEpoll::removeItem() warn. not find fd[" << fd << "].";
    }
    else
    {
        CoEpollItem* item = it->second;
        if(item->getEvents())
        {
            cancelAllEvent(fd);
        }
        itemMaps_.erase(fd);
        delete item;
        item = nullptr;
    }
    return ;
}


}