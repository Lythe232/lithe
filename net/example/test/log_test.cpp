//
// Created by lythe on 3/6/23.
//
#include "logger.h"
#include "logEvent.h"
#include "logAppender.h"
#include "../thread/thread.h"
#include "common/timestamp.h"
#include "../thread/currentThread.h"

#include <string>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <sys/time.h>
#include <functional>
using namespace lithe;

static auto g_logger = LOG_ROOT();
void func1()
{
    for(int i = 0; i < 1; i++)
    {
        printf("------i = %ld\n", i);
        LOG_DEBUG(g_logger) << "--------------------------" << CurrentThread::cacheTid() << " get after " << CurrentThread::cacheTid();
    }
}
void func2()
{
    for(int i = 0; i < 25; i++)
    {
        printf("///////i = %ld\n", i);
        LOG_WARN(g_logger)  << "//////////////////////////"  << CurrentThread::cacheTid() << " get after " << CurrentThread::cacheTid();
    }
}
void func3()
{
    for(int i = 0; i < 25; i++)
    {
        printf("++++++i = %ld\n", i);
        LOG_ERROR(g_logger) << "++++++++++++++++++++++++++" << CurrentThread::cacheTid() << " get after " << CurrentThread::cacheTid();
    }
}
void func4()
{
    for(int i = 0; i < 25; i++)
    {
        printf("~~~~~~i = %ld\n", i);
        LOG_FATAL(g_logger) << "~~~~~~~~~~~~~~~~~~~~~~~~~~" << CurrentThread::cacheTid() << " get after " << CurrentThread::cacheTid();
    }
}
int main()
{

    Timestamp time_;

    Thread thread1(std::bind(func1));
    Thread thread2(std::bind(func2));
    Thread thread3(std::bind(func3));
    Thread thread4(std::bind(func4));
    Timestamp s = time_.now();
    thread1.start();
    // thread2.start();
    // thread3.start();
    // thread4.start();

    printf("_________________\n");

    thread1.join();
    // thread2.join();
    // thread3.join();
    // thread4.join();
    Timestamp e = time_.now();
    printf("time: %.4lfs\n", (e.microSeconds() - s.microSeconds()) / 1000000.0);


                // int64_t allTime = 0; 
                // for(int i = 0; i < 10; i ++)
                // {
                //     Timestamp s = time_.now();

                //     for(int i = 0; i < 1000; i ++)
                //     {
                //         LOG_DEBUG(g_logger) << "Hello" << 1 << " " << 2.2 << " " << 3.3333333 << " " << false;
                //     }

                //     Timestamp e = time_.now();
                //     printf("time: %.4lfs\n", (e.microSeconds() - s.microSeconds()) / 1000000.0);
                //     allTime += e.microSeconds() - s.microSeconds();

                // }
                // printf("all time = %.4lfs, average time = %.4lfs\n", allTime / 1000000.0, allTime / 10000000.0);
    

}