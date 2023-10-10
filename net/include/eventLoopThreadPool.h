#pragma once

#include <functional>
#include <vector>

namespace lithe
{
class EventLoop;
class EventLoopThread;
class EventLoopThreadPool
{
public:
    EventLoopThreadPool(int threadNum);
    ~EventLoopThreadPool();

    void start(std::function<void(EventLoop*)> threadIninCallback = nullptr);
    EventLoop* getNextLoop();
    EventLoop* getMainLoop();
private:

    EventLoop* baseLoop_;   
    std::vector<EventLoopThread*> pool_;
    bool useCaller_;
    bool started_;
    int numThreads_;
    int nextLoop_;
};


}