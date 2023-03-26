//
// Created by lythe on 3/6/23.
//

#include "log.h"
#include "fileUtils.h"
#include "assert.h"
namespace lithe {

    //m  p  r  c  t  n  d  f  l  T  F  N
class MessageFormatItem : public LogFormatter::FormatItem
{
public:
    explicit MessageFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << event->getContent();
        
    }
private:
    std::string str_;
};
class LevelFormatItem : public LogFormatter::FormatItem
{
public:
    explicit LevelFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << event->getLevel();
    }
private:
};
class StringFormatItem : public LogFormatter::FormatItem
{
public:
    explicit StringFormatItem(std::string str) : str_(str)
    {
    }
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << str_;
    }
private:
    std::string str_;
};
class LogNameFormatItem : public LogFormatter::FormatItem
{
public:
    explicit LogNameFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        //TODO
    }
private:
    std::string str_;
};
class ThreadIdFormatItem : public LogFormatter::FormatItem
{
public:
    explicit ThreadIdFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << event->getThreadId();
    }
private:
    std::string str_;
};
class NewLineFormatItem : public LogFormatter::FormatItem
{
public:
    explicit NewLineFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << "\n";
    }
private:
    std::string str_;
};
class ElapseFormatItem : public LogFormatter::FormatItem
{
public:
    explicit ElapseFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << event->getElapse();
    }
private:
    std::string str_;
};
class DateTimeFormatItem : public LogFormatter::FormatItem
{
public:
    explicit DateTimeFormatItem(std::string fmt = "%Y-%m-%d %H:%M:%S") : fmt_(fmt) {}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        //TODO
    }
private:
    std::string fmt_;
};
class FiberFormatItem : public LogFormatter::FormatItem
{
public:
    explicit FiberFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        //TODO
    }
private:
    std::string str_;
};
class LineFormaterItem : public LogFormatter::FormatItem
{
public:
    explicit LineFormaterItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os <<  event->getLine();
    }
private:
};
class ThreadNameFormatItem : public LogFormatter::FormatItem
{
public:
    explicit ThreadNameFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << event->getThreadName();
    }
private:
};
class FiberIdFormatItem : public LogFormatter::FormatItem
{
public:
    explicit FiberIdFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << event->getFiberId();
    }
private:
    std::string str_;
};
class TabFormatItem : public LogFormatter::FormatItem
{
public:
    explicit TabFormatItem(std::string str = ""){}
    void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
    {
        os << "\t";
    }
private:
    std::string str_;
};
Logger::Logger(std::string name) : name_(name)
{
    level_ = LogLevel::DEBUG;
    formatter_.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}
