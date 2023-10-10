#pragma once

#include "socket.h"
#include "channel.h"
#include "byteStream.h"
#include "address.h"
#include "buffer.h"

#include <memory>

namespace lithe
{

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    enum State
    {
        Disconnected,
        Disconnecting,
        Connecting,
        Connected
    };
    typedef std::function<void(std::shared_ptr<TcpConnection>)> WriteCompleteCallback;
    typedef std::function<void(std::shared_ptr<TcpConnection>)> CloseCallback;
    typedef std::function<void(std::shared_ptr<TcpConnection>)> ConnectionCallback;
    typedef std::function<void(std::shared_ptr<TcpConnection>, std::shared_ptr<Buffer>, Timestamp)> MessageCallback;

    TcpConnection(EventLoop* loop, int sockfd, std::shared_ptr<IPAddress> localAddr, std::shared_ptr<IPAddress> peerAddr);
    TcpConnection(EventLoop* loop, std::shared_ptr<Socket> socket, std::shared_ptr<IPAddress> peerAddr);
    ~TcpConnection();


    void send(const std::string& str);  
    void send(const char* data, size_t len);
    void send(std::shared_ptr<Buffer> buffer);
    void shutdown();
    void forceClose();
    std::shared_ptr<Socket> getSocket() { return socket_; }
    void setState(State state) { state_ = state; }
    State getState() { return state_; }
    EventLoop* getLoop() { return loop_; }
    void setTcpNoDelay(bool flag);
    void setRecvTimeout(uint64_t timeout)
    {
        recvTimeout_ = timeout;
    }
    void setSendTimeout(uint64_t timeout)
    {
        sendTimeout_ = timeout;
    }
    void setWriteCompleteCallback(WriteCompleteCallback callback)
    {
        writeCompleteCallback_ = callback;
    }
    void setCloseCallback(CloseCallback callback)
    {
        closeCallback_ = callback;
    }
    void setConnectionCallback(ConnectionCallback callback)
    {
        connectionCallback_ = callback;
    }
    void setMessageCallback(MessageCallback callback)
    {
        messageCallback_ = callback;
    }
    void connectEstablished();
    void connectDestroyed();

    std::shared_ptr<IPAddress> getLocalAddr()
    {
        return localAddr_;
    }
    std::shared_ptr<IPAddress> getPeerAddr()
    {
        return peerAddr_;
    }



private:
    void sendInLoop(const char* data, size_t len);
    void sendInLoop(const std::string& str);
    
    //only used by scheduler_
    void handleRead();
    void handleWrite();
    void handleClose();
    void handleError();
    
    WriteCompleteCallback writeCompleteCallback_;
    CloseCallback         closeCallback_;
    ConnectionCallback    connectionCallback_;
    MessageCallback       messageCallback_;
    
    EventLoop* loop_;
    State state_;
    std::shared_ptr<IPAddress> localAddr_;
    std::shared_ptr<IPAddress> peerAddr_;
    std::shared_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    std::shared_ptr<Buffer> inputBuf_;
    std::shared_ptr<Buffer> outputBuf_;
    size_t   highWaterMark_;
    uint64_t recvTimeout_;
    uint64_t sendTimeout_;
};

}
