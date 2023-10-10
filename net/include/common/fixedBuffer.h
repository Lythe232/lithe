#pragma once

#include <string.h>
#include <string>
#include <assert.h>
namespace lithe
{
const int SmallFile = 4000;
const int LargeFile = 4000 * 1000;

/// thread unsafe
/// @brief 
/// @tparam SIZE 
template<int SIZE>
class FixedBuffer
{
public:
    FixedBuffer() : 
        cur_(data_),
        r_pos_(data_)
    {
    }
    ~FixedBuffer()
    {
    }
    const char* data() { return data_; }
    void add(size_t len) { cur_ += len; }
    bool append(const char* str, size_t len)
    {
        if(avail() < len)
        {
            return false;
        }
        memcpy(cur_, str, len);
        cur_ += len;
        return true;
    }
    size_t fetch(char* buf, size_t len)
    {
        size_t ar = 0;
        memset(buf, 0, len);
        if(getReadableSize() >= len)
        {
            memcpy(buf, r_pos_, len);
            ar = len;
        }
        else
        {
            memcpy(buf, r_pos_, getReadableSize());
            ar = getReadableSize();
        }
        r_pos_ += ar;
        // buf[ar] = '\0';
        assert(!!(cur_ >= r_pos_));
        return ar;
    }
    size_t avail() const { return static_cast<size_t>(end() - cur_); }
    void reset() { cur_ = data_; }
    char* current() const { return cur_; }
    size_t length() const { return static_cast<size_t>(cur_ - data_);}
    size_t getReadableSize(){ return static_cast<size_t>(cur_ - r_pos_); }
    void bzero()
    {
        memset(data_, 0, sizeof(data_));
    }
    std::string toString() const { return std::string(data_, length()); }
private:
    const char* end() const { return data_ + sizeof(data_); }

    static void cookieStart();
    static void cookieEnd();

    void (*cookie_)();
    char data_[SIZE];
    char* cur_;
    char* r_pos_;
};

}
