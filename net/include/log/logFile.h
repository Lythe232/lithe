#pragma once

#include "include/common/timestamp.h"
#include "include/common/fileUtils.h"
#include "include/common/singleton.h"
#include "include/thread/mutex.h"

#include <memory>

namespace lithe
{

class LogFile 
{
public:
    ~LogFile();
    LogFile(std::string basename, off_t rollSize, bool threadSafe, int flushInterval, int checkEveryN);

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
    Timestamp stamp_;
    int count_;
    std::unique_ptr<Mutex> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;
};

}   //namespace lithe
