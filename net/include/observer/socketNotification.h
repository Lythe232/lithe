#pragma once

#include "notification.h"


namespace lithe
{

class EventLoop;

class SocketNotification : public Notification
{
public:

    SocketNotification(EventLoop* loop);
    virtual ~SocketNotification();

    std::shared_ptr<Socket> socket() const { return socket_; }

private:
    setSocket(std::shared_ptr<Socket> socket) { socket_ = socket; }
    
    EventLoop* loop_;
    std::shared_ptr<Socket> socket_;

    friend class SocketNotifier;
}


class ReadNotification : public SocketNotification
{
public:
    ReadNotification(EventLoop* loop);
    ~ReadNotification();
};

class WritNotification : public SocketNotification
{
public:
    WritNotification(EventLoop* loop);
    ~WritNotification();
};

class ErrorNotification : public SocketNotification
{
public:
    ErrorNotification(EventLoop* loop);
    ~ErrorNotification();
}

}
