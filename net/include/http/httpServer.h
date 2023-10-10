#pragma once

#include "include/address.h"
#include "include/tcpServer.h"
#include "include/common/timestamp.h"
#include "include/http/httpSession.h"

#include <vector>
#include <string>

namespace lithe
{

class HttpServer 
{
public:
    HttpServer(std::shared_ptr<IPAddress> addr, int numThread);
    ~HttpServer();

    void start();

    void onMessage(std::shared_ptr<TcpConnection>, std::shared_ptr<Buffer> , Timestamp ts);
    void onConnection(std::shared_ptr<TcpConnection>);


private:

    TcpServer server_;

    std::string name_;
    std::vector<std::shared_ptr<HttpSession>> sessions_;

};


}