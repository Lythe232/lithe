#include "include/log/logFormatter.h"
#include "include/log/logger.h"
#include "include/log/logEvent.h"

#include <map>
#include <memory>
#include <functional>
namespace lithe {

static const char* SourceFile(const char* filename)
{
    const char* c = strrchr(filename, '/');
    if(c)
    {
        const char* str = c + 1;
        return str;
    }
    return filename;
}

void LogFormatter::format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    // if(!items_.empty())
    // {
    //     for(auto& i : items_)
    //     {
    //         // printf("-------------------\n");
    //         i->format(stream, logger, level, event);
    //     }
    // }

    //%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
    stream.resetBuffer();
    struct tm tm;
    time_t now = time(NULL);
    gmtime_r(&now, &tm);
    char timebuf[32];
    strftime(timebuf, sizeof timebuf, "%Y-%m-%d %H:%M:%S", &tm);

    stream << timebuf;
    stream << "\t";
    stream << event->getThreadId();
    stream << "\t";
    stream << event->getThreadName();
    stream << "\t";
    stream << event->getFiberId();
    stream << "\t";
    stream << LogLevel::toString(event->getLevel());
    stream << "\t";
    stream << event->getLogger()->getName();
    stream << "\t";
    stream << SourceFile(event->getFile());
    stream << ":";
    stream << event->getLine();
    stream << "\t";
    stream << event->getContent();
    stream << "\n";
}
void LogFormatter::init()
{

}

    //m  p  r  c  t  n  d  f  l  T  F  N
// class MessageFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit MessageFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << event->getContent();
//     }
// private:
//     std::string str_;
// };
// class LevelFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit LevelFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << LogLevel::toString(event->getLevel());
//     }
// private:
// };
// class FilenameFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit FilenameFormatItem(std::string str = "") {}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << event->getFile();
//     }
// };
// class StringFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit StringFormatItem(std::string str) : str_(str)
//     {
//     }
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << str_;
//     }
// private:
//     std::string str_;
// };
// class LogNameFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit LogNameFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         //TODO
//         stream << event->getLogger()->getName();
//     }
// private:
//     std::string str_;
// };
// class ThreadIdFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit ThreadIdFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << event->getThreadId();
//     }
// private:
//     std::string str_;
// };
// class NewLineFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit NewLineFormatItem(std::string str = ""){}
//     void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         os << "\n";
//     }
// private:
//     std::string str_;
// };
// class ElapseFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit ElapseFormatItem(std::string str = ""){}
//     void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         os << event->getElapse();
//     }
// private:
//     std::string str_;
// };
// class DateTimeFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit DateTimeFormatItem(std::string fmt = "%Y-%m-%d %H:%M:%S") : fmt_(fmt) {}
//     void format(LogStream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         //TODO
//         struct tm tm;
//         char timebuf[32];
//         time_t now = event->getTime();
//         gmtime_r(&now, &tm);
//         strftime(timebuf, sizeof timebuf, fmt_.c_str(), &tm);
//         os << timebuf;
//     }
// private:
//     std::string fmt_;
// };
// class FiberFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit FiberFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         //TODO
//         stream << event->getFiberId();
//     }
// private:
//     std::string str_;
// };
// class LineFormaterItem : public LogFormatter::FormatItem
// {
// public:
//     explicit LineFormaterItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream <<  event->getLine();
//     }
// private:
// };
// class ThreadNameFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit ThreadNameFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << event->getThreadName();
//     }
// private:
// };
// class FiberIdFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit FiberIdFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << event->getFiberId();
//     }
// private:
//     std::string str_;
// };
// class TabFormatItem : public LogFormatter::FormatItem
// {
// public:
//     explicit TabFormatItem(std::string str = ""){}
//     void format(LogStream& stream, std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event) override
//     {
//         stream << "\t";
//     }
// private:
//     std::string str_;
// };



// void LogFormatter::init()
// {
//     std::string ss; //store string
//     std::vector<std::tuple<std::string, std::string, int>> vec;
//     for(int i = 0; i < pattern_.size(); i ++) {
//         if (pattern_[i] != '%') {
//             ss.append(1, pattern_[i]);
//             continue;
//         }
//         if (pattern_[i + 1] == '%') //%%
//         {
//             ss.append(1, '%');
//             continue;
//         }
//         std::string str;
//         std::string fmt;
//         int fmt_begin = 0;
//         int n =  i + 1;
//         int isParse = 0;

