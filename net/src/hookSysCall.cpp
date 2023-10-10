#include "include/hookSysCall.h"
#include "include/fdmanager.h"
#include "include/coScheduler.h"
#include "include/timer.h"
#include "include/log/log.h"
#include "include/channel.h"
#include "include/poller.h"

#include <dlfcn.h>
#include <stdint.h>
#include <utility>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/ioctl.h>
static auto g_logger = LOG_ROOT();

read_fun sys_read_fun            = (read_fun)dlsym(RTLD_NEXT, "read");
readv_fun sys_readv_fun          = (readv_fun)dlsym(RTLD_NEXT, "readv");
recv_fun sys_recv_fun            = (recv_fun)dlsym(RTLD_NEXT, "recv");
recvfrom_fun sys_recvfrom_fun    = (recvfrom_fun)dlsym(RTLD_NEXT, "recvfrom");
recvmsg_fun sys_recvmsg_fun      = (recvmsg_fun)dlsym(RTLD_NEXT, "recvmsg");
write_fun sys_write_fun          = (write_fun)dlsym(RTLD_NEXT, "write");
writev_fun sys_writev_fun        = (writev_fun)dlsym(RTLD_NEXT, "writev");
send_fun sys_send_fun            = (send_fun)dlsym(RTLD_NEXT, "send");
sendto_fun sys_sendto_fun        = (sendto_fun)dlsym(RTLD_NEXT, "sendto");
sendmsg_fun sys_sendmsg_fun      = (sendmsg_fun)dlsym(RTLD_NEXT, "sendmsg");
socket_fun sys_socket_fun        = (socket_fun)dlsym(RTLD_NEXT, "socket");
connect_fun sys_connect_fun      = (connect_fun)dlsym(RTLD_NEXT, "connect");
accept_fun sys_accept_fun        = (accept_fun)dlsym(RTLD_NEXT, "accept");
fcntl_fun sys_fcntl_fun          = (fcntl_fun)dlsym(RTLD_NEXT, "fcntl");
ioctl_fun sys_ioctl_fun          = (ioctl_fun)dlsym(RTLD_NEXT, "ioctl");
sleep_fun sys_sleep_fun          = (sleep_fun)dlsym(RTLD_NEXT, "sleep");
usleep_fun sys_usleep_fun        = (usleep_fun)dlsym(RTLD_NEXT, "usleep");
nanosleep_fun sys_nanosleep_fun  = (nanosleep_fun)dlsym(RTLD_NEXT, "nanosleep");
setsockopt_fun sys_setsockopt_fun = (setsockopt_fun)dlsym(RTLD_NEXT, "setsockopt");
getsockopt_fun sys_getsockopt_fun = (getsockopt_fun)dlsym(RTLD_NEXT, "getsockopt");
close_fun sys_close_fun          = (close_fun)dlsym(RTLD_NEXT, "close");
shutdown_fun sys_shutdown_fun    = (shutdown_fun)dlsym(RTLD_NEXT, "shutdown");
namespace lithe
{
static thread_local bool t_hook_sys_fun = false;

bool is_hook_enable()
{
    return t_hook_sys_fun;
}
void set_hook_enable(bool flag)
{
    t_hook_sys_fun = flag;
    
}



// #define HOOK_FUN(XX)    
//     XX(read)            
//     XX(readv)           
//     XX(recv)            
//     XX(recvfrom)        
//     XX(recvmsg)         
//     XX(write)           
//     XX(writev)          
//     XX(send)            
//     XX(sendto)          
//     XX(sendmsg)         
//     XX(socket)          
//     XX(connect)         
//     XX(accept)          
//     XX(fcntl)           
//     XX(ioctl)           
//     XX(sleep)           
//     XX(usleep)          
//     XX(nanosleep)       
//     XX(setsockopt)      
//     XX(getsockopt)      
//     XX(close)           
// void hook_init()
// {
//     static bool is_inited = false;
//     if(is_inited) {
//         return;
//     }
// #define XX(name)        
//     sys_##name##_fun = (name##_fun)dlsym(RTLD_NEXT, #name)

// #undef XX
// }



// class _HookIniter
// {
// public:
//     _HookIniter()
//     {
//         printf("___HOOK_INITER__\n");
//         hook_init();
//     }
// };

// static _HookIniter s_hook_initer;

}   //namespace lithe


