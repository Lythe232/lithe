#pragma once

#include "logger.h"
#include "logStream.h"
#include "logFormatter.h"
#include "logFile.h"
#include "include/common/fileUtils.h"
#include "include/common/processInfo.h"
#include "include/thread/mutex.h"
#include "include/thread/thread.h"
#include "include/thread/condition.h"
#include "AsyncLogging.h"

#include <memory>
namespace lithe
{

class LogAppender
{
public:
    virtual ~LogAppender() 
    {
    }
    virtual void log(std::shared_ptr<Logger> , LogLevel::Level level, std::shared_ptr<LogEvent>) = 0;
    virtual void setFormatter(std::shared_ptr<LogFormatter> formatter) { formatter_ = formatter; }
    bool hasFormatter(){ return hasFormatter_; }
    void setFormatter(bool flag){ hasFormatter_ = flag; }
protected:
    std::shared_ptr<LogFormatter> formatter_;
    LogStream stream_;
    bool hasFormatter_ = false;
};

class FileAppender : public LogAppender
{
public:
    ~FileAppender();
    FileAppender(std::string basename, off_t rollSize,  int flushInterval, int checkEveryN);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override;

private:
    std::unique_ptr<Mutex> mutex_;
    std::unique_ptr<LogFile> file_;
};
class AsyncAppender : public LogAppender
{
public:
    AsyncAppender(std::string basename, off_t rollSize, int flushInterval, int checkEveryN);
    ~AsyncAppender();
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent>) override;

private:
    std::unique_ptr<AsyncLogging> async_;
    std::unique_ptr<Mutex> mutex_;

};

class StdoutAppender : public LogAppender
{
public:
    StdoutAppender();
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override;
    
private:
    std::unique_ptr<Mutex> mutex_;
};

}   //lithe