#include "include/http/httpSession.h"

#include <string>
namespace lithe
{

static auto g_logger = LOG_ROOT();


HttpSession::HttpSession(std::shared_ptr<TcpConnection> connection) :
    connection_(connection)
{
}

HttpSession::~HttpSession()
{
}


void HttpSession::recvRequest(std::shared_ptr<Buffer> buffer)
{
    std::shared_ptr<HttpRequest> request(new HttpRequest());
    std::string data = buffer->fetchAllAsString();  //TODO 读取的同时将缓冲区内的数据也清空了，下次再调用时就没有之前的数据了
    printf("HttpSession::recvRequest data = %s\n", data.c_str());
    request->recvRequest(data.c_str(), data.size());
    if(!request->has_req_line_crlf())
    {
        assert(0);
        return ;
    }
    if(request->hasError())
    {
        LOG_ERROR(g_logger) << "HttpSession::recvRequest() has error.";
        request->printfError();
        connection_->forceClose();
    }
}

void HttpSession::sendResponse(std::shared_ptr<HttpResponse> res)
{
    std::string str = res->dumpString();
    connection_->send(str);
}


}