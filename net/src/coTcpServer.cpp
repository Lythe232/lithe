#include "include/coTcpServer.h"


namespace lithe
{

CoTcpServer::CoTcpServer(int threadCount, std::shared_ptr<IPAddress> address) : 
    acceptor_(new CoScheduler("Acceptor")),
    threadCount_(threadCount),
    nextScheduler_(0),
    started_(false),
    listenSocket_(new Socket(AF_INET, SOCK_STREAM, 0)),
    address_(address),
    mutex_()
{
    listenSocket_->setTcpNoDelay(true);
    listenSocket_->setReuseAddr(true);
    listenSocket_->setReusePort(true);
    schedulers_.resize(threadCount_);
    for(int i = 0; i < threadCount_; i++)
    {
        schedulers_[i] = new CoScheduler("IOWorker");
    }
}

CoTcpServer::~CoTcpServer()
{
    for(int i = 0; i < threadCount_; i++)
    {
        if(schedulers_[i])
        {
            delete schedulers_[i];
        }
    }
}

void CoTcpServer::start()
{
    printf("Start...\n");
    started_ = true;
    listenSocket_->bind(address_);
    for(int i = 0; i < threadCount_; i++)
    {
        schedulers_[i]->start();
    }
    acceptor_->start();
    acceptor_->scheduleAndWakeUp(std::bind(&CoTcpServer::startAccept, this));
}

void CoTcpServer::handleClient(std::shared_ptr<CoTcpConnection> client)
{
    client->establishingConnect();

    while(client->connected())
    {
        client->input();
        client->process();
        client->output();
    }
    removeConnection(client->fd());
}

void CoTcpServer::startAccept()
{
    printf("StartAccpet... listenSocket[%d]\n", listenSocket_->getSocket());
    listenSocket_->listen(5);
    while(started_)
    {
        std::shared_ptr<IPAddress> peeraddr = std::make_shared<IPAddress>();
        CoScheduler* scheduler = getNextScheduler();
        auto cli = listenSocket_->accept(peeraddr);
        if(cli == nullptr)
        {
            continue;
        }
        std::shared_ptr<CoTcpConnection> connection(new CoTcpConnection(cli, scheduler));
        {
            MutexLockGuard lock(mutex_);
            connections_[cli->getSocket()] = connection;
        }
        cli->setRecvTimeout(2000);
        std::shared_ptr<Coroutine> cor = std::make_shared<Coroutine>(std::bind(&CoTcpServer::handleClient, this, connection));
        scheduler->scheduleAndWakeUp(cor);
    }
}

void CoTcpServer::removeConnection(int fd)
{
    MutexLockGuard lock(mutex_);
    connections_.erase(fd);
}

CoScheduler* CoTcpServer::getNextScheduler()
{
    int nextSche =  nextScheduler_ % threadCount_ ;
    nextScheduler_++;
    return schedulers_[nextSche];
}

}
