//
// Created by lythe on 3/6/23.
//
#include "logger.h"
#include "logEvent.h"
#include "logAppender.h"
#include "../thread/thread.h"
#include "common/timestamp.h"


#include <string>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <sys/time.h>
#include <functional>
using namespace lithe;

LoggerManager g_loggerMgr;

void func1()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 20; i++)
    {
        LOG_DEBUG(logger) << "--------------------------";
    }
}
void func2()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 20; i++)
    {
        LOG_DEBUG(logger) << "**************************";
    }
}
void func3()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 20; i++)
    {
        LOG_DEBUG(logger) << "++++++++++++++++++++++++++";
    }
}
void func4()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 20; i++)
    {
        LOG_DEBUG(logger) << "--------------------------";
    }
}
void func5()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 20; i++)
    {
        LOG_DEBUG(logger) << "~~~~~~~~~~~~~~~~~~~~~~~~~~";
    }
}
int main()
{

    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();

    //std::string  fmt = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n";
    Thread thread1(std::bind(func1));
    Thread thread2(std::bind(func2));
    Thread thread3(std::bind(func3));
    Thread thread4(std::bind(func4));
    Thread thread5(std::bind(func5));
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    // thread1.start();
    // thread2.start();
    // thread3.start();
    // thread4.start();
    // thread5.start();


    // thread1.join();
    // thread2.join();
    // thread3.join();
    // thread4.join();
    // thread5.join();
    for(int i = 0; i < 1000000; i ++)
    {
        LOG_ERROR(logger) << "Hello, World" << ": " << 1;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    double start_ms =  start.tv_sec * 1000 + start.tv_nsec / 1000000;
    double end_ms =  end.tv_sec * 1000 + end.tv_nsec / 1000000;
    printf("time: %.4lfms\n", end_ms - start_ms);
}