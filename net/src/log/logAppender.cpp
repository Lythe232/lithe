#include "include/log/logAppender.h"
#include "include/thread/mutex.h"

#include <string>

namespace lithe{



FileAppender::FileAppender(std::string basename, off_t rollSize, int flushInterval, int checkEveryN) :
                            mutex_(new Mutex()),
                            file_(new LogFile(basename, rollSize, true, flushInterval, checkEveryN))

{
}
FileAppender::~FileAppender()
{
}
void FileAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    {
        MutexLockGuard lock(*mutex_);
        formatter_->format(stream_, logger, level, event);
    }
    // file_->append(stream_.buffer().data(), stream_.buffer().length());   // fails?

    file_->append(stream_.toString().c_str(), stream_.toString().size());
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
    
    fprintf(stdout, "%s", stream_.toString().c_str());
}

AsyncAppender::AsyncAppender(std::string basename, off_t rollSize, int flushInterval, int checkEveryN) :
                            async_(new AsyncLogging(basename, rollSize, flushInterval)),
                            mutex_(new Mutex())
{
    async_->start();
}
AsyncAppender::~AsyncAppender()
{
}
void AsyncAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, std::shared_ptr<LogEvent> event)
{
    {
        MutexLockGuard lock(*mutex_);
        formatter_->format(stream_, logger, level, event);
    }

    async_->append(stream_.buffer().data(), stream_.buffer().length());

}



} // lithe