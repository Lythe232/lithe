#pragma once

#include "logStream.h"
#include "logFile.h"
#include "include/thread/mutex.h"
#include "include/thread/thread.h"
#include "include/thread/condition.h"
#include "include/common/countDownLatch.h"
#include "include/common/blockingQueue.h"

#include <vector>
#include <memory>
#include <atomic>
namespace lithe
{

class AsyncLogging
{
public:
    AsyncLogging(std::string basename, off_t rollSize, int flushInterval = 3);
    ~AsyncLogging()
    {
        if(running_)
        {
            stop();
        }
    }

    void append(const char* logline, size_t size);
    void append(const std::string logline);
    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop()
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:
    void threadFunc_();

    typedef lithe::FixedBuffer<lithe::LargeFile> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    std::string basename_;
    const off_t rollSize_;
    const int flushInterval_;
    std::atomic<bool> running_;
    
    lithe::Thread thread_;
    lithe::CountDownLatch latch_;
    lithe::Mutex mutex_;
    lithe::Condition cond_;

    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;

    BlockingQueue<std::string> queue_;

};

class RingBuffer
{

};

}