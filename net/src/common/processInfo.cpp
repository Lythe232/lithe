#include "include/common/processInfo.h"

#include <sys/types.h>
#include <unistd.h>
namespace ProcessInfo
{

pid_t pid()
{
    return ::getpid();
}

std::string hostname()
{
    char buf[256];
    if(::gethostname(buf, sizeof buf) == 0)
    {
        buf[sizeof(buf) - 1] = '\0';
        return buf;
    }
    else
    {
        return "unkownhost";
    }
}
int tid()
{
    return ::gettid();
}

}