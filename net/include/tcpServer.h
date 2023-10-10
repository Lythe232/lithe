#pragma once 

#include "eventLoop.h"
#include "tcpConnection.h"
#include "address.h"
#include "eventLoopThreadPool.h"
#include "acceptor.h"

#include <memory>
#include <functional>
#include <map>
namespace lithe
{

class TcpServer
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;


    TcpServer(std::shared_ptr<IPAddress> addr, int numThread);
    ~TcpServer();

    void start();

    void newConnection(std::shared_ptr<Socket>, std::shared_ptr<IPAddress>);

    void setConnectionCallback(const TcpConnection::ConnectionCallback& cb) { connectionCallback_ = cb; }

    void setWriteCompleteCallback(const TcpConnection::WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

    void setMessageCallback(const TcpConnection::MessageCallback& cb) { messageCallback_ = cb; }

    void removeConnection(const std::shared_ptr<TcpConnection>& connection);

    void removeConnectionInLoop(std::shared_ptr<TcpConnection>& connection);

private:
    std::unique_ptr<EventLoopThreadPool> threadPool_;
    EventLoop* baseLoop_;
    Acceptor* acceptor_;

    TcpConnection::WriteCompleteCallback   writeCompleteCallback_;
    TcpConnection::ConnectionCallback      connectionCallback_;
    TcpConnection::MessageCallback         messageCallback_;

    std::map<int, TcpConnectionPtr > connections_;
    std::shared_ptr<IPAddress> address_;
    bool started_;
};

}