#pragma once


#include "thread.h"

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
extern thread_local int t_cacheTid_;
extern thread_local char tidString[32];
pid_t cacheTid();

char* getThreadName();

pid_t gettid();

int tid();


}   //namespace CurrentThread