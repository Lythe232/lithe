#include "timestamp.h"

#include <sys/time.h>

Timestamp Timestamp::now()
{   
    struct timeval tv;

    gettimeofday(&tv, nullptr);
    uint64_t seconds = tv.tv_sec;

    return Timestamp(seconds * MicroSecondPerSecond + tv.tv_usec);
}
time_t Timestamp::seconds()
{
    return static_cast<time_t>(stamp_ / MicroSecondPerSecond);
}

int64_t Timestamp::microSeconds()
{
    return stamp_;
}
