#pragma once

//#include <sstream>
#include "logStream.h"
#include "include/common/timestamp.h"
namespace lithe
{

class LogEvent
{
public:
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* filename, uint32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, std::string threadName)
        : filename_(filename), line_(line), elapse_(elapse), threadId_(threadId), fiberId_(fiberId), logger_(logger), level_(level), threadName_(threadName)
    {
    }
    const char* getFile() { return filename_; }
    const uint32_t getLine() { return line_; }
    const uint32_t getElapse() { return elapse_; }
    const uint32_t getThreadId() { return threadId_; }
    const uint32_t getFiberId() { return fiberId_; }
    const time_t getTime() { return stamp_.now().seconds(); }
    std::shared_ptr<Logger> getLogger() 
    { 
        assert(logger_);
        return logger_; 
    }
    LogLevel::Level getLevel() { return level_; }
    std::string getThreadName() { return threadName_; }
    // const char* getThreadName() { return threadName_; }
    //std::stringstream& getSS() { return ss_; }
    LogStream& getStream(){ return stream_; }
    //std::string getContent() { return ss_.str(); }
    std::string getContent() { return stream_.toString();}
private:
    const char* filename_ = nullptr;
    uint32_t line_;
    uint32_t elapse_;
    uint32_t threadId_;
    uint32_t fiberId_;
    Timestamp stamp_;
    std::shared_ptr<Logger> logger_;
    LogLevel::Level level_;
    std::string threadName_;
    // char threadName_[32];
    //std::stringstream ss_;
    LogStream stream_;
};

class LogEventWrap
{
public:
    LogEventWrap() : 
    event_()
    {
    }
    LogEventWrap(std::shared_ptr<LogEvent> event)
    : event_(event)
    {
    }
    ~LogEventWrap()
    {
        event_->getLogger()->log(event_->getLevel(), event_);        
    }
    LogStream& getStream() { return event_->getStream(); }
private:
    std::shared_ptr<LogEvent> event_;
};
}   //lithe