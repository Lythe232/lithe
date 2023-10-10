#pragma once

#include <string>
#include <unordered_map>

namespace lithe
{

class HttpResponse
{
public:
    HttpResponse();
    HttpResponse(int status, std::string version = "HTTP/1.1");
    HttpResponse(const HttpResponse&) = default;
    HttpResponse(HttpResponse&&) = default;

    HttpResponse& operator=(const HttpResponse&) = default;
    HttpResponse& operator=(HttpResponse&&) = default;

    ~HttpResponse() = default;

    void setBegin(int status, std::string version = "HTTP/1.1");
    void setBegin(int status, std::string phrase, std::string version = "HTTP/1.1");
    void setHeader(std::string name, std::string value);
    void setBody(std::string body = "");

    void setVersion(std::string version);
    void setStatus(int status);
    void setBodyLength(size_t length);
    void setContentType(std::string name);
    void setCookie(std::string name, std::string value, size_t maxAge, 
                    std::string path, std::string domain, bool secure, bool strict, bool httpOnly);

    std::string getVersion() { return version_; }
    int getStatus() { return status_; }
    std::string getStatusAsString(int status);
    std::string getBody();
    int getBodyLength();

    HttpResponse& makeOkResponse(std::string content = "");
    HttpResponse& makeErrorResponse(int status, std::string content = "", std::string contentType = "text/plain; charset=UTF-8");
    HttpResponse& makeGetResponse(std::string content = "", std::string contentType = "text/plain; charset=UTF-8");

    void clear();
    std::string dumpString();

    bool parseResponseLine(const char*, HttpResponse& response);
    bool parseHeader(const char* , const char*, std::unordered_map<std::string, std::string>& headers);

    bool recvResponse(const char*, size_t size);
private:
    static const std::unordered_map<std::string, std::string> mimeMap_;
    static const std::unordered_map<int, std::string> statusMap_;

    std::string version_;
    int status_;
    std::string phrase_;
    std::string body_;
    size_t bodyLength_;
    size_t bodyOffset_;
    std::unordered_map<std::string, std::string> headers_;
};


}
