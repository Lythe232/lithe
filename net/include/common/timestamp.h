#pragma once

#include <stdint.h>
#include <time.h>
#include <string>


class Timestamp
{
public:
    Timestamp() : stamp_(0)
    {
    }
    Timestamp(uint64_t stamp) : stamp_(stamp)
    {
    }
    std::string toString();
    std::string toFormatString(bool showMicroseconds = true);
    static Timestamp now();
    time_t seconds();
    uint64_t microSeconds();
    uint64_t milliSecond();
    static const int MicroSecondPerSecond = 1000 * 1000;
private:
    int64_t stamp_;

};