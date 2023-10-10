#pragma once
#include <algorithm>
#include <functional>
namespace lithe
{
namespace util
{

static const char* digits = "9876543210123456789";
static const char* zero = digits + 9;

template<typename T>
inline size_t convert(char buf[], T value)
{

    T v = value;
    char* p = buf;
    do
    {
        *p++ = zero[v % 10];
        v /= 10;
    }while(v != 0);
    if(value < 0)
    {
        *p++ = '-';
    }    
    *p = '\0';
    std::reverse(buf, p);
    return p - buf;
}

inline bool isSpace(char chr) { return chr == ' ' || chr == '\t'; };

void split(const char* start, const char* end, char chr, std::function<void(const char* , const char*)>);

std::pair<size_t, size_t> trim(const char* start, const char* end, size_t left, size_t right);

}   //namespace util

}   //namespace lithe