#pragma once

#include "logStream.h"
#include "logger.h"
#include "include/common/timestamp.h"
#include "include/thread/mutex.h"


#include <vector>


namespace lithe
{

class LogFormatter
{
public:
    explicit LogFormatter() //std::string pattern = "") : pattern_(pattern)
    {
        mutex_.reset(new Mutex());
        init();
    }
    void init();
    void format(LogStream&, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event);
    ~LogFormatter(){  }
    
    // class FormatItem
    // {
    // public:
    //     virtual ~FormatItem(){}
    //     virtual void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) = 0;
    // private:
    // };
    
    // bool isError() { return isError_; }
private:
    // std::string pattern_;
    // std::vector<std::shared_ptr<FormatItem> > items_;
    // bool isError_ = false;
    Timestamp stamp_;
    std::unique_ptr<Mutex> mutex_;
};

}   //lithe