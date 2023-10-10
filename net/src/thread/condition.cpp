#include "include/thread/condition.h"

#include <stdint.h>
bool lithe::Condition::waitForSeconds(double seconds)
{
    struct timespec abstime;

    clock_gettime(CLOCK_REALTIME, &abstime);

    const int64_t nanoSecondsPerSecond = 1000000000;

    const int64_t nanoSeconds = static_cast<int64_t>(seconds * nanoSecondsPerSecond);

    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoSeconds) / nanoSecondsPerSecond);
    abstime.tv_nsec = static_cast<long> ((abstime.tv_sec  + nanoSeconds) % nanoSecondsPerSecond);
    


    return pthread_cond_timedwait(&cond_, mutex_.getPthreadMutex(), &abstime);
}