#include "include/util/string_util.h"


namespace lithe
{

namespace util
{
void split(const char* start, const char* end, char chr, 
            std::function<void(const char*, const char*)> fn)
{
    if(end < start)
    {
        return ;
    }   
    size_t i = 0;
    size_t begin = 0;
    while((start + i < end) && start[i] != '\0')
    {
        if(start[i] == chr)
        {
            auto pair = trim(start, end, begin, i);
            if(pair.first < pair.second)
            {
                fn(&start[pair.first], &start[pair.second]);
            }
            begin = i + 1;
        }
        i++;
    }
    if(i)
    {
        auto pair = trim(start, end, begin, i);
        if(pair.first < pair.second)
        {
            fn(&start[pair.first], &start[pair.second]);
        }
    }
}

std::pair<size_t, size_t> trim(const char* start, const char* end, size_t left, size_t right)
{
    while(start + left < end && isSpace(start[left]))
    {
        left++;
    }
    while(right > 0 && isSpace(end[right - 1]))
    {
        right--;
    }
    return std::make_pair(left, right);
}

}   //namespace util

}   //namespace lithe