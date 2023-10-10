#pragma once

#include "coEpoll.h"
#include "coEpollItem.h"
#include "buffer.h"
#include "socket.h"
#include "address.h"
#include "coScheduler.h"


namespace lithe
{

class CoTcpConnection
{
public:
    enum State 
    {
        Connecting,
        DisConnecting,
        Connected,
        DisConnected
    };

    CoTcpConnection(std::shared_ptr<Socket> socket, CoScheduler* scheduler);
    ~CoTcpConnection();
    int fd() { return fd_; }
    ssize_t send(const char* data, int len);
    ssize_t send(std::string& data);
    ssize_t send(std::shared_ptr<Buffer> buffer);
    ssize_t recv(char* data, int len);

    void input();
    void output();
    void process();

    void establishingConnect();
    void destoryConnect();

    void setState(State state) { state_ = state; }
    State getState() { return state_; }

    bool connected() const { return state_ == Connected; }

    std::shared_ptr<Buffer> getRecvBuffer() { return recvBuffer_; }

    void setTcpNoDelay(bool flag);
private:
    int fd_;
    State state_;
    std::shared_ptr<Socket> socket_;
    CoScheduler* scheduler_;
    CoEpoll* poller_;
    CoEpollItem* channel_;
    std::shared_ptr<IPAddress> localAddr_;
    std::shared_ptr<IPAddress> remoteAddr_;

    std::shared_ptr<Buffer> recvBuffer_;
    std::shared_ptr<Buffer> sendBuffer_;
};

}