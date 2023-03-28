#pragma once

#include "common/singleton.h"
#include "thread/mutex.h"

#include <string>
#include <memory>
#include <list>
#include <map>

#define LOG_LEVEL(logger, level) LogEventWrap(std::shared_ptr<LogEvent>(new LogEvent(logger, level, __FILE__, __LINE__, 0, 1, 2, 0, "threadName"))).getStream()
#define LOG_DEBUG(logger) LOG_LEVEL(logger, LogLevel::DEBUG)
#define LOG_INFO(logger) LOG_LEVEL(logger, LogLevel::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, LogLevel::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger, LogLevel::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, LogLevel::FATAL)

namespace lithe
{


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

class LogEvent;
class LogAppender;
class LogFormatter;
class Logger : public std::enable_shared_from_this<Logger>
{
public:
    explicit Logger(std::string name = "root");
    ~Logger();

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
    void setFormatter(std::shared_ptr<LogFormatter> formatter);
    void setFormatter(const std::string& val);
    void setRoot(std::shared_ptr<Logger> root){ root_ = root; };
    std::string getName() { return name_; }
private:
    std::string name_;
    LogLevel::Level level_;
    std::list<std::shared_ptr<LogAppender>> appenders_;
    std::shared_ptr<LogFormatter> formatter_;
    std::shared_ptr<Logger> root_;

    std::unique_ptr<Mutex> mutex_;
};



class LoggerManager : public Singleton<LoggerManager, void>
{
public:
    LoggerManager();

    std::shared_ptr<Logger> getRoot();
    std::shared_ptr<Logger> getLogger(std::string& name);

private:
    std::map<std::string, std::shared_ptr<Logger>> loggers_;
    std::shared_ptr<Logger> root_;
};


}   // namespace lithe
