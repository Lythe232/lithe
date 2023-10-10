#pragma once

#include "coScheduler.h"
#include "socket.h"
#include "coTcpConnection.h"

#include <vector>
#include <list>

namespace lithe
{

class CoTcpServer
{
public:
    CoTcpServer(int threadCount, std::shared_ptr<IPAddress> address);
    ~CoTcpServer();

    void start();

    void handleClient(std::shared_ptr<CoTcpConnection> client);
    void startAccept();

    void removeConnection(int fd);
    CoScheduler* getNextScheduler();
private:    

    CoScheduler* acceptor_;
    std::vector<CoScheduler*> schedulers_;
    int threadCount_;
    int nextScheduler_;
    bool started_;
    std::map<int, std::shared_ptr<CoTcpConnection>> connections_;
    std::shared_ptr<Socket> listenSocket_;
    std::shared_ptr<IPAddress> address_;
    std::list<std::shared_ptr<Coroutine>> idleCors_;
    Mutex mutex_;
};

}
