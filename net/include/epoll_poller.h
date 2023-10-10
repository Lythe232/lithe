#pragma once

#include "poller.h"
#include "socket.h"

#include <sys/epoll.h>
#include <map>
#include <vector>
namespace lithe
{
class Channel;
class EpollPoller : public Poller 
{
public:

    EpollPoller();
    ~EpollPoller();
    int fd()                                                    override;
    int poll(uint64_t timeout, Poller::ChannelList*)            override;
    void delEvent(Channel* channel)                             override;
    int addEvent(Channel* channel, uint32_t event)              override;
    int updateEvent(Channel* channel, int opt, uint32_t event)  override;
    void removeChannel(int fd);
private:
    static const int MAX_EVENT = 256;
    int epfd_;
    std::vector<struct epoll_event> epevents_;
    std::map<int, Channel*> channelMap_;
    Mutex mutex_;
};

}   //namespace lithe
