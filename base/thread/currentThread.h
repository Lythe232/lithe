#pragma once


#include "thread.h"


namespace CurrentThread
{
extern __thread int cacheTid_;
extern __thread char tidString[32];
void cacheTid();

int tid();


}   //namespace CurrentThread