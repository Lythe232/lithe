#include "logAppender.h"
#include "thread/mutex.h"

#include <string>

namespace lithe{

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
                            mutex_(threadSafe ? new Mutex() : nullptr),
                            startOfPeriod_(0),
                            lastRoll_(0),
                            lastFlush_(0)
{
    rollFile();
}
void FileAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    {
        if(mutex_)
        {
            MutexLockGuard lock(*mutex_);
            stream_.resetBuffer();
            formatter_->format(stream_, logger, level, event);
        }
        else
        {
            formatter_->format(stream_, logger, level, event);
        }
    }
    append(stream_.toString().c_str(), stream_.toString().size());

    //fprintf(stdout, "file_->wirttenBytes() = %ld\trollSize_ = %ld\n", file_->writtenBytes(), rollSize_);
}
bool FileAppender::rollFile()
{
    time_t now = stamp_.now().seconds();
    std::string filename = getLogFileName(basename_, &now);
    //fprintf(stdout, "filename = %s\n", filename.c_str());
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;
    if(now > lastRoll_)
    {
        lastRoll_ = now;
        lastFlush_ = now;
        startOfPeriod_ = start;
        file_.reset(new FileUtils::AppendFile(filename));
        return true;
    }
    return false;
}

void FileAppender::append_unlocked(const char *logline, size_t len)
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
void FileAppender::append(const char *logline, size_t len)
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
void FileAppender::flush()
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
StdoutAppender::StdoutAppender()
{
    mutex_.reset(new Mutex());
}
void StdoutAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    {
        MutexLockGuard lock(*mutex_);
        formatter_->format(stream_, logger, level, event);
    }
    
    fprintf(stdout, stream_.toString().c_str());
}

std::string FileAppender::getLogFileName(const std::string &basename, time_t *now)
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

} // lithe