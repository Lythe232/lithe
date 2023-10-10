#pragma once

#include "thread/mutex.h"
#include "sys/epoll.h"
#include "log/log.h"
#include <vector>
#include <map>

namespace lithe
{
class CoEpollItem;
class CoScheduler;
class CoEpoll
{
public:
    CoEpoll();
    CoEpoll(CoScheduler* scheduler);
    ~CoEpoll();
    
    int poll(uint64_t timeout, std::vector<CoEpollItem*>* items);
    int addEvent(int fd, int events);
    void delEvent(int fd);
    void cancelEvent(int fd, int event);
    void cancelAllEvent(int fd);
    int updateEvent(int opt, int events, int fd);

    void addItem(CoEpollItem* item);
    void removeItem(int fd);
private:
    static const int InitEventSize = 256;

    int epfd_;
    CoScheduler* scheduler_;
    std::vector<epoll_event> epevents_;
    std::map<int, CoEpollItem*> itemMaps_;
    Mutex mutex_;
};


}