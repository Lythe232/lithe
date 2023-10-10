#include "include/epoll_poller.h"
#include "include/channel.h"
namespace lithe
{

static auto g_logger  = LOG_ROOT();

EpollPoller::EpollPoller() :
    epfd_(::epoll_create(5000)),
    epevents_(256),
    channelMap_(),
    mutex_()
{
}

EpollPoller::~EpollPoller()
{
}


int EpollPoller::poll(uint64_t timeout, Poller::ChannelList* result)
{
    int rt = 0;
    while(true)
    {
        result->clear();
        rt = epoll_wait(epfd_, &*epevents_.begin(), static_cast<int>(epevents_.size()), static_cast<int>(timeout)   );
        if(rt <= 0)
        {
            if(rt == 0)
            {
                LOG_INFO(g_logger) << "Nothing happend.";
                return 0;
            }
            if(errno == EINTR)
            {
                continue;
            }
            LOG_FATAL(g_logger) << "EpollPoller::poll fails. errno[" << errno << "].";
            assert(0);
            return -1;
        }
        else
        {
            for(int i = 0; i < rt; i++)
            {
                Channel* channel = static_cast<Channel*>(epevents_[i].data.ptr);
                auto it = channelMap_.find(channel->fd());
                // assert(it != channelMap_.end());
                channel->setRevents(epevents_[i].events);
                result->push_back(channel);
            }

            if(rt == epevents_.size())
            {
                epevents_.resize(epevents_.size() * 2);
            }
            LOG_INFO(g_logger) << rt << " events happend.";

            return rt;
        }
    }
    LOG_INFO(g_logger) << rt << " events happend.";
    return rt;
}
int EpollPoller::addEvent(Channel* channel, uint32_t event)           
{
    int fd = channel->fd();
    MutexLockGuard lock(mutex_);

    int oldEvent = channel->getEvents();
    epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));    
    ev.data.ptr = channel;                   //
    ev.events = oldEvent | event | EPOLLET;
    auto it = channelMap_.find(fd);
    int rt = 0;
    if(it == channelMap_.end())
    {
        channelMap_[fd] = channel;
        rt = epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev);
    }
    else
    {
        rt = epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev);
    }
    if(rt < 0)
    {
        LOG_ERROR(g_logger) << "EpollPoller::addEvent() fails. fd[" << fd << "] errno[" << errno << "].";
    } 
    channel->setEvents(ev.events);
    return rt;
}
void EpollPoller::delEvent(Channel* channel)          
{
    int fd = channel->fd();
    epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));

    MutexLockGuard lock(mutex_);

    auto it = channelMap_.find(fd);
    if(it == channelMap_.end())
    {
        LOG_WARN(g_logger) << "EpollPoller::delEvent() warn . not find fd.";
        return ;
    }
    int rt = epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, nullptr);
    if (rt < 0)
    {
        LOG_ERROR(g_logger) << "EpollPoller::delEvent() fails. errno[" << errno << "].";
        return ;
    }
    channel->setEvents(0);
}
int EpollPoller::updateEvent(Channel* channel, int opt, uint32_t event)
{
    if(opt == EPOLL_CTL_DEL)
    {
        LOG_WARN(g_logger) << "EpollPoller::updateEvent call EpollPoller::delEvent.";
        delEvent(channel);
        return 0;
    }

    int fd = channel->fd();
    int oldEvent = channel->getEvents();
    epoll_event ev;
    memset(&ev, 0, sizeof(epoll_event));
    MutexLockGuard lock(mutex_);
    ev.data.ptr = channel;
    ev.events = event | EPOLLET;
    auto it = channelMap_.find(fd);
    
    if(it == channelMap_.end())
    {
        LOG_WARN(g_logger) << "EpollPoller::updateEvent() warn. not find fd.";
        return 0;
    }
    int rt = epoll_ctl(epfd_, opt, fd, &ev);
    if (rt < 0)
    {
        LOG_ERROR(g_logger) << "EpollPoller::delEvent() fails. errno[" << errno << "].";
        return rt;
    }
    channel->setEvents(ev.events);
    return 0;
}
int EpollPoller::fd()                                  
{
    return epfd_;
}

void EpollPoller::removeChannel(int fd)
{
    MutexLockGuard lock(mutex_);
    auto it = channelMap_.find(fd);
    if(it == channelMap_.end())
    {
        LOG_WARN(g_logger) << "EpollPoller::removeChannel() warn. not find fd.";
        return;
    }    
    channelMap_.erase(it);
    return ;
}

}