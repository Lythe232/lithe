#include "include/http/httpRequest.h"
#include "include/util/string_util.h"
#include "include/log/log.h"


#include <string.h>
#include <assert.h>

namespace lithe
{

static auto g_logger  = LOG_ROOT(); 
 
const std::set<std::string> HttpRequest::methods_ 
{
    "GET",     "HEAD",    "POST",  "PUT",   "DELETE",
    "CONNECT", "OPTIONS", "TRACE", "PATCH", "PRI"
};

HttpRequest::HttpRequest() : 
    method_(""),
    version_(""),
    url_(""),
    path_(""),
    body_(""),
    bodyLength_(0),
    bodyOffset_(0),
    error_(0)
{
}

HttpRequest::HttpRequest(std::string method, std::string path, std::string version) : 
    method_(method),
    version_(version),
    url_(""),
    path_(path),
    query_(""),
    body_(""),
    bodyLength_(0),
    bodyOffset_(0),
    error_(0)
{
}

void HttpRequest::setBegin(std::string method, std::string url, std::string version)
{
    method_ = method;
    url_ = url;
    version_ = version;
}

void HttpRequest::setHeader(std::string key, std::string value)
{
    headers_.insert ({key, value});
}

void HttpRequest::setCookie(std::string name, std::string value)
{
    if(cookie_.empty())
    {
        cookie_ += (name + "=" + value);
    }
    else
    {
        cookie_ += (";" + name + "=" + value);
    }
    cookies_.insert({name, value});
}

// void HttpRequest::addCookie(std::string name, std::string value)
// {
//     assert(!cookie_.empty());
//     cookie_ += (";" + name + "=" + value);
//     cookies_.insert({name, value});
// }

void HttpRequest::setBody(std::string body)
{
    char buf[32];
    util::convert(buf, body.size());
    setHeader("Content-Length", buf);
    body_ = body;
}


std::string HttpRequest::getHeader(std::string key)
{
    auto it = headers_.find(key);
    if(it == headers_.end())
    {
        return std::string();
    }
    return it->second;
}

HttpRequest& HttpRequest::makeGetRequest(std::string path)
{
    clear();
    setBegin("GET", path);
    setBody();
    return *this;
}

HttpRequest& HttpRequest::makePostRequest(std::string path, std::string content, std::string contentType)
{
    clear();
    setBegin("POST", path);
    if(!contentType.empty())
    {
        setHeader("Content-Type", contentType);
    }
    setBody(content);
    return *this;
}

HttpRequest& HttpRequest::makePutRequest(std::string path, std::string content, std::string contentType)
{
    clear();
    setBegin("PUT", path);
    if(!contentType.empty())
    {
        setHeader("Content-Type", contentType);
    }
    setBody(content);
    return *this;

}

HttpRequest& HttpRequest::makeDeleteRequest(std::string path)
{
    clear();
    setBegin("DELETE", path);
    setBody();

    return *this;
}

std::string HttpRequest::dumpString()
{
    std::string cache;
    cache = method_ + " " + url_ +  " " + version_ + "\r\n";

    for(auto& it : headers_)
    {
        cache += (it.first + ": " + it.second + "\r\n");
    }
    cache += "\r\n";
    if(!body_.empty())
    {
        cache += body_;
    }
    return cache;
}

void HttpRequest::clear()
{
    method_ = "";
    url_ = "";
    path_   = "/";
    query_  = "";
    cookie_ = "";
    body_   = "";

    headers_.clear();
    cookies_.clear();
    params_.clear();    
}

void HttpRequest::parseQuery(const std::string& str, std::unordered_map<std::string, std::string>& params)
{
    std::set<std::string> cache;
    util::split(&*str.begin(), &*(str.end() - 1), '&', [&](const char* begin, const char* end)
    {
        std::string kv(begin, end);
        if(cache.find(kv) != cache.end()) 
        {
            return ;
        }
        cache.insert(kv);

        std::string k;
        std::string v;

        util::split(begin, end, '=', [&](const char* begin1, const char* end1)
        {
            if(k.empty())
            {
                k.assign(begin1, end1);
            }
            else
            {
                v.assign(begin1, end1);
            }
        });
        if(!k.empty())
        {
            params.emplace(k, v);
        }
    });
}

bool HttpRequest::parseRequestLine(const char* line, HttpRequest& req)
{
    int len = strlen(line);
    if(len < 2 || line[len - 2] != '\r' || line[len - 1] != '\n')
    {
        error_ |= RL_END_NFD;
        return false;
    }
    len -= 2;
    std::string str2(line, line + len);
    int count = 0;
    util::split(line, line + len, ' ', 
        [&](const char* begin, const char* end)
        {
            switch(count)
            {
                case 0:
                    req.method_ = std::string(begin, end);
                    break;
                case 1:
                    req.url_ = std::string(begin, end);
                    break;
                case 2:
                    req.version_ = std::string(begin, end);
                    break;
                default:
                    break;
            }
            count ++;
        });
    if(count != 3)
    {
        error_ |= RL_PAR_ERR;
        return false;
    }
    if(req.version_ != "HTTP/1.1" && req.version_ != "HTTP/1.0")
    {
        error_ |= VS_ERR;
        return false;
    }
    if(methods_.find(req.method_) == methods_.end())
    {
        error_ |= MHD_ERR;
        return false;
    }

    for(size_t i = 0; i < req.url_.size(); i++)
    {
        if(req.url_[i] == '#')
        {
            req.url_.erase(i);
        }
    }

    int count1 = 0;
    util::split(&*req.url_.begin(), &*req.url_.end(), '?', 
        [&](const char* begin, const char* end)
        {
            switch(count1)
            {
                case 0:
                    req.path_ = std::string(begin, end);
                    break;
                case 1:
                    parseQuery(std::string(begin, end), req.params_);
                    break;
            }
            count1++;

        }
    );
    return true;
}

bool HttpRequest::parseHeader(const char* begin, const char* end, std::unordered_map<std::string, std::string>& headers)
{
    while(begin < end && util::isSpace(end[-1]))
    {
        --end;
    }

    const char* p = begin;
    while(p < end && *p != ':')
    {
        p++;
    }

    if(p == end)
    {
        error_ |= HDR_ERR;
        return false;
    }
    const char* name_end = p;

    if(*p++ != ':')
    {
        error_ |= HDR_ERR;
        return false;
    }
    while(p < end && util::isSpace(*p))
    {
        p++;
    }
    if(p < end)
    {
        std::string key = std::string(begin, name_end);
        std::string value = std::string(p, end);

        headers.emplace(key, value);
        return true;
    }
    error_ |= HDR_ERR;

    return false;
}

bool HttpRequest::recvRequest(const char* data, size_t size)
{
    const char* p = data;
    size_t request_line_end = 0;
    size_t index = 0;
    while(p[index] != '\r')
    {
        index ++;
        request_line_end++;
        if(index >= size)
        {
            std::string str(data, size);
            printf("str data = %s, size = %d\n", str.c_str(), size);
            LOG_ERROR(g_logger )  << "HttpRequest::recvRequest request line error.";
            return false;
        }
    }
    index++;
    if(index >= size || p[index] != '\n')
    {
        LOG_ERROR(g_logger )  << "HttpRequest::recvRequest request line error.";
        return false;
    }
    index++;
    request_line_end += 2;
    if(index >= size)
    {
        LOG_ERROR(g_logger )  << "HttpRequest::recvRequest request line error.";
        return false;
    }
    std::string rl(data, request_line_end);
    if(!parseRequestLine(rl.c_str(), *this))    ///TODO FIXME 
    {
        LOG_ERROR(g_logger) << "HttpRequest::recvRequest() parse request line error.";
        return false;
    }
    //index = header begin
    while(index + 3 < size && !(p[index] == '\r' && p[index+1] == '\n' && p[index+2] == '\r' && p[index+3] == '\n'))
    {
        size_t header_begin = index;
        size_t header_end = index;
        while(!(p[index] == '\r' && p[index+1] == '\n'))
        {
            index++;
            header_end++;
            if(index + 1 >= size)
            {
                error_ |= HDR_ERR;

                return false;
            }
        }

        if (index + 3 < size && p[index+2] == '\r' && p[index+3] == '\n') 
        {
            {
                parseHeader(data + header_begin, data + header_end, headers_);
            }
            break;
        }
        index += 2;
        if(index >= size)
        {
            error_ |= HDR_ERR;

            return false;
        }
        {
            parseHeader(data + header_begin, data + header_end, headers_);
        }
    }

    if(index + 3 >= size)
    {
        error_ |= HDR_ERR;
        return false;
    }

    //index = body begin
    index += 3;
    auto cl = headers_.find("Content-Length");
    if(cl != headers_.end())
    {
        index ++;
        bodyOffset_ = index;
        bodyLength_ = ::atoi(cl->second.c_str());
    }

    auto ck = headers_.find("Cookie");

    if(ck != headers_.end())
    {
        util::split(&*(ck->second.begin()), &*(ck->second.end() - 1), ';', [&](const char* begin, const char* end)
        {
            std::string name;
            std::string value;
            util::split(begin, end, '=', [&](const char* begin1, const char* end1){
                if(name.empty())
                {
                    name.assign(begin1, end1);
                }
                else
                {
                    value.assign(begin1, end1);
                }

            });
            if(!name.empty())
            {
                setCookie(name, value);
            }
        });
    }
    return true;
}

void HttpRequest::printfError()
{
    printf("============================================================\n");
    if(!error_)
        printf("No error.\n");
    if(error_ & RL_END_NFD)
        printf("http request line not found \\r\\n.\n");
    if(error_ & RL_PAR_ERR)
        printf("http request line parse error.\n");
    if(error_ & VS_ERR)
        printf("http version error.\n");
    if(error_ & MHD_ERR)
        printf("http method error.\n");
    if(error_ & HDR_ERR)
        printf("http header parse error.\n");
    printf("============================================================\n");
}

}