#include "coroutine.h"
#include "coScheduler.h"
#include "../common/blockingQueue.h"
// #include "../common/MemoryUsage.h"
#include "../thread/currentThread.h"
#include "log/log.h"
#include "hookSysCall.h"

#include <iostream>
#include <unistd.h>
#include <vector>
#include <atomic>


using namespace lithe;
namespace 
{
LoggerManager* g_loggerMgr = new LoggerManager(); 
}
void func()
{
    printf("1--\n");
    printf("2--\n");
    printf("3--\n");
    printf("4--\n");
    printf("5--\n");
    printf("6--\n");
    printf("7--\n");
    printf("8--\n");
    printf("9--\n");
    printf("+_+_++_+_++_+\n");
    CoScheduler::yieldToReady();

}
void func1()
{
    printf("1==\n");
    printf("2==\n");
    printf("3==\n");
    printf("4==\n");
    printf("5==\n");
    printf("6==\n");
    printf("7==\n");
    printf("8==\n");
    printf("9==\n");

}
void func2()
{
    printf("1~~\n");
    printf("2~~\n");
    printf("3~~\n");
    printf("4~~\n");
    printf("5~~\n");
    printf("6~~\n");
    printf("7~~\n");
    printf("8~~\n");
    printf("9~~\n");
}
static auto g_logger = LOG_ROOT();

int main()
{
    // Coroutine co(func);
    // Coroutine co1(func1);
    // Coroutine co2(func2);

    // co.resume();
    // co1.resume();
    // co2.resume();

    CoScheduler* scheduler = new CoScheduler(4, true, "Scheduler");
    scheduler->start();

    std::shared_ptr<Coroutine> co1(new Coroutine(func));
    std::shared_ptr<Coroutine> co2(new Coroutine(func2));
    
    std::shared_ptr<Coroutine> co3(new Coroutine(func1));

    // co1->resume();
    // co2->resume();
    // co3->resume();


    // scheduler->schedule(func);
    // scheduler->schedule(func1);
    // scheduler->schedule(func2);
    scheduler->schedule(co1);
    scheduler->schedule(co2);
    std::cout << is_hook_enable() << std::endl;
    
    // scheduler->stop();
    // StackAllocator::print();
    // printMemoryUsage();
}

