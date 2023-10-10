#include "socket.h"
#include "coroutine/coTcpServer.cpp"
#include "address.h"
#include <signal.h>
class IgnoreSigPipe
{
public: 
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
};
IgnoreSigPipe initObj;

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
#include <mcheck.h>
int main()
{   
    setenv("MALLOC_TRACE", "test.log", 1);
    mtrace();
    std::shared_ptr<IPAddress> address = std::make_shared<IPAddress>(AF_INET, "0.0.0.0", 5555);
    EchoServer* server = new EchoServer(2, address, "EchoServer");
    server->start();
    while(1)
    {
    }
    muntrace();
}