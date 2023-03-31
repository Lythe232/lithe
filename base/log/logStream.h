#pragma once

#include <string.h>
#include <string>
namespace lithe
{
const int SmallFile = 4000;
const int LargeFile = 4000 * 1000;
template <int SIZE>
class FixedBuffer
{
public:
    FixedBuffer() : cur_(data_)
    {
        //setCookie(cookieStart);
    }
    ~FixedBuffer()
    {
        //setCookie(cookieEnd);
    }   
    const char* data() { return data_; }
    size_t avail() { return end() - cur_; }
    void add(size_t len) { cur_ += len; }
    void append(const char* buf, size_t len)
    {
        if(static_cast<size_t>(avail() > len))
        {
            memcpy(cur_, buf, len);
            cur_ += len;
        }
    }

    int length() const { return static_cast<int>(cur_ - data_); }

    void setCookie(void (*cookie)()) { cookie_ = cookie; }

    void reset() 
    { 
        cur_ = data_; 
    }
    
    char* current() { return cur_; }

    std::string toString() { return std::string(data_, length());}

    void bzero()
    {
        memset(data_, 0, sizeof data_);
    }
private:
    char* end() { return data_ + sizeof(data_); }

    static void cookieStart();
    static void cookieEnd();


    void (*cookie_)();
    char data_[SIZE];
    char* cur_;
};

class LogStream
{
public:
    typedef LogStream self;
    typedef FixedBuffer<SmallFile> Buffer;
    LogStream() {}
    self& operator<<(bool flag) { 
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
private:
    
    template <typename T>
    void formatInteger(T t);

    Buffer buffer_;

    static const int kMaxNumericSize = 48;
};



}