#pragma once

#include <pthread.h>
#include <string>
#include <functional>
namespace lithe
{
namespace details
{
struct ThreadData
{
    std::function<void ()> func_;
};
void* threadFunc(void* args)
{
    ThreadData* data = static_cast<ThreadData*>(args);
    data->func_();
    delete [] data;
}
}   // namespace details
    

class Thread
{
public:
    Thread(std::function<void ()>, std::string name = std::string());
    ~Thread();
    void start();

    int join();

private:
    pthread_t pthreadId_;
    pid_t tid_;
    std::string name_;
    bool started_;
    bool joined_;
    details::ThreadData datas_;
};


}   //namespace lithe