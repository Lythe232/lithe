#include "include/log/logFile.h"
#include "include/common/processInfo.h"

namespace lithe
{

LogFile::LogFile(std::string basename, 
                            off_t rollSize, 
                            bool threadSafe,
                            int flushInterval, 
                            int checkEveryN) 
                            : 
                            file_(new FileUtils::AppendFile()),
                            basename_(basename), 
                            rollSize_(rollSize), 
                            flushInterval_(flushInterval), 
                            checkEveryN_(checkEveryN),
                            stamp_(),
                            count_(0),
                            mutex_(threadSafe ? new Mutex() : nullptr),
                            startOfPeriod_(0),
                            lastRoll_(0),
                            lastFlush_(0)
{
    rollFile();
}
LogFile::~LogFile()
{
}
bool LogFile::rollFile()
{
    time_t now = 0;
    std::string filename = getLogFileName(basename_, &now);     //TODO
    if(now > lastRoll_)
    {
        time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new FileUtils::AppendFile(filename));
        return true;
    }
    return false;
}

void LogFile::append_unlocked(const char *logline, size_t len)
{
    file_->append(logline);

    if(file_->writtenBytes() >= rollSize_)
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
void LogFile::append(const char *logline, size_t len)
{
    if(mutex_)
    {
        MutexLockGuard lock(*mutex_);
        append_unlocked(logline, len);
    }
    else
    {
        append_unlocked(logline, len);
    }
}
void LogFile::flush()
{
    if(mutex_)
    {
        MutexLockGuard lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
}


std::string LogFile::getLogFileName(const std::string &basename, time_t *now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;
    
    char timebuf[32];
    
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm);
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();
    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());

    filename += pidbuf;
    filename += ".log";

    return filename;
}

}   //namespace lithe