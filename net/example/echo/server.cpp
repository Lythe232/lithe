#include "include/socket.h"
#include "include/coScheduler.h"
#include "include/address.h"
#include "include/tcpServer.h"
#include "include/http/httpRequest.h"
#include "include/http/httpResponse.h"
#include "include/util/string_util.h"

#include <iostream>
#include <signal.h>
#include <string.h>
#include <string>
using namespace lithe;
static auto g_logger = LOG_ROOT();

class EchoServer
{
public:
    EchoServer(std::shared_ptr<IPAddress> addr, int threadNum, std::string name) :
        server_(addr, threadNum),
        name_(name)
    {
        //二进制兼容性，使用闭包来代替虚函数
        server_.setMessageCallback(std::bind(&EchoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        server_.setConnectionCallback(std::bind(&EchoServer::onConnection, this, std::placeholders::_1));
    }
    void onMessage(std::shared_ptr<TcpConnection> connection, std::shared_ptr<Buffer> buffer, Timestamp ts)
    {
        printf("buffer->readableSize = %ld\n", buffer->readableSize());
        std::string peerAddr = connection->getPeerAddr()->toString();

        // printf("host: %s. recv data: %s\n", peerAddr.c_str(), str.c_str());
        // std::string* str = new std::string(buffer->fetchAllAsString());
        std::string str(buffer->fetchAllAsString());
        connection->send(str);
        connection->forceClose();
        printf("END--------------------------------------\n");
    }
    void onConnection(std::shared_ptr<TcpConnection> connection)
    {
        if(connection->getState() == TcpConnection::Disconnected)
        {
            printf("连接已经断开\n");
        }
        else if(connection->getState() == TcpConnection::TcpConnection::Connected)
        {
            printf("连接已经建立\n");
        }
    }
    void start()
    {
        server_.start();
    }
private:
    TcpServer server_;      
    std::string name_;
};
int main()
{

    std::shared_ptr<IPAddress> address(new IPAddress(AF_INET, "0.0.0.0", 5555));
    HttpRequest* req = new HttpRequest("GET", "/");
    // req->setCookie("lang", "en_US");
    // std::string str = req->makePostRequest("/", "HELOO WORLD", "json/text").dumpString();
    // printf("Request: %s\n", str.c_str());

    // HttpResponse* resp = new HttpResponse(200);
    // std::string str1 = resp->makeOkResponse().dumpString();

    // printf("Response: %s\n", str1.c_str());
    // lithe::util::split(buf, buf + strlen(buf), ' ',
    //     ([&](const char* a, const char* b)
    //     {
    //         std::string str(a, b);
    //         std::cout << str <<  std::endl;
    //     })
    // );
    std::unordered_map<std::string, std::string> strmap;
    std::string s = "GET http://www.example.com/index.html?param1=value1&param2=value2 HTTP/1.1\r\n";
    std::string s1 = "Location: local";
    std::string s2 = "GET /example HTTP/1.1\r\nHost: example.com\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/94.0.4606.71 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\nAccept-Language: en-US,en;q=0.9\r\nCookie: PHPSESSID=abcdef1234567890; UserName=testuser\r\n\r\n";
    std::string s3 = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.149 Safari/537.36\r\nAccept-Language: en-US,en;q=0.9\r\nConnection: keep-alive\r\n"; //Content-Length: 31\r\n\r\n1234567891011121314151617181920
    req->recvRequest(s3.c_str(), s3.size());
    std::cout << req->dumpString() << std::endl;
    // const char* buf = "HTTP/1.1 200 OK";
    // HttpResponse* res = new HttpResponse(200);

    // std::string s4 = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 134\r\nConnection: keep-alive\r\n\r\n<!DOCTYPE html><html><head><title>Example Page</title></head><body><h1>Hello, World!</h1><p>This is an example page.</p></body></html>";
    // res->recvResponse(s4.c_str(), s4.size());
    // std::cout << res->dumpString() << std::endl;

    EchoServer server(address, 2, "EchoServer");           
    server.start();
}

