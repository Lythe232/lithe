
#include "include/common/fileUtils.h"

#include <assert.h>
#include <error.h>
#include <string.h>

namespace FileUtils
{
AppendFile::AppendFile(std::string s) : 
    fp_(fopen(s.c_str(), "ae"))
{
    if(!fp_)
    {
        printf("file not open errno = %d, errstr = %s\n", errno, strerror(errno));
    }
    assert(fp_);
    ::setbuffer(fp_, buffer_, sizeof buffer_);
}
AppendFile::~AppendFile()
{
    if(fp_)
    {
        flush();
        ::fclose(fp_);
    }
}
size_t AppendFile::write(const char* logline, size_t len)
{
    return ::fwrite_unlocked(logline, 1, len, fp_);
    // return ::fwrite(logline, 1, len, fp_);
}
void AppendFile::flush()
{
    ::fflush(fp_);
}
void AppendFile::append(std::string logline)
{
    size_t written = 0;
    size_t len = logline.size();
    while(written != len)
    {
        size_t n = len - written;
        size_t remain = write(logline.c_str() + written, n);

        if(remain != n)
        {
            int ferr = ferror(fp_);
            if(ferr)
            {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror(ferr));
                break;
            }
        }
        written += n;
    }
    writtenBytes_ += written;
}
void AppendFile::append(const char* logline, int len)
{
    size_t written = 0;
    size_t length = len;
    while(written != length)
    {
        size_t n = length - written;
        size_t remain = write(logline + written, n);
        if(remain != n)
        {
            int ferr = ferror(fp_);
            if(ferr)
            {
                fprintf(stderr, "AppendFile::append() failed %s\n", strerror(ferr));
                break;
            }
        }
        written += n;
    }
    writtenBytes_ += written;
}


}   //namespace FileUtils
