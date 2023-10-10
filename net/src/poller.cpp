#include "include/poller.h"
#include "include/coScheduler.h"

namespace lithe
{
Poller::Poller()
{
}

Poller::~Poller()
{
}
int Poller::fd()
{
    return -1;
}
}