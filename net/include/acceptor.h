#pragma once
#include "include/address.h"
#include "include/channel.h"
#include "include/eventLoop.h"
#include "include/socket.h"

#include <functional>
#include <memory>

namespace lithe
{

class Acceptor
{
public:
    typedef std::function<void(std::shared_ptr<Socket>, std::shared_ptr<IPAddress>)> ConnectionCallback;
    Acceptor(EventLoop* loop, std::shared_ptr<IPAddress> listenAddr);
    ~Acceptor();

    bool listenning() { return listenning_; }
    void listen();
    void setNewConnectionCallback(const ConnectionCallback& cb) { newConnectionCallback_  = cb; }

private:
    void handleRead();
    
    EventLoop* loop_;
    std::shared_ptr<IPAddress> listenAddr_;
    std::shared_ptr<Socket> listenSocket_;
    Channel* listenChannel_;
    ConnectionCallback newConnectionCallback_;
    int idleFd_;
    bool listenning_;
};


}