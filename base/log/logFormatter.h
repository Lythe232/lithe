#pragma once

#include "logStream.h"
#include "logger.h"

#include <vector>


namespace lithe
{

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
    bool isError() { return isError_; }
private:
    std::string pattern_;
    std::vector<std::shared_ptr<FormatItem> > items_;
    bool isError_ = false;
};

}   //lithe