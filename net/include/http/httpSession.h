#pragma once

#include "include/tcpConnection.h"
#include "include/buffer.h"
#include "include/http/httpRequest.h"
#include "include/http/httpResponse.h"

#include <functional>

namespace lithe
{

class HttpSession
{
public:
    HttpSession(std::shared_ptr<TcpConnection> connection);
    ~HttpSession();
    void recvRequest(std::shared_ptr<Buffer> buffer);
    void sendResponse(std::shared_ptr<HttpResponse> res);

    void onMessage(std::shared_ptr<TcpConnection> , std::shared_ptr<Buffer>, Timestamp ts);
    void onConnection(std::shared_ptr<TcpConnection> );

private:
    std::shared_ptr<TcpConnection> connection_;
};


}