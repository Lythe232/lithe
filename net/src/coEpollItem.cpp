#include "include/coEpollItem.h"
#include "include/coScheduler.h"

namespace lithe
{

static auto g_logger = LOG_ROOT();

CoEpollItem::CoEpollItem(int fd, CoScheduler* scheduler) :
    fd_(fd),
    events_(0),
    revents_(0),
    scheduler_(scheduler),
    poller_(scheduler_->getPoller()),
    cor_(nullptr)
{
}

CoEpollItem::~CoEpollItem()
{
}

void CoEpollItem::processEvent()
{
    assert(cor_->getState() == Coroutine::READY || cor_->getState() == Coroutine::RUNNING);
    if(cor_)
    {
        scheduler_->schedule(cor_);
    }
    else
    {
        LOG_WARN(g_logger) << "CoEpollItem::processEvent warn. coroutine is null.";
        return ;
    }
    revents_ = 0;
    poller_->delEvent(fd_);
    // poller_->cancelAllEvent(fd_);
    // poller_->cancelEvent(fd_, event);
}

void CoEpollItem::setCoroutine(std::shared_ptr<Coroutine> cor)
{
    // if(cor_->getState() == Coroutine::RUNNING || cor_->getState() == Coroutine::READY)
    // {
    //     LOG_WARN(g_logger) << "CoEpollItem::setCoroutine() warn.";
    // }
    cor_ = cor;
}

void CoEpollItem::disableReading()
{
    assert(revents_ & EPOLLIN);
    poller_->updateEvent(EPOLL_CTL_MOD, fd_, revents_ & ~EPOLLIN);
}

void CoEpollItem::enableReading()
{
    poller_->addEvent(fd_, EPOLLIN);
}

void CoEpollItem::disableWriting()
{
    assert(revents_ & EPOLLOUT);
    poller_->updateEvent(EPOLL_CTL_MOD, fd_, revents_ & ~EPOLLOUT);
}

void CoEpollItem::enableWriting()
{
    poller_->addEvent(fd_, EPOLLOUT);
}

void CoEpollItem::reset()
{
    assert(cor_->getState() != Coroutine::END || cor_->getState() != Coroutine::EXCEP);
    fd_ = -1;
    scheduler_ = nullptr;
    cor_ = nullptr;
}

}