struct timer_info
{
    int cancelled = 0;
};

// 判断当前是否启用了IO Hook模式，如果没有，则直接调用系统IO函数并返回结果。
// 获取fd对应的管理信息，如果不存在，则直接调用系统IO函数并返回结果。
// 检查fd是否已经关闭，如果已经关闭，则设置errno为EBADF并返回-1。
// 检查fd是否为socket类型，如果不是，则直接调用系统IO函数并返回结果。
// 检查fd是否设置为非阻塞模式，如果是，则直接调用系统IO函数并返回结果。
// 获取该fd的超时时间，并创建一个 timer_info 对象，记录该IO事件是否被取消。
// 进入 do-while 循环中，尝试调用系统IO函数进行IO操作。
// 如果返回值小于 0 并且 errno 为 EINTR，则表明是中断信号导致的错误，需要继续重试。
// 如果返回值小于 0 并且 errno 为 EAGAIN，则表明 IO 事件需要异步等待，此时进入下一步处理。
// 创建一个协程条件变量和一个定时器，并向全局 IO 复用模块注册事件。
// 如果需要设置超时时间，则再额外创建一个定时器。
// 在等待 IO 事件完成时，当前协程会处于等待状态，当 IO 事件完成后，协程会被自动唤醒，并执行回调函数。
// 在回调函数中，先检查 IO 事件是否被取消或超时，然后取消 IO 事件，并唤醒对应的协程。
// 如果 IO 事件未超时且未被取消，则返回最终的 IO 操作结果。
// 如果 IO 事件超时或被取消，则返回 -1 并设置 errno 为对应的错误码。
template<typename OriginFun, typename... Args>
static ssize_t do_io(int fd, OriginFun sysFun, const char* hookFunName, int event, int timeout_so, Args&&... args)
{
    if(!lithe::is_hook_enable())
    {
        return sysFun(fd, std::forward<Args>(args)...);
    }
    std::shared_ptr<lithe::Fd> spFd = lithe::FdMgr::getSingletonPtr()->get(fd);
    if(!spFd)
    {
        return sysFun(fd, std::forward<Args>(args)...);
    }

    if(spFd->isClosed())
    {
        errno = EBADF;
        return -1;
    }
    if(!spFd->isSocket() || spFd->getUserNonblock())
    {
        return sysFun(fd, std::forward<Args>(args)...);
    }

    uint64_t timeout = spFd->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info());
    ssize_t rt = 0;

    while(1)
    {
        rt = sysFun(fd, std::forward<Args>(args)...);
        if(rt == -1 && errno == EINTR)
        {
            continue;
        }
        else if(rt == -1 && errno == EAGAIN)
        {
            auto scheduler = lithe::CoScheduler::getThis();
            auto poller = scheduler->getPoller();
            std::shared_ptr<lithe::Timer> timer;
            std::weak_ptr<timer_info> winfo(tinfo);
            std::shared_ptr<lithe::TimerManager> tm = scheduler->getTimerManager();
            if(timeout != (uint64_t)-1)
            {
                timer = tm->addConditionTimer(timeout, [winfo, poller, fd](){
                    auto w = winfo.lock();
                    printf("conditionTimer fd = %d\n", fd);
                    if(!w || w->cancelled)
                    {
                        return ;
                    }
                    w->cancelled = ETIMEDOUT;
                    poller->delEvent(fd);
                    // poller->cancelAllEvent(fd);
                }, winfo);
            }   //timeout != -1

            event |= (EPOLLERR | EPOLLHUP);
            LOG_INFO(g_logger) << hookFunName << " addEvent(" << fd << ", " << event << ").";
            int err = poller->addEvent(fd, event);
            if(err)
            {
                if(timer)
                {
                    tm->cancelTimer(timer);
                }
                return -1;
            }
            else
            {
                lithe::CoScheduler::yieldToHold();
                if(timer)
                {
                    tm->cancelTimer(timer);
                }
                if(tinfo->cancelled)
                {
                    errno = tinfo->cancelled;   //TIMEOUT
                    return -1;
                }
                continue;
            }
        }   //errno = EAGAIN
        else
        {
            // return rt;
        }
        return rt;
    }   //while

    return rt;
}

