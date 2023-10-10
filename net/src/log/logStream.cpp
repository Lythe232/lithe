#include "include/log/logStream.h"

#include <algorithm>
#include <assert.h>

namespace lithe
{
namespace details
{
    const char digits[] = "9876543210123456789";
    const char* zero = digits + 9;
    template <typename T>
    ssize_t convert(char buf[], T value)
    {
        T i = value;
        char* p = buf;
        assert(zero);
        do
        {
            int lsd = i % 10;
            i /= 10;
            *p++ = zero[lsd];
        } while (i != 0);
        if(value < 0)
        {
            *p++ = '-';
        }
        *p = '\0';

        std::reverse(buf, p);
        return p - buf;
    }
}   //namespace details

template <int SIZE>
void FixedBuffer<SIZE>::cookieStart()
{
}

template <int SIZE>
void FixedBuffer<SIZE>::cookieEnd()
{
}

template <typename T>
void LogStream::formatInteger(T t)
{
    if(buffer_.avail() > kMaxNumericSize)
    {
        size_t len = details::convert(buffer_.current(), t);
        buffer_.add(len);
    }
}

LogStream &LogStream::operator<<(unsigned short val)
{
    *this << static_cast<unsigned int> (val);
    return *this;
}

LogStream &LogStream::operator<<(short val)
{
    *this << static_cast<int>(val);
    return *this;
}

LogStream &LogStream::operator<<(unsigned int val)
{
    formatInteger(val);
    return *this;
}

LogStream &LogStream::operator<<(int val)
{
    formatInteger(val);
    return *this;
}

LogStream &LogStream::operator<<(float val)
{
    *this << static_cast<double>(val);
    return *this;
}

LogStream &LogStream::operator<<(double val)
{
    if(buffer_.avail() > kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", val);
        buffer_.add(len);
    }
    return *this;
}

LogStream &LogStream::operator<<(unsigned long val)
{
    formatInteger(val);
    return *this;
}

LogStream &LogStream::operator<<(long val)
{
    formatInteger(val);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long val)
{
    formatInteger(val);
    return *this;
}

LogStream &LogStream::operator<<(long long val)
{
    formatInteger(val);
    return *this;
}

LogStream &LogStream::operator<<(const void *)
{
    //TODO
    return * this;
}

LogStream &LogStream::operator<<(const Buffer & buf)
{
    //TODO
    return *this;
}

} // namespace lithe