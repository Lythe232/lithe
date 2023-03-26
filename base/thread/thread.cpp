#include "thread.h"
#include "../logger.h"
namespace lithe
{

Thread::Thread(std::function<void ()> args, std::string name)
                :
                name_(name),
                started_(false),
                joined_(false)
{
    datas_.func_ = args;
}

Thread::~Thread()
{
    pthread_detach(pthreadId_);
}

void Thread::start()
{
    if(pthread_create(&pthreadId_, nullptr, &details::threadFunc, &datas_))
    {
        
    }
}
int Thread::join()
{
    return pthread_join(pthreadId_, nullptr);
}


}