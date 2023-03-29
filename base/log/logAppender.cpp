#include "logAppender.h"
#include "thread/mutex.h"

#include <string>

namespace lithe{



FileAppender::FileAppender(std::string basename, off_t rollSize, bool threadSafe, int flushInterval, int checkEveryN, bool isAsync) :
                            mutex_(threadSafe ? new Mutex() : nullptr),
                            async_(isAsync ? new AsyncLogging(basename, rollSize, flushInterval) : nullptr),
                            file_(isAsync ? nullptr : new LogFile(basename, rollSize, threadSafe, flushInterval, checkEveryN))

{
    if(async_)
    {
        async_->start();
    }
}
FileAppender::~FileAppender()
{

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
                stream_.resetBuffer();
                formatter_->format(stream_, logger, level, event);
            }
        }
    if(async_)
    {
        async_->append(stream_.toString().c_str(), stream_.toString().size());
    }
    else
    {
        file_->append(stream_.toString().c_str(), stream_.toString().size());

    }
    //fprintf(stdout, "file_->wirttenBytes() = %ld\trollSize_ = %ld\n", file_->writtenBytes(), rollSize_);
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


} // lithe