//         switch (pattern_[n]) {
//             //m  p  r  c  t  n  d  f  l  T  F  N
//             case 'm':   //Message
//                 str.append(1, 'm');
//                 break;
//             case 'p':   //LogLevel
//                 str.append(1, 'p');
//                 break;
//             case 'r':   //leiji haomiao
//                 str.append(1, 'r');
//                 break;
//             case 'c':   //Log Name
//                 str.append(1, 'c');
//                 break;
//             case 't':   //Thread Id
//                 str.append(1, 't');
//                 break;
//             case 'n':   //\n
//                 str.append(1, 'n');
//                 break;
//             case 'd':   //date
//                 str.append(1, 'd');
//                 break;
//             case 'f':   //Filename
//                 str.append(1, 'f');
//                 break;
//             case 'l':   //Line
//                 str.append(1, 'l');
//                 break;
//             case 'T':   //Tab
//                 str.append(1, 'T');
//                 break;
//             case 'F':   //Fiber Id
//                 str.append(1, 'F');
//                 break;
//             case 'N':   //Thread Name
//                 str.append(1, 'N');
//                 break;
//             default:
//                 ss.append(1, pattern_[n]);
//         }
//         while((n + 1) < pattern_.size())
//         {
//             if(!isParse)
//             {
//                 if(pattern_[n + 1] == '{')
//                 {
//                     fmt_begin = n + 1;
//                     n++;
//                     isParse = 1;
//                     continue;
//                 }
//                 break;
//             }
//             if(isParse)
//             {
//                 if(pattern_[n + 1] == '}')
//                 {
//                     fmt = pattern_.substr((fmt_begin + 1) , n - fmt_begin);
//                     n++;
//                     isParse = 0;
//                     break;
//                 }
//             }
//             n++;
//         }
//         if (!isParse)
//         {
//             if(!ss.empty())
//             {
//                 vec.push_back(std::make_tuple(ss, std::string(), 0));
//                 ss.clear();
//             }
//             i = (n);
//             if(!str.empty())
//             {
//                 vec.push_back(std::make_tuple(str, fmt, 1));
//                 str.clear();
//             }

//         }
//         else if(isParse)
//         {
//                 if(!ss.empty())
//                 {
//                     vec.push_back(std::make_tuple(ss, std::string(), 0));
//                     ss.clear();
//                 }
//                 vec.push_back(std::make_tuple("<<pattern error>> ", fmt, 0));
//                 isError_ = true;
//         }

//     }
//     static std::map<std::string, std::function<std::shared_ptr<FormatItem>(const std::string&)> > FmtMap = {
// #define XX(str, C) 
//             {#str, [](const std::string& s){ return std::shared_ptr<FormatItem>(new C(s)); }}
//         //m  p  r  c  t  n  d  f  l  T  F  N
//             XX(m, MessageFormatItem),
//             XX(p, LevelFormatItem),
//             XX(r, ElapseFormatItem),
//             XX(c, LogNameFormatItem),
//             XX(t, ThreadIdFormatItem),
//             XX(n, NewLineFormatItem),
//             XX(d, DateTimeFormatItem),
//             XX(f, FilenameFormatItem),
//             XX(l, LineFormaterItem),
//             XX(T, TabFormatItem),
//             XX(F, FiberIdFormatItem),
//             XX(N, ThreadNameFormatItem)
// #undef XX
//     };
//     for(auto& i : vec)
//     {
//         if(std::get<2>(i) == 1)
//         {
//             auto it = FmtMap.find(std::get<0> (i));
//             if(it == FmtMap.end())
//             {
//                 std::cout << "<<pattern error for " << std::get<0>(i) << ">>" << std::endl;
//                 isError_ = true;
//                 items_.push_back(std::shared_ptr<FormatItem>(new StringFormatItem("<<pattern error for " + std::get<0>(i) + ">>")));
//             }
//             items_.push_back(it->second(std::get<1>(i)));
//         }
//         else if (std::get<2>(i) == 0)
//         {
//             items_.push_back(std::shared_ptr<FormatItem>(new StringFormatItem(std::get<0>(i))));
//         }
//     }
// }

}