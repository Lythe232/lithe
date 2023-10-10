#include "include/socket.h"
#include "include/coTcpServer.h"
#include "include/address.h"
#include <signal.h>

namespace lithe
{

class EchoServer
{
public:
    EchoServer(int threadCount, std::shared_ptr<IPAddress> address, std::string name) :
        server_(new CoTcpServer(threadCount, address)),
        name_(name)
    {

    }
    ~EchoServer();

    void start()
    {
        server_->start();
    }


private:
    CoTcpServer* server_;
    std::string name_;

};
}

using namespace lithe;
int main()
{   
    std::shared_ptr<IPAddress> address = std::make_shared<IPAddress>(AF_INET, "0.0.0.0", 5555);
    EchoServer* server = new EchoServer(2, address, "EchoServer");
    server->start();
    while(1)
    {
    }
}