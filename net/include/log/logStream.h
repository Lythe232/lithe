#pragma once

#include "../common/fixedBuffer.h"

#include <string.h>
#include <string>
namespace lithe
{


class LogStream
{
public:
    typedef LogStream self;
    typedef FixedBuffer<SmallFile> Buffer;
    LogStream() 
    {
    }
    ~LogStream()
    {
    }
    self& operator<<(bool flag) 
    { 
        buffer_.append((flag ? "1" : "0"), 1); 
        return *this;
    }
    self& operator<<(unsigned short val);
    self& operator<<(short val);
    self& operator<<(unsigned int val);
    self& operator<<(int val);
    self& operator<<(float val);
    self& operator<<(double val);
    self& operator<<(unsigned long val);
    self& operator<<(long val);
    self& operator<<(unsigned long long val);
    self& operator<<(long long val);

    self& operator<<(const void*);
    self& operator<<(char c)
    {
        buffer_.append(&c, 1);
        return *this;
    }
    self& operator<<(const char* str)
    {
        if(str)
        {
            buffer_.append(str, strlen(str));
        }
        else
        {
            buffer_.append("(null)", 6);
        }
        return *this;
    }

    self& operator<<(std::string str)
    {
        buffer_.append(str.c_str(), str.size());
        return *this;
    }
    self& operator<<(const Buffer& );
    Buffer& buffer() { return buffer_; }
    std::string toString() { return buffer_.toString(); }
    
    void resetBuffer() { buffer_.reset(); }
    uint32_t readableSize() { return buffer_.getReadableSize(); }
private:
    
    template <typename T>
    void formatInteger(T t);

    Buffer buffer_;

    static const int kMaxNumericSize = 48;
};



}
