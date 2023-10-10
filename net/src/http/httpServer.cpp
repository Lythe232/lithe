#include "include/http/httpServer.h"

namespace lithe
{

HttpServer::HttpServer(std::shared_ptr<IPAddress> addr, int numThread) :
    server_(addr, numThread)
{
    server_.setConnectionCallback(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
    server_.start();
}

void HttpServer::onConnection(std::shared_ptr<TcpConnection> connection)
{
    if(connection->getState() == TcpConnection::Connected)
    {
        printf("Connected\n");
    }
    else if(connection->getState() == TcpConnection::Disconnected)
    {
        printf("Disconnected\n");
    }
}

void HttpServer::onMessage(std::shared_ptr<TcpConnection> connection, std::shared_ptr<Buffer> buffer, Timestamp ts)
{
        std::shared_ptr<HttpSession> session = std::make_shared<HttpSession>(connection);
        session->recvRequest(buffer);
        std::shared_ptr<HttpResponse> res = std::make_shared<HttpResponse>();
        res->makeOkResponse("<!DOCTYPE html><html lang=\"en\">    \
                                <head>    \
                                    <meta charset=\"UTF-8\">     \
                                    <title>Title</title>   \
                                </head>  \
                                <body>   \
                                    <h1 style=\"text-align: center;\">Welcome</h1>   \
                                    <h2 style=\"text-align: center;\">Hello lithe</h2>    \
                                </body> \
                                </html>");
        session->sendResponse(res);
}

}