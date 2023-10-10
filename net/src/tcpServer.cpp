#include "include/tcpServer.h"

namespace lithe
{

TcpServer::TcpServer(std::shared_ptr<IPAddress> addr, int numThread) : 
    threadPool_(new EventLoopThreadPool(numThread)),
    baseLoop_(threadPool_->getMainLoop()),
    acceptor_(new Acceptor(baseLoop_, addr)),
    writeCompleteCallback_(nullptr),
    connectionCallback_(nullptr),
    messageCallback_(nullptr),
    address_(addr),
    started_(false)
{
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    if(acceptor_)
    {
        delete acceptor_;
        acceptor_ = nullptr;
    }
}

void TcpServer::start()
{
    if(!started_)
    {
        started_ = true;
        threadPool_->start();
    }
    if(!acceptor_->listenning())
    {
        baseLoop_->runInLoop(std::bind(&Acceptor::listen, acceptor_));
    }
    ::strcpy(CurrentThread::tidString, "Acceptor");
    baseLoop_->loop();
}

void TcpServer::newConnection(std::shared_ptr<Socket> socket, std::shared_ptr<IPAddress> addr)
{
    auto loop = threadPool_->getNextLoop();

    std::shared_ptr<TcpConnection> connection(new TcpConnection(loop, socket, addr));
    connections_[socket->getSocket()] = connection;
    connection->setConnectionCallback(connectionCallback_);
    connection->setMessageCallback(messageCallback_);
    connection->setWriteCompleteCallback(writeCompleteCallback_);
    connection->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
    loop->runInLoop(std::bind(&TcpConnection::connectEstablished, connection));
}

void TcpServer::removeConnection(const std::shared_ptr<TcpConnection>& connection)
{
    baseLoop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, connection));
}

void TcpServer::removeConnectionInLoop(std::shared_ptr<TcpConnection>& connection)
{
    baseLoop_->assertNotInLoopThread();

    connections_.erase(connection->getSocket()->getSocket());
    EventLoop* loop = connection->getLoop();
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, connection));
}

};