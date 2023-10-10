#include "include/thread/currentThread.h"

namespace CurrentThread
{
pid_t gettid()
{
    return syscall(SYS_gettid);
}

}   //namespace CurrentThread