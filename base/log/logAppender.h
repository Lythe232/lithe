#pragma once

#include "logger.h"
#include "logStream.h"
#include "logFormatter.h"
#include "../fileUtils.h"
#include "../processInfo.h"

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
    FileAppender(std::string basename, off_t rollSize, bool threadSafe, int flushInterval, int checkEveryN);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override;

    bool rollFile();
    void append_unlocked(const char* logline, size_t len);
    void append(const char* logline, size_t len);
    void flush();
private:
    std::string getLogFileName(const std::string& basename, time_t* now);

    std::unique_ptr<FileUtils::AppendFile> file_;
    std::string basename_;
    const off_t rollSize_;
    const int flushInterval_;
    const int checkEveryN_;

    int count_;

    int mutex_;

    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;


    const static int kRollPerSeconds_ = 60 * 60 * 24;
};
class StdoutAppender : public LogAppender
{
public:
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override;
private:
};

}   //lithe