#include "include/common/timestamp.h"

#include <sys/time.h>

std::string Timestamp::toString()
{
    return std::string();
}

std::string Timestamp::toFormatString(bool showMicroseconds)
{
    char buf[64];
    time_t seconds = static_cast<time_t>(stamp_ / MicroSecondPerSecond);
    struct tm tm;
    gmtime_r(&seconds, &tm);
    if (showMicroseconds)
    {
        int microseconds = static_cast<int>(stamp_ % MicroSecondPerSecond);
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec,
        microseconds);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec);
    }
    return buf;
}

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

uint64_t Timestamp::microSeconds()
{
    return stamp_;
}

uint64_t Timestamp::milliSecond()
{
    return stamp_ / 1000;
}
