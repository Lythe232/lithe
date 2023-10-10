#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>

namespace lithe
{

class HttpRequest
{
public:
    enum ErrorState
    {
        NOT_ERR = 0,
        RL_END_NFD = 1,
        RL_PAR_ERR = 2,
        VS_ERR = 4,
        MHD_ERR = 8,
        HDR_ERR = 16
    };
    static const std::set<std::string> methods_;
    HttpRequest();
    HttpRequest(std::string method, std::string path, std::string version = "HTTP/1.1");
    HttpRequest(const HttpRequest&) = default;
    HttpRequest(HttpRequest&&) = default;
    ~HttpRequest() = default;

    HttpRequest& operator=(const HttpRequest&) = default;
    HttpRequest& operator=(HttpRequest&&) = default;

    void setBegin(std::string method, std::string path, std::string version = "HTTP/1.1");
    void setHeader(std::string key, std::string value);
    void setQuery(std::string query) { query_ = query; }
    void setCookie(std::string name, std::string value);
    // void addCookie(std::string name, std::string value);
    void setBody(std::string body = "");

    std::string getVersion() { return version_; }
    std::string getMethod(){ return method_; }

    size_t getHeadersCount() { return headers_.size(); }
    std::string getHeader(std::string key);
    std::string getBody() { return body_; }
    size_t getBodyLength() { return bodyLength_; }

    HttpRequest& makeGetRequest(std::string path);
    HttpRequest& makePostRequest(std::string path, std::string content, std::string contentType);
    HttpRequest& makePutRequest(std::string path, std::string content, std::string contentType);
    HttpRequest& makeDeleteRequest(std::string path);

    std::string dumpString();
    void clear();

    void parseQuery(const std::string& str, std::unordered_map<std::string, std::string>& params);
    bool parseRequestLine(const char*, HttpRequest&);
    bool parseHeader(const char* begin, const char* end, std::unordered_map<std::string, std::string>& headers);

    bool recvRequest(const char* data, size_t size);

    bool hasError() { return error_; }
    bool has_req_line_crlf() { return !(error_ & RL_END_NFD); }
    bool has_req_line_params_err() { return error_ & RL_PAR_ERR; }
    bool has_version_error() { return error_ & VS_ERR; }
    bool has_method_error() { return error_ & MHD_ERR; }
    bool has_header_error() { return error_ & HDR_ERR; }

    void printfError();
private:

    std::string method_;
    std::string version_;

    std::string url_;
    std::string path_;
    std::string query_;
    std::string cookie_;    //cookie string
    std::string body_;

    size_t bodyLength_;
    size_t bodyOffset_;

    //0000 0001     "\r\n" of the request line not found
    //0000 0010     error in request line params
    //0000 0100     version error
    //0000 1000     method error
    //0001 0000     header error
    //0010 0000
    //0100 0000
    //1000 0000
    uint8_t error_;

    std::unordered_map<std::string, std::string> headers_;
    std::unordered_map<std::string, std::string> cookies_;
    std::unordered_map<std::string, std::string> params_;

};


}