extern "C"
{
// #define XX(name) name##_fun sys_##name##_fun = nullptr;
//     HOOK_FUN(XX);
// #undef XX

unsigned int sleep(unsigned seconds)
{
    if(!lithe::is_hook_enable())
    {
        return sys_sleep_fun(seconds);
    }
    auto cor = lithe::Coroutine::getRunCo();
    assert(cor);
    auto scheduler = lithe::CoScheduler::getThis();
    assert(scheduler);
    auto tm = scheduler->getTimerManager();
    assert(tm);
    tm->addTimer(seconds * 1000,
                std::bind((void (lithe::CoScheduler::*)(std::shared_ptr<lithe::Coroutine>))&lithe::CoScheduler::schedule, scheduler, cor));
    lithe::CoScheduler::yieldToHold();
    return 0;
}

int usleep(useconds_t usec)
{
    if(!lithe::is_hook_enable())
    {
        sys_usleep_fun(usec);
    }

    auto scheduler = lithe::CoScheduler::getThis();
    auto tm = scheduler->getTimerManager();
    auto cor = lithe::Coroutine::getRunCo();
    tm->addTimer((uint64_t)(usec / 1000),
                   std::bind((void (lithe::CoScheduler::*)
                   (std::shared_ptr<lithe::Coroutine>))&lithe::CoScheduler::schedule, 
                   scheduler, cor));
    lithe::CoScheduler::yieldToHold();
    return 0;
}

int nanosleep(const struct timespec* req, struct timespec* rem)
{
    if(!lithe::is_hook_enable())
    {
        sys_nanosleep_fun(req, rem);
    }
    uint64_t timeout = (req->tv_nsec / 1000 / 1000) + (req->tv_sec * 1000);
    auto scheduler = lithe::CoScheduler::getThis();
    auto tm = scheduler->getTimerManager();
    auto cor = lithe::Coroutine::getRunCo();

    tm->addTimer(timeout,
            std::bind((void (lithe::CoScheduler::*)
                        (std::shared_ptr<lithe::Coroutine>))&lithe::CoScheduler::schedule, scheduler, cor));
    lithe::CoScheduler::yieldToHold();
    return 0;
}

int socket(int domain, int type, int protocol)
{
    if(!lithe::is_hook_enable())
    {
        return sys_socket_fun(domain, type, protocol);
    }

    int fd = sys_socket_fun(domain, type, protocol);

    if(fd == -1)
    {
        return fd;
    }
    lithe::FdMgr::getSingletonPtr()->get(fd, true);
    return fd;
}

int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen)
{
    socklen_t len = sizeof(sockaddr);
    int fd = do_io(sockfd, sys_accept_fun, "sys_accept_fun", EPOLLIN, SO_RCVTIMEO, addr, &len);
    if(fd >= 0)
    {   
        lithe::FdMgr::getSingletonPtr()->get(fd, true);
    }
    return fd;
}
int connect_with_timeout(int sockfd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout)
{
if(!lithe::is_hook_enable())
    {
        return sys_connect_fun(sockfd, addr, addrlen);
    }

    auto spFd = lithe::FdMgr::getSingletonPtr()->get(sockfd);
    if(spFd->isClosed())
    {
        errno = EBADF;
        return -1;
    }
    if(!spFd->isSocket())
    {
        return sys_connect_fun(sockfd, addr, addrlen);
    }
    if(spFd->getUserNonblock())
    {
        return sys_connect_fun(sockfd, addr, addrlen);
    }

    int fd = sys_connect_fun(sockfd, addr, addrlen);
    if(fd == -1 && errno == EINPROGRESS)
    {
        return fd;
    }
    else if(fd == 0)
    {
        return 0;
    }
    auto scheduler = lithe::CoScheduler::getThis();
    auto tm = scheduler->getTimerManager();
    auto poller = scheduler->getPoller();
    std::shared_ptr<lithe::Timer> timer;
    std::shared_ptr<timer_info> tinfo;
    std::weak_ptr<timer_info> winfo(tinfo);
    timer = tm->addConditionTimer(spFd->getTimeout(SO_RCVTIMEO), [poller, timer, winfo, fd](){
        
        auto w = winfo.lock();
        if(!w || w->cancelled)
        {
            return ;
        }
        w->cancelled = ETIMEDOUT;
        poller->delEvent(fd);   ///////////////////////
    }, winfo);

    int rt = poller->addEvent(fd, EPOLLIN);
    if(rt == 0)
    {
        lithe::CoScheduler::yieldToHold();
        if(timer)
        {
            tm->cancelTimer(timer);
        }
        if(tinfo->cancelled)
        {
            errno = tinfo->cancelled;
            return -1;
        }
    }
    else
    {
        if(timer)
        {
            tm->cancelTimer(timer);
        }
        //TODO LOG
    }
    int err = 0;
    socklen_t len = sizeof(int);
    if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) == -1)
    {
        return -1;
    }
    if(!err)
    {
        return 0;
    }
    else
    {
        errno = err;
        return -1;
    }
}
int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen)
{
    return connect_with_timeout(sockfd, addr, addrlen, 3000);   //set
}
ssize_t read(int fd, void* buf, size_t count)
{
    ssize_t rt = do_io(fd, sys_read_fun, "sys_read_fun", (EPOLLIN), SO_RCVTIMEO, buf, count);
    return rt;
}
ssize_t readv(int fd, const struct iovec* iov, int iovcnt)
{
    return do_io(fd, sys_readv_fun, "sys_readv_fun", (EPOLLIN), SO_RCVTIMEO, iov, iovcnt);
}
ssize_t recv(int sockfd, void* buf, size_t len, int flags)
{
    return do_io(sockfd, sys_recv_fun, "sys_recv_fun", (EPOLLIN), SO_RCVTIMEO, buf, len, flags);
}
ssize_t recvfrom(int sockfd, void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen)
{
    return do_io(sockfd, sys_recvfrom_fun, "sys_recvfrom_fun", (EPOLLIN), SO_RCVTIMEO, buf, len, flags, src_addr, addrlen);
}
ssize_t recvmsg(int sockfd, struct msghdr* msg, int flags)
{
    return do_io(sockfd, sys_recvmsg_fun, "sys_recvfrom_fun", (EPOLLIN), SO_RCVTIMEO, msg, flags);
}
ssize_t write(int fd, const void* buf, size_t count)
{
    return do_io(fd, sys_write_fun, "sys_write_fun", (EPOLLOUT), SO_SNDTIMEO, buf, count);
}
ssize_t writev(int fd, const struct iovec* iov, int iovcnt)
{
    return do_io(fd, sys_writev_fun, "sys_writev_fun", (EPOLLOUT), SO_SNDTIMEO, iov, iovcnt);
}
ssize_t send(int sockfd, const void* buf, size_t len, int flags)
{
    return do_io(sockfd, sys_send_fun, "sys_send_fun", (EPOLLOUT), SO_SNDTIMEO, buf, len, flags);
}
ssize_t sendto(int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen)
{
    return do_io(sockfd, sys_sendto_fun, "sys_sendto_fun", (EPOLLOUT), SO_SNDTIMEO, buf, len, flags, dest_addr, addrlen);
}
ssize_t sendmsg(int sockfd, const struct msghdr* msg, int flags)
{
    return do_io(sockfd, sys_sendmsg_fun, "sys_sendmsg_fun", (EPOLLOUT), SO_SNDTIMEO, msg, flags);
}
int close(int fd)
{
    if(!lithe::is_hook_enable())
    {
        return sys_close_fun(fd);
    }
    auto spFd = lithe::FdMgr::getSingletonPtr()->get(fd, false);
    if(spFd)
    {
        lithe::FdMgr::getSingletonPtr()->del(fd);
    }
    printf("close() fd[%d]\n", fd);
    return sys_close_fun(fd);
}
int shutdown(int sockfd, int how)
{
    if(!lithe::is_hook_enable())
    {
        return sys_shutdown_fun(sockfd, how);
    }
    auto spFd = lithe::FdMgr::getSingletonPtr()->get(sockfd, false);
    if(spFd)
    {
        auto scheduler = lithe::CoScheduler::getThis();
        auto poller = scheduler->getPoller();
        if(poller)
        {
            poller->delEvent(sockfd);
        }
        lithe::FdMgr::getSingletonPtr()->del(sockfd);
    }
    return sys_shutdown_fun(sockfd, how);
}
int fcntl(int fd, int cmd, ...)
{
    va_list va;
    va_start(va, cmd);
    switch(cmd)
    {
        case F_SETFL:
        {
            int arg = va_arg(va, int);
            va_end(va);
            auto spFd = lithe::FdMgr::getSingletonPtr()->get(fd);
            if(!spFd || spFd->isClosed() || !spFd->isSocket())
            {
                return sys_fcntl_fun(fd, cmd, arg);
            }
            spFd->setUserNonblock(arg & O_NONBLOCK);
            if(spFd->getSysNonblock())
            {
                arg |= O_NONBLOCK;
            }
            else
            {
                arg &= ~O_NONBLOCK;
            }
            return sys_fcntl_fun(fd, cmd, arg);
        }
        break;
        case F_GETFL:
        {
            va_end(va);
            int arg = sys_fcntl_fun(fd, cmd);
            auto spFd = lithe::FdMgr::getSingletonPtr()->get(fd);
            if(!spFd || spFd->isClosed() || !spFd->isSocket())
            {
                return arg;
            }
            if(spFd->getUserNonblock())
            {
                return arg | O_NONBLOCK;
            }
            else
            {
                return arg & ~O_NONBLOCK;
            }
        }
            break;
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ:
#endif
            {
                int arg = va_arg(va, int);
                va_end(va);
                return sys_fcntl_fun(fd, cmd, arg); 
            }
            break;
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
            {
                va_end(va);
                return sys_fcntl_fun(fd, cmd);
            }
            break;
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK:
            {
                struct flock* arg = va_arg(va, struct flock*);
                va_end(va);
                return sys_fcntl_fun(fd, cmd, arg);
            }
            break;
        case F_GETOWN_EX:
        case F_SETOWN_EX:
            {
                struct f_owner_exlock* arg = va_arg(va, struct f_owner_exlock*);
                va_end(va);
                return sys_fcntl_fun(fd, cmd, arg);
            }
            break;
        default:
            va_end(va);
            return sys_fcntl_fun(fd, cmd);
    }
}

