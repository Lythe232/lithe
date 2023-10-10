#pragma once

namespace lithe
{

class SocketNotifier
{
public:
    SocketNotifier(std::shared_ptr<Socket> socket);
    ~SocketNotifier();

    void addObserver(AbstractObserver& observer);
    void removeObserver(AbstractObserver& observer);


private:

    NotificationCenter* nc_;
    std::shared_ptr<Socket> socket_;

};


}