void Logger::log(LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    if(level >= level_)
    {
        auto self = shared_from_this();
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
void Logger::setFormatter() {
    if (!appenders_.empty())
    {
        for(auto& it : appenders_)
        {
            if(!it->hasFormatter())
            {
                it->setFormatter(true);
                it->setFormatter(formatter_);
            }
        }
    }

}
void Logger::addAppender(std::shared_ptr<LogAppender> appender)
{
    appenders_.push_back(appender);
}
void Logger::delAppender(std::shared_ptr<LogAppender> appender)
{
    if(!appenders_.empty())
    {
        for(auto i = appenders_.begin(); i != appenders_.end(); i++)
        {
            if(*i == appender)
            {
                appenders_.erase(i);
            }
        }
    }
}
    //#Logger
//######################################################################

std::stringstream& LogFormatter::format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    if(!items_.empty())
    {
        for(auto& i : items_)
        {
            i->format(stream, logger, level, event);
        }
    }
}

//std::ostream& LogFormatter::format(std::ostream& ofs, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
//{
//}
void LogFormatter::init()
{
    std::string ss; //store string
    std::vector<std::tuple<std::string, std::string, int>> vec;
    for(int i = 0; i < pattern_.size(); i ++) {
        if (pattern_[i] != '%') {
            ss.append(1, pattern_[i]);
            continue;
        }
        if (pattern_[i + 1] == '%') //%%
        {
            ss.append(1, '%');
            continue;
        }
        std::string str;
        std::string fmt;
        int fmt_begin = 0;
        int n =  i + 1;
        int isParse = 0;

        switch (pattern_[n]) {
            //m  p  r  c  t  n  d  f  l  T  F  N
            case 'm':   //Message
                str.append(1, 'm');
                break;
            case 'p':   //LogLevel
                str.append(1, 'p');
                break;
            case 'r':   //leiji haomiao
                str.append(1, 'r');
                break;
            case 'c':   //Log Name
                str.append(1, 'c');
                break;
            case 't':   //Thread Id
                str.append(1, 't');
                break;
            case 'n':   //\n
                str.append(1, 'n');
                break;
            case 'd':   //date
                str.append(1, 'd');
                break;
            case 'f':   //Fiber
                str.append(1, 'f');
                break;
            case 'l':   //Line
                str.append(1, 'l');
                break;
            case 'T':   //Tab
                str.append(1, 'T');
                break;
            case 'F':   //Fiber Id
                str.append(1, 'F');
                break;
            case 'N':   //Thread Name
                str.append(1, 'N');
                break;
            default:
                ss.append(1, pattern_[n]);
        }
        while((n + 1) < pattern_.size())
        {
            if(!isParse)
            {
                if(pattern_[n + 1] == '{')
                {
                    fmt_begin = n + 1;
                    n++;
                    isParse = 1;
                    continue;
                }
                break;
            }
            if(isParse)
            {
                if(pattern_[n + 1] == '}')
                {
                    fmt = pattern_.substr((fmt_begin + 1) , n - fmt_begin);
                    n++;
                    isParse = 0;
                    break;
                }
            }
            n++;
        }
        if (!isParse)
        {
            if(!ss.empty())
            {
                vec.push_back(std::make_tuple(ss, std::string(), 0));
                ss.clear();
            }
            i = (n);
            if(!str.empty())
            {
                vec.push_back(std::make_tuple(str, fmt, 1));
                str.clear();
            }

        }
        else if(isParse)
        {
                if(!ss.empty())
                {
                    vec.push_back(std::make_tuple(ss, std::string(), 0));
                    ss.clear();
                }
                vec.push_back(std::make_tuple("<<pattern error>> ", fmt, 0));
                isError_ = true;
        }

    }
    static std::map<std::string, std::function<std::shared_ptr<FormatItem>(const std::string&)> > FmtMap = {
#define XX(str, C) \
            {#str, [](const std::string& s){ return std::shared_ptr<FormatItem>(new C(s)); }}
        //m  p  r  c  t  n  d  f  l  T  F  N
            XX(m, MessageFormatItem),
            XX(p, LevelFormatItem),
            XX(r, ElapseFormatItem),
            XX(c, LogNameFormatItem),
            XX(t, ThreadIdFormatItem),
            XX(n, ThreadNameFormatItem),
            XX(d, DateTimeFormatItem),
            XX(f, FiberFormatItem),
            XX(l, LineFormaterItem),
            XX(T, TabFormatItem),
            XX(F, FiberIdFormatItem),
            XX(N, NewLineFormatItem)
#undef XX
    };
    for(auto& i : vec)
    {
        if(std::get<2>(i) == 1)
        {
            auto it = FmtMap.find(std::get<0> (i));
            if(it == FmtMap.end())
            {
                std::cout << "<<pattern error for " << std::get<0>(i) << ">>" << std::endl;
                isError_ = true;
                items_.push_back(std::shared_ptr<FormatItem>(new StringFormatItem("<<pattern error for " + std::get<0>(i) + ">>")));
            }
            items_.push_back(it->second(std::get<1>(i)));
        }
        else if (std::get<2>(i) == 0)
        {
            items_.push_back(std::shared_ptr<FormatItem>(new StringFormatItem(std::get<0>(i))));
        }
    }
}
FileAppender::~FileAppender()
{
}
FileAppender::FileAppender(std::string basename, 
                            off_t rollSize, 
                            bool threadSafe,
                            int flushInterval, 
                            int checkEveryN) 
                            : 
                            basename_(basename), 
                            rollSize_(rollSize), 
                            flushInterval_(flushInterval), 
                            checkEveryN_(checkEveryN),
                            count_(0),
                            mutex_(threadSafe ? 1 : 0),
                            startOfPeriod_(0),
                            lastRoll_(0),
                            lastFlush_(0)
{
    rollFile();
}
void FileAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    formatter_->format(stream_, logger, level, event);
}
void FileAppender::rollFile()
{
    std::string filename = nullptr;
    file_.reset(new FileUtils::AppendFile(filename));

}

void FileAppender::append_unlocked(const char *logline, size_t len)
{
    file_->append(logline);
    if(file_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    else
    {
        ++count_;
        if(count_ >= checkEveryN_)
        {
            count_ = 0;
            time_t now = ::time(NULL);
            time_t thisOfPeriod = now / kRollPerSeconds_ * kRollPerSeconds_;
            if(thisOfPeriod != startOfPeriod_)
            {
                rollFile();                
            }
            else if(now - lastFlush_ > flushInterval_)
            {
                lastFlush_ = now;
                file_->flush();
            }
        }
        
    }
}
void FileAppender::append(const char *logline, size_t len)
{
    
}
void StdoutAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    formatter_->format(stream_, logger, level, event);
    fprintf(stdout, stream_.c_str());
}

} // lithe