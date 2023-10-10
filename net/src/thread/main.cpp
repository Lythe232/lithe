
#include "include/thread/thread.h"
#include "thread/thread/mutex.h"

#include <iostream>
#include <memory>
using namespace lithe;

int g_val = 0;

Mutex* g_mutex = new Mutex();
void func1()
{
    MutexLockGuard mutex(*g_mutex);
    for(int i = 0; i < 100000; i++)
    {
        g_val += 1;
    }
}
void func2()
{
    MutexLockGuard mutex(*g_mutex);

    for(int i = 0; i < 100000; i++)
    {
        g_val += 1;
    }
}
int main()
{
    Thread thread1(std::bind(func1));
    Thread thread2(std::bind(func2));
    thread1.start();
    thread2.start();

    thread1.join();
    thread2.join();
    std::cout << "g_val = " << g_val << std::endl;
    return 0;
}