//
// Created by lythe on 3/6/23.
//
#include "log.h"
#include <string>
#include <iostream>
#include <memory>
using namespace lithe;
#define LOG
int main()
{
    //std::string  fmt = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n";
    //LogFormatter formatter(fmt);
    //std::shared_ptr<Logger> logger, LogLevel::Level level, const char* filename, uint32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, time_t time, std::string threadName
    std::shared_ptr<Logger> logger(new Logger("root"));
    std::shared_ptr<LogEvent> event(new LogEvent(logger, logger->getLevel(), "test", __LINE__, 312, 1, 2, 3, "ThreadID"));
    event->getSS() << "HELLO WORLD" << std::endl;
    std::shared_ptr<LogAppender> appender(new StdoutAppender());
    //std::shared_ptr<LogAppender> appender(new FileAppender("./log.txt"));
    logger->addAppender(appender);
    logger->setFormatter();
    std::cout << "info:";
    logger->info(event);
    std::cout << "\n fatal" << std::endl;
    logger->fatal(event);
    getchar();
}