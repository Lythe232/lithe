#pragma once

#include "include/common/noncopyable.h"

#include <pthread.h>
#include <string>
#include <functional>
#include <atomic>
namespace lithe
{
namespace details
{

};  // namespace details

class Thread : public Noncopyable
{
public:
    typedef std::function<void()> ThreadFunc;

    Thread(ThreadFunc func, std::string name = std::string());
    ~Thread();
    void start();
    int join();
    int gettid();

private:
    static void* run(void* args);

    void setDefaultName();
    pthread_t pthreadId_;
    pid_t tid_;
    std::string name_;
    bool started_;
    bool joined_;
    ThreadFunc func_;
    std::atomic<int> numCreated_ = {0};
};


}   //namespace lithe