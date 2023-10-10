#include "include/log/AsyncLogging.h"
#include "include/common/fileUtils.h"
#include "include/common/timestamp.h"

#include <unistd.h>
#include <assert.h>

namespace lithe
{

AsyncLogging::AsyncLogging(std::string basename, off_t rollSize, int flushInterval) :
                    basename_(basename),
                    rollSize_(rollSize),
                    flushInterval_(flushInterval),
                    running_(false),
                    thread_(std::bind(&AsyncLogging::threadFunc_, this), "Logging"),
                    latch_(1),
                    mutex_(),
                    cond_(mutex_),
                    currentBuffer_(new Buffer()),
                    nextBuffer_(new Buffer()),
                    buffers_(),
                    queue_()
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}


void AsyncLogging::append(const char* logline, size_t len)
{
    MutexLockGuard lock(mutex_);

    if(currentBuffer_->avail() > len)
    {
        currentBuffer_->append(logline, len);
    }
    else
    {
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_)
        {
            currentBuffer_ = std::move(nextBuffer_);
        }
        else
        {
            currentBuffer_.reset(new Buffer());
        }
        currentBuffer_->append(logline, len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc_()
{

    assert(running_ == true);
    
    LogFile* file = new LogFile(basename_, rollSize_, true, flushInterval_, 1024);


    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    
    newBuffer1->bzero();
    newBuffer2->bzero();

    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);


    while(running_)
    {
        if(latch_.getCount())
        {
            latch_.countDown();
        }
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {
            MutexLockGuard lock(mutex_);
            if(buffers_.empty())
            {
                cond_.waitForSeconds(flushInterval_);
            }

            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if(!nextBuffer_)
            {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if(buffersToWrite.size() > 25)
        {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n", 
                                Timestamp::now().toFormatString().c_str(), 
                                buffersToWrite.size()-2);
            fprintf(stderr, "%s\n", buf);
            file->append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }
        for(const auto& buffer : buffersToWrite)
        {
            file->append(buffer->data(), buffer->length());
        }
        if(buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }
        if(!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();    
            newBuffer1->bzero();
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();    
            newBuffer2->bzero();
        }
        buffersToWrite.clear();
        file->flush();
    }
    MutexLockGuard lock(mutex_);
    if(currentBuffer_->length() != 0)
    {
        file->append(currentBuffer_->data(), currentBuffer_->length());
    }
    file->flush();
    delete file;    //..
}



};  //namespace lithe