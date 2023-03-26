//
// Created by lythe on 3/6/23.
//

#ifndef UNTITLED_LOG_H
#define UNTITLED_LOG_H

#include "logStream.h"
#include "fileUtils.h"

#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <memory>
#include <time.h>
#include <functional>
#include <iostream>
#include <map>



namespace lithe {
class LogLevel {
public:
    enum Level
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        UNKOWN
    };
    static const std::string toString(LogLevel::Level level)
    {
        switch(level){
#define XX(name) case name: return #name;
            XX(DEBUG)
            XX(INFO)
            XX(WARN)
            XX(ERROR)
            XX(FATAL)
#undef XX
            default :
                return "UNKOWN";
        }
    }
    static LogLevel::Level fromString(std::string str)
    {
#define XX(s, level) if(str == #s) { return level; }
            XX(debug, DEBUG)
            XX(info, INFO)
            XX(warn, WARN)
            XX(error, ERROR)
            XX(fatal, FATAL)

            XX(DEBUG, DEBUG)
            XX(INFO, INFO)
            XX(WARN, WARN)
            XX(ERROR, ERROR)
            XX(FATAL, FATAL)
#undef XX
            return UNKOWN;
    }

};
    //#LogLevel
//######################################################################

class LogEvent;
class LogAppender;
class LogFormatter;
class Logger : public std::enable_shared_from_this<Logger>
{
public:
    explicit Logger(std::string name = "root");
    void log(LogLevel::Level level, std::shared_ptr<LogEvent> event);
    void debug(std::shared_ptr<LogEvent> event);
    void info(std::shared_ptr<LogEvent> event);
    void warn(std::shared_ptr<LogEvent> event);
    void error(std::shared_ptr<LogEvent> event);
    void fatal(std::shared_ptr<LogEvent> event);

    void setLevel(LogLevel::Level level);
    LogLevel::Level getLevel() { return level_; }
    void addAppender(std::shared_ptr<LogAppender> appender);
    void delAppender(std::shared_ptr<LogAppender> appender);
    void setFormatter();
private:
    std::string name_;
    LogLevel::Level level_;
    std::list<std::shared_ptr<LogAppender>> appenders_;
    std::shared_ptr<LogFormatter> formatter_;
};

class LogEvent
{
public:
    LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, const char* filename, uint32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, time_t time, std::string threadName)
        : filename_(filename), line_(line), elapse_(elapse), threadId_(threadId), fiberId_(fiberId), time_(time), logger_(logger), level_(level), threadName_(threadName)
    {
    }
    const char* getFile() { return filename_; }
    const uint32_t getLine() { return line_; }
    const uint32_t getElapse() { return elapse_; }
    const uint32_t getThreadId() { return threadId_; }
    const uint32_t getFiberId() { return fiberId_; }
    const time_t getTime() { return time_; }
    std::shared_ptr<Logger> getLogger() { return logger_; }
    LogLevel::Level getLevel() { return level_; }
    std::string getThreadName() { return threadName_; }
    std::stringstream& getSS() { return ss_; }
    std::string getContent() { return ss_.str(); }
private:
    const char* filename_;
    uint32_t line_;
    uint32_t elapse_;
    uint32_t threadId_;
    uint32_t fiberId_;
    time_t time_;
    std::shared_ptr<Logger> logger_;
    LogLevel::Level level_;
    std::string threadName_;
    std::stringstream ss_;
};

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
    FileAppender(std::string, off_t, bool, int, int);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override;

    void rollFile();
    void append_unlocked(const char* logline, size_t len);
    void append(const char* logline, size_t len);
private:
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

class LogFormatter
{
public:
    explicit LogFormatter(std::string pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n") : pattern_(pattern)
    {
        init();
    }
    void init();
    std::stringstream& format(LogStream&, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);
    //void format(std::string filename, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);
    //std::ofstream& format(std::ofstream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);
    class FormatItem
    {
    public:
        virtual ~FormatItem(){}
        virtual void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) = 0;
    private:
    };
    ~LogFormatter(){  }
private:
    std::string pattern_;
    std::vector<std::shared_ptr<FormatItem> > items_;
    bool isError_ = false;
};



} // lithe



#endif //UNTITLED_LOG_H
