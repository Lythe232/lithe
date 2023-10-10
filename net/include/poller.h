#pragma once

#include "coroutine.h"

#include <sys/poll.h>
#include <sys/epoll.h>

namespace lithe
{
class Channel;
class CoScheduler;

class Poller
{
public:
    typedef std::vector<Channel*> ChannelList;
    Poller();
    virtual ~Poller();

    virtual int poll(uint64_t, ChannelList*) = 0;

    virtual int addEvent(Channel*, uint32_t) = 0;

    virtual void delEvent(Channel*) = 0;

    virtual int updateEvent(Channel*, int, uint32_t) = 0;

    virtual void removeChannel(int) = 0;

    virtual int fd();

};

}   //namespace lithe