int ioctl(int fd, unsigned long request, ...)
{
    va_list va;
    va_start(va, request);
    void* arg = va_arg(va, void*);
    va_end(va);

    if(FIONBIO == request)
    {
        bool un = !!*(int*)arg;
        auto spFd = lithe::FdMgr::getSingletonPtr()->get(fd);
        if(!spFd || spFd->isClosed() || !spFd->isSocket())
        {
            return sys_ioctl_fun(fd, request, arg);
        }
        spFd->setUserNonblock(un);
    }
    return sys_ioctl_fun(fd, request, arg);
}

int getsockopt(int sockfd, int level, int optname, void* optval, socklen_t* optlen)
{
    return sys_getsockopt_fun(sockfd, level, optname, optval, optlen);
}

int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen)
{
    if(!lithe::is_hook_enable())
    {
        return sys_setsockopt_fun(sockfd, level, optname, optval, optlen);
    }
    if(level == SOL_SOCKET)
    {
        if(optname == SO_RCVTIMEO || optname == SO_SNDTIMEO)
        {
            auto spFd = lithe::FdMgr::getSingletonPtr()->get(sockfd);
            if(spFd)
            {
                const timeval* tv = (const timeval*)optval;
                spFd->setTimeout(optname, tv->tv_sec * 1000 + tv->tv_usec / 1000);
            }
        }
    }
    return sys_setsockopt_fun(sockfd, level, optname, optval, optlen);
}

}