#pragma once

#include "include/thread/mutex.h"
#include "include/thread/condition.h"

namespace lithe
{

class CountDownLatch
{
public:
    CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount();
private:
    Mutex mutex_;
    Condition cond_;
    int count_;
};

};  //namespace lithe