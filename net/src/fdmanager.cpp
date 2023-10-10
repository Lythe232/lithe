#include "include/fdmanager.h"
#include "include/thread/currentThread.h"
#include "include/hookSysCall.h"
#include "include/log/log.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>

namespace lithe
{
static auto g_logger = LOG_ROOT();
lithe::Fd::Fd(int fd) :
    fd_(fd),
    isInit_(false),
    isClosed_(false),
    isSocket_(false),
    sysNonblock_(false),
    userNonblock_(false),
    recvTimeout_(-1),
    sendTimeout_(-1)
{
    init();
}

Fd::~Fd()
{
}

bool lithe::Fd::init()
{
    if(isInit_)
    {
        return true;
    }
    struct stat fdStat;
    if(fstat(fd_, &fdStat) == -1)
    {
        isInit_ = false;
        isSocket_ = false;
    }
    else
    {
        isInit_ = true;
        isSocket_ = S_ISSOCK(fdStat.st_mode);
    }
    if(isSocket_)
    {
        //TODO change to hook
        int f = sys_fcntl_fun(fd_, F_GETFL, 0);
        if(f < 0)
        {
            LOG_FATAL(g_logger) << "Fd::Fd() sys_fcntl_fun fails. errno[" << errno << "]. strerror = " << strerror(errno);
        }

        if(!(f & O_NONBLOCK))
        {
            if( sys_fcntl_fun(fd_, F_SETFL, f | O_NONBLOCK) < 0)
            {
                LOG_FATAL(g_logger) << "Fd::Fd() sys_fcntl_fun fails. errno[" << errno << "]. strerror = " << strerror(errno);
            }
        }
        sysNonblock_ = true;
    }
    else
    {
        sysNonblock_ = false;
    }

    userNonblock_ = false;
    isClosed_ = false;
    return isInit_;
}

void Fd::setSysNonblock(bool flag)
{
    int f = sys_fcntl_fun(fd_, F_GETFL, 0);
    if(f < 0)
    {
        LOG_FATAL(g_logger) << "Fd::setSysNonblock() sys_fcntl_fun fails. errno[" << errno << "]. strerror = " << strerror(errno);
        printf("Fd::setSysNonblock() sys_fcntl_fun fails. errno[%d]. strerror = %s\n", errno, strerror(errno));
        sysNonblock_ = false;
    }
    if(flag)
    {
        if(!(f & O_NONBLOCK))
        {
            if(sys_fcntl_fun(fd_, F_SETFL, f | O_NONBLOCK) < 0)
            {
                printf("Fd::setSysNonblock() sys_fcntl_fun fails. errno[%d]. strerror = %s\n", errno, strerror(errno));
                LOG_FATAL(g_logger) << "Fd::setSysNonblock() sys_fcntl_fun fails. errno[" << errno << "]. strerror = " << strerror(errno);
                sysNonblock_ = !flag;
            }
        }
        sysNonblock_ = flag;
    }
    else
    {
        if((f & O_NONBLOCK))
        {
            if(sys_fcntl_fun(fd_, F_SETFL, f & ~O_NONBLOCK) < 0)
            {
                printf("Fd::setSysNonblock() sys_fcntl_fun fails. errno[%d]. strerror = %s\n", errno, strerror(errno));


                LOG_FATAL(g_logger) << "Fd::setSysNonblock() sys_fcntl_fun fails. errno[" << errno << "]. strerror = " << strerror(errno);
                sysNonblock_ = !flag;
            }
        }
        sysNonblock_ = flag;
    }
}

void lithe::Fd::setTimeout(int type, uint64_t timeout)
{
    if(type == SO_RCVTIMEO)
    {
        recvTimeout_ = timeout;
    }
    else
    {
        sendTimeout_ = timeout;
    }
}

uint64_t lithe::Fd::getTimeout(int type)
{
    if(type == SO_RCVTIMEO)
    {
        return recvTimeout_;
    }
    else
    {
        return sendTimeout_;
    }
}

lithe::FdManager::FdManager() : 
    mutex_(new Mutex())
{
    fds_.resize(64);
}

FdManager::~FdManager()
{
}

std::shared_ptr<Fd> lithe::FdManager::get(int fd, bool autoCreate)
{
    MutexLockGuard lock(*mutex_);
    if((int)fds_.size() <= fd)
    {
        if(!autoCreate)
        {
            assert(0);
            return nullptr;
        }
        fds_.resize(fd * 2);
    }
    else
    {
        if(fds_[fd])
        {
            return fds_[fd];
        }
    }
    std::shared_ptr<Fd> newFd = std::make_shared<Fd>(fd);
    fds_[fd] = newFd;
    return newFd;
    // newFd.reset(new Fd(fd)); 
}

void lithe::FdManager::del(int fd)
{
    MutexLockGuard lock(*mutex_);
    if((int)fds_.size() <= fd)
    {
        return ;
    }
    fds_[fd].reset();
}


}   //namespace lithe
