//
// Created by lythe on 3/6/23.
//
#include "logger.h"
#include "logEvent.h"
#include "logAppender.h"


#include <string>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <sys/time.h>

using namespace lithe;

int main()
{
    // auto logger = LOG_ROOT();
    //std::string  fmt = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n";
    //LogFormatter formatter(fmt);
    //std::shared_ptr<Logger> logger, LogLevel::Level level, const char* filename, uint32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, time_t time, std::string threadName
    std::shared_ptr<Logger> logger(new Logger("root"));
    // std::shared_ptr<LogEvent> event(new LogEvent(logger, logger->getLevel(), __FILE__, __LINE__, 312, 1, 2, time(NULL), "ThreadName"));
    logger->addAppender(std::shared_ptr<LogAppender>(new FileAppender("/home/lythe/Codes/lithe/log", 40960, false, 3, 1024)));
    struct timespec start;
    struct timespec end;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
    for(int i = 0; i < 10; i ++)
    {
        LOG_ERROR(logger) << "LOG_ERROR" << 1 << 2.222 << false;
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
    double start_ms =  start.tv_sec * 1000 + start.tv_nsec / 1000000;
    double end_ms =  end.tv_sec * 1000 + end.tv_nsec / 1000000;
    printf("time: %.4lfms\n", end_ms - start_ms);

}