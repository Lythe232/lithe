#include "logger.h"
#include "logFormatter.h"
#include "logAppender.h"
#include "logEvent.h"
namespace lithe
{
Logger::Logger(std::string name) : name_(name)
{
    level_ = LogLevel::DEBUG;
    mutex_.reset(new Mutex());
    formatter_.reset(new LogFormatter());   //TODO
}
Logger::~Logger()
{    
}
void Logger::log(LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    if(level >= level_)
    {
        auto self = shared_from_this();
        
        MutexLockGuard lock(*mutex_);
        if(!appenders_.empty())
        {
            for(auto& i : appenders_)
            {
                i->log(self, level, event);
            }
        }
    }
    
}
void Logger::debug(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::DEBUG, event);
}
void Logger::info(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::INFO, event);
}
void Logger::warn(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::WARN, event);
}
void Logger::error(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::ERROR, event);
}
void Logger::fatal(std::shared_ptr<LogEvent> event)
{
    log(LogLevel::FATAL, event);
}
void Logger::setLevel(LogLevel::Level level)
{
    level_ = level;
}
// void Logger::setFormatter(std::shared_ptr<LogFormatter> formatter) {
//     if (!appenders_.empty())
//     {
//         for(auto& it : appenders_)
//         {
//             if(!it->hasFormatter())
//             {
//                 it->setFormatter(true);
//                 it->setFormatter(formatter);
//             }
//         }
//     }

// }
// void Logger::setFormatter(const std::string &val)
// {
//     std::shared_ptr<LogFormatter> formatter(new LogFormatter(val));
//     if(formatter->isError())
//     {
//         //TODO ERROR
//         return ;
//     }
//     setFormatter(formatter);
// }
void Logger::addAppender(std::shared_ptr<LogAppender> appender)
{
    MutexLockGuard lock(*mutex_);
    if(!appender->hasFormatter())
    {
        appender->setFormatter(true);
        appender->setFormatter(formatter_);
    }
    appenders_.push_back(appender);
}
void Logger::delAppender(std::shared_ptr<LogAppender> appender)
{
    if(!appenders_.empty())
    {
        MutexLockGuard lock(*mutex_);
        for(auto i = appenders_.begin(); i != appenders_.end(); i++)
        {
            if(*i == appender)
            {
                appenders_.erase(i);
            }
        }
    }
}


template<> LoggerManager* Singleton<LoggerManager, void>::singleton_ = 0;
LoggerManager::LoggerManager()
{
    root_.reset(new Logger);
    root_->addAppender(std::shared_ptr<LogAppender>(new FileAppender("../../log/log", 64 * 1024 * 1024, 3, 1024)));
    // root_->addAppender(std::shared_ptr<LogAppender>(new AsyncAppender("../../log/log", 64 * 1024 * 1024,  3, 1024)));
    loggers_[root_->getName()] = root_;
}
std::shared_ptr<Logger> LoggerManager::getLogger(std::string& name)
{
    auto it = loggers_.find(name);
    if(it != loggers_.end())
    {   
        return it->second;
    }

    std::shared_ptr<Logger> logger(new Logger(name));
    logger->setRoot(root_);
    loggers_[name] = logger;

    return logger;
}
std::shared_ptr<Logger> LoggerManager::getRoot()
{
    return root_;
}

    //#Logger
}   //lithe