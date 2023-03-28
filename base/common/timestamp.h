#pragma once

#include <stdint.h>
#include <time.h>

class Timestamp
{
public:
    Timestamp() : stamp_(0)
    {
    }
    Timestamp(uint64_t stamp) : stamp_(stamp)
    {
    }
    Timestamp now();
    time_t seconds();
    int64_t microSeconds();
    static const int MicroSecondPerSecond = 1000 * 1000;
private:
    int64_t stamp_;

};