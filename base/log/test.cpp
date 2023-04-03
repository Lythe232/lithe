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
    for(int i = 0; i < 250; i++)
    {
        LOG_DEBUG(logger) << "--------------------------";
    }
}
void func2()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 250; i++)
    {
        LOG_WARN(logger)  << "//////////////////////////";
    }
}
void func3()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 250; i++)
    {
        LOG_ERROR(logger) << "++++++++++++++++++++++++++";
    }
}
void func4()
{
    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();
    for(int i = 0; i < 250; i++)
    {
        LOG_FATAL(logger) << "~~~~~~~~~~~~~~~~~~~~~~~~~~";
    }
}
int main()
{

    auto logger = g_loggerMgr.getSingletonPtr()->getRoot();

    Thread thread1(std::bind(func1));
    Thread thread2(std::bind(func2));
    Thread thread3(std::bind(func3));
    Thread thread4(std::bind(func4));

    Timestamp time_;
    Timestamp s = time_.now();

    thread1.start();
    thread2.start();
    thread3.start();
    thread4.start();



    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();

    // for(int i = 0; i < 10; i ++)
    // {
    //     LOG_DEBUG(logger) << "Hello" << 1 << 2.2 << 3.333333333333 << false;
    // }
    Timestamp e = time_.now();
    printf("time: %.4lfs\n", (e.microSeconds() - s.microSeconds()) / 1000000.0);

}