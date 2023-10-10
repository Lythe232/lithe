#include "include/http/httpResponse.h"
#include "include/http/httpRequest.h"
#include "include/util/string_util.h"

#include <assert.h>
#include <string.h>
#include <algorithm>

namespace lithe
{

const std::unordered_map<std::string, std::string> HttpResponse::mimeMap_ = 
{
    // Base content types
    { ".html",      "text/html" },
    { ".css",       "text/css" },
    { ".js",        "text/javascript" },
    { ".vue",       "text/html" },
    { ".xml",       "text/xml" },

    // Application content types
    { ".atom",      "application/atom+xml" },
    { ".fastsoap",  "application/fastsoap" },
    { ".gzip",      "application/gzip" },
    { ".json",      "application/json" },
    { ".map",       "application/json" },
    { ".pdf",       "application/pdf" },
    { ".ps",        "application/postscript" },
    { ".soap",      "application/soap+xml" },
    { ".sql",       "application/sql" },
    { ".xslt",      "application/xslt+xml" },
    { ".zip",       "application/zip" },
    { ".zlib",      "application/zlib" },

    // Audio content types
    { ".aac",       "audio/aac" },
    { ".ac3",       "audio/ac3" },
    { ".mp3",       "audio/mpeg" },
    { ".ogg",       "audio/ogg" },

    // Font content types
    { ".ttf",       "font/ttf" },

    // Image content types
    { ".bmp",       "image/bmp" },
    { ".emf",       "image/emf" },
    { ".gif",       "image/gif" },
    { ".jpg",       "image/jpeg" },
    { ".jpm",       "image/jpm" },
    { ".jpx",       "image/jpx" },
    { ".jrx",       "image/jrx" },
    { ".png",       "image/png" },
    { ".svg",       "image/svg+xml" },
    { ".tiff",      "image/tiff" },
    { ".wmf",       "image/wmf" },

    // Message content types
    { ".http",      "message/http" },
    { ".s-http",    "message/s-http" },

    // Model content types
    { ".mesh",      "model/mesh" },
    { ".vrml",      "model/vrml" },

    // Text content types
    { ".csv",       "text/csv" },
    { ".plain",     "text/plain" },
    { ".richtext",  "text/richtext" },
    { ".rtf",       "text/rtf" },
    { ".rtx",       "text/rtx" },
    { ".sgml",      "text/sgml" },
    { ".strings",   "text/strings" },
    { ".url",       "text/uri-list" },

    // Video content types
    { ".H264",      "video/H264" },
    { ".H265",      "video/H265" },
    { ".mp4",       "video/mp4" },
    { ".mpeg",      "video/mpeg" },
    { ".raw",       "video/raw" }
};

const std::unordered_map<int, std::string> HttpResponse::statusMap_ = 
{
        { 100, "Continue"                       },
        { 101, "Switching Protocols"            },
        { 102, "Processing"                     },
        { 103, "Early Hints"                    },

        { 200, "OK"                             },
        { 201, "Created"                        },
        { 202, "Accepted"                       },
        { 203, "Non-Authoritative Information"  },
        { 204, "No Content"                     },
        { 205, "Reset Content"                  },
        { 206, "Partial Content"                },
        { 207, "Multi-Status"                   },
        { 208, "Already Reported"               },

        { 226, "IM Used"                        },

        { 300, "Multiple Choices"               },
        { 301, "Moved Permanently"              },
        { 302, "Found"                          },
        { 303, "See Other"                      },
        { 304, "Not Modified"                   },
        { 305, "Use Proxy"                      },
        { 306, "Switch Proxy"                   },
        { 307, "Temporary Redirect"             },
        { 308, "Permanent Redirect"             },

        { 400, "Bad Request"                    },
        { 401, "Unauthorized"                   },
        { 402, "Payment Required"               },
        { 403, "Forbidden"                      },
        { 404, "Not Found"                      },
        { 405, "Method Not Allowed"             },
        { 406, "Not Acceptable"                 },
        { 407, "Proxy Authentication Required"  },
        { 408, "Request Timeout"                },
        { 409, "Conflict"                       },
        { 410, "Gone"                           },
        { 411, "Length Required"                },
        { 412, "Precondition Failed"            },
        { 413, "Payload Too Large"              },
        { 414, "URI Too Long"                   },
        { 415, "Unsupported Media Type"         },
        { 416, "Range Not Satisfiable"          },
        { 417, "Expectation Failed"             },

        { 421, "Misdirected Request"            },
        { 422, "Unprocessable Entity"           },
        { 423, "Locked"                         },
        { 424, "Failed Dependency"              },
        { 425, "Too Early"                      },
        { 426, "Upgrade Required"               },
        { 427, "Unassigned"                     },
        { 428, "Precondition Required"          },
        { 429, "Too Many Requests"              },
        { 431, "Request Header Fields Too Large"},

        { 451, "Unavailable For Legal Reasons"  },

        { 500, "Internal Server Error"          },
        { 501, "Not Implemented"                },
        { 502, "Bad Gateway"                    },
        { 503, "Service Unavailable"            },
        { 504, "Gateway Timeout"                },
        { 505, "HTTP Version Not Supported"     },
        { 506, "Variant Also Negotiates"        },
        { 507, "Insufficient Storage"           },
        { 508, "Loop Detected"                  },

        { 510, "Not Extended"                   },
        { 511, "Network Authentication Required"}
};

HttpResponse::HttpResponse() : 
    status_(0),
    version_("HTTP/1.1"),
    phrase_(""),
    body_(""),
    bodyLength_(0),
    bodyOffset_(0)
{

}

HttpResponse::HttpResponse(int status, std::string version) :
    status_(status),
    version_(version),
    phrase_(""),
    body_(""),
    bodyLength_(0),
    bodyOffset_(0)
{
}

void HttpResponse::setBegin(int status, std::string version)
{
    const auto& it = statusMap_.find(status);    
    if(it != statusMap_.end())
    {
        phrase_ = it->second;
        setBegin(status, phrase_, version);
        return ;
    }
    setBegin(status, "UNKOWN STATUS", version);
    return ;
}

void HttpResponse::setBegin(int status, std::string phrase, std::string version)
{
    version_ = version;
    status_ = status;
    phrase_ = phrase;
}

void HttpResponse::setHeader(std::string name, std::string value)
{
    headers_.insert({name, value});
}

void HttpResponse::setBody(std::string body)
{
    body_ = body;
    setBodyLength(body.size());
}

void HttpResponse::setVersion(std::string version)
{
    version_ = version;
}

void HttpResponse::setStatus(int status)
{
    status_ = status;
}

void HttpResponse::setBodyLength(size_t length)
{
    bodyLength_ = length;
    char buf[32];
    util::convert(buf, bodyLength_);
    setHeader("Content-Length", buf);    
}

void HttpResponse::setContentType(std::string name)
{
    const auto& it = mimeMap_.find(name);
    if(it != mimeMap_.end())
    {
        setHeader("Content-Type", it->second);
    }
    setHeader("Content-Type", "UNKOWN TYPE");
}

void HttpResponse::setCookie(std::string name, std::string value, size_t maxAge, 
                std::string path, std::string domain, bool secure, bool strict, bool httpOnly)
{
    std::string cookie;
    char buf[32];
    util::convert(buf, maxAge);
    cookie = (name + ": " + value + "Max-Age=" + buf);
    if(!domain.empty())
    {
        cookie += ("; Domain=" + domain);
    }
    if(!path.empty())
    {
        cookie += ("; Path=" + path);
    }
    if(secure)
    {
        cookie += ("; Secure");
    }
    if(strict)
    {
        cookie += ("; SameSite=Strict");
    }
    if(httpOnly)
    {
        cookie += "; HttpOnly\r\n";
    }
    setHeader("Set-Cookie", cookie);
}

std::string HttpResponse::getStatusAsString(int status)
{
    const auto& it = statusMap_.find(status);
    if(it != statusMap_.end())
    {
        return it->second;
    }
    return std::string("UNKOWN STATUS");
}

HttpResponse& HttpResponse::makeOkResponse(std::string content)
{
    clear();
    setBegin(200);
    setBody(content);
    return *this;
}
HttpResponse& HttpResponse::makeErrorResponse(int status, std::string content, std::string contentType)
{

}

HttpResponse& HttpResponse::makeGetResponse(std::string content, std::string contentType)
{

}

void HttpResponse::clear()
{
    status_ = 0;
    phrase_ = "";
    body_ = "";
    bodyLength_ = 0;
    headers_.clear();
}

std::string HttpResponse::dumpString()
{
    std::string response;
    char buf[32];
    util::convert(buf, status_);
    response = (version_ + " " + buf + " " + getStatusAsString(status_) + "\r\n");
    for(const auto& it : headers_)
    {
        response += (it.first + ": " + it.second + "\r\n");
    }
    response += ("\r\n");
    if(!body_.empty())
    {
        response += (body_);
    }
    return response;
}

bool HttpResponse::parseResponseLine(const char* line, HttpResponse& response)
{
    int len = strlen(line);
    if(len < 2 || !(line[len - 2] == '\r' && line[len - 1] == '\n'))
    {
        return false;
    }
    len -=  2;

    int count = 0;
    util::split(line, line + len, ' ', 
    [&](const char* begin, const char* end)
    {
        switch(count)
        {
            case 0: 
                response.version_ = std::string(begin, end);
                break;
            case 1:
                {
                    std::string status(begin, end);
                    response.status_ = ::atoi(status.c_str());
                }
                break;
            case 2:
                response.phrase_ = getStatusAsString(response.status_);
                break;
            default:
                break;
        }
        count ++;
    });
    if(count != 3)
    {

        return false;
    }
    if(response.version_ != "HTTP/1.1" && response.version_ != "HTTP/1.0")
    {
        return false;
    }

    auto status = HttpResponse::statusMap_.find(response.status_);
    if(status == HttpResponse::statusMap_.end())
    {
        return false;
    }

    return true;
}

bool HttpResponse::parseHeader(const char* begin, const char* end, std::unordered_map<std::string, std::string>& headers)
{
    while(begin < end && util::isSpace(end[-1]))
    {
        end --;
    }
    const char* p = begin;
    while(p < end && *p != ':')
    {
        p++;
    }
    if(p == end)
    {
        return false;
    }
    
    const char* name_end = p;
    if(*p++ != ':')
    {
        return false;
    }
    while(p < end && util::isSpace(*p))
    {
        p++;
    }
    if(p < end)
    {
        std::string name(begin, name_end);
        std::string value(p, end);
        headers.emplace(name, value);
        return true;
    }
    return false;
}

bool HttpResponse::recvResponse(const char* data, size_t size)
{
    int index = 0;
    int response_line_end = 0;
    if(size < 2)
    {
        return false;
    }
    while(data[index] != '\r')
    {
        index++;
        response_line_end++;
        if(index >= size)
        {
            return false;
        }
    }
    index++;
    if(index >= size && data[++index] != '\n')
    {
        return false;
    }
    index++;
    response_line_end += 2;
    if(index >= size)
    {
        return false;
    }
    std::string rl(data, data + response_line_end);
    {
        bool f = parseResponseLine(rl.c_str(), *this);
        assert(f);
    }
    while(index + 3 < size && !(data[index] == '\r' && data[index + 1] == '\n' && data[index + 2] == '\r' && data[index + 3] == '\n'))
    {
        int header_begin = index;
        int header_end = index;
        while(!(data[index] == '\r' && data[index + 1 ] == '\n'))
        {
            index++;
            header_end++;
            if(index + 1 >= size)
            {
                return false;
            }
        }

        if(index + 3 < size && (data[index + 2] == '\r' && data[index + 3] == '\n'))
        {
            {
                bool f = parseHeader(data + header_begin, data + header_end, headers_);
                assert(f);
            }
            index += 3;
            break;
        }
        index += 2;
        if(index >= size)
        {
            return false;
        }
        {
            bool f = parseHeader(data + header_begin, data + header_end, headers_);
            assert(f);
        }
    }

    auto cl = headers_.find("Content-Length");
    if(cl != headers_.end())
    {
        index ++;
        bodyOffset_ = index;
        bodyLength_ = ::atoi(cl->second.c_str());
    }

}

}