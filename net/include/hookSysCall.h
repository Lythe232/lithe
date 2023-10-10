#pragma once
#include <unistd.h>
#include <stdint.h>
namespace lithe
{
    bool is_hook_enable();
    void set_hook_enable(bool flag);
}   //namespace lithe

extern "C"
{

//sleep
typedef unsigned int (*sleep_fun)(unsigned int seconds);
extern sleep_fun sys_sleep_fun;
typedef int (*usleep_fun)       (useconds_t usec);
extern usleep_fun sys_usleep_fun;
typedef int (*nanosleep_fun)        (const struct timespec* req, struct timespec* rem);
extern nanosleep_fun sys_nanosleep_fun;

//I/O
typedef ssize_t (*read_fun)     (int fd, void* buf, size_t count);
extern read_fun sys_read_fun;

typedef ssize_t (*readv_fun)    (int fd, const struct iovec* iov, int iovcnt);
extern readv_fun sys_readv_fun;


typedef ssize_t (*write_fun)    (int fd, const void* buf, size_t count);
extern write_fun sys_write_fun;


typedef ssize_t (*writev_fun)   (int fd, const struct iovec* iov, int iovcnt);
extern writev_fun sys_writev_fun;

typedef int     (*close_fun)    (int fd);
extern close_fun sys_close_fun;


//SOCKET
typedef int     (*shutdown_fun) (int sockfd, int how);
extern shutdown_fun sys_shutdown_fun;


typedef ssize_t (*recv_fun)     (int fd, void* buf, size_t len, int flags);
extern recv_fun sys_recv_fun;

typedef ssize_t (*recvfrom_fun) (int sockfd, void* buf, size_t len, int flags, struct sockaddr* src_addr, socklen_t* addrlen);
extern recvfrom_fun sys_recvfrom_fun;

typedef ssize_t (*recvmsg_fun)      (int sockfd, struct msghdr* src_addr, int flags);
extern recvmsg_fun sys_recvmsg_fun;

typedef ssize_t (*send_fun)     (int sockfd, const void* buf, size_t len, int flags);
extern send_fun sys_send_fun;

typedef ssize_t (*sendto_fun)   (int sockfd, const void* buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen);
extern sendto_fun sys_sendto_fun;

typedef ssize_t (*sendmsg_fun)  (int sockfd, const struct msghdr* msg, int flags);
extern sendmsg_fun sys_sendmsg_fun;

typedef int     (*socket_fun)   (int domain, int type, int protocol);
extern socket_fun sys_socket_fun;

typedef int     (*connect_fun)  (int sockfd, const struct sockaddr* addr, socklen_t addrlen);
extern connect_fun sys_connect_fun;
int connect_with_timeout(int sockfd, const struct sockaddr* addr, socklen_t addrlen, uint64_t timeout);

typedef int     (*accept_fun)   (int sockfd, struct sockaddr* addr, socklen_t* addrlen);
extern accept_fun sys_accept_fun;

typedef int     (*getsockopt_fun)   (int sockfd, int level, int optname, void* optval, socklen_t* optlen);
extern getsockopt_fun sys_getsockopt_fun;

typedef int     (*setsockopt_fun)   (int sockfd, int level, int optname, const void* optval, socklen_t optlen);
extern setsockopt_fun sys_setsockopt_fun;

typedef int     (*fcntl_fun)    (int fd, int cmd, ...);
extern fcntl_fun sys_fcntl_fun;

typedef int     (*ioctl_fun)    (int fd, unsigned long request, ...);
extern ioctl_fun sys_ioctl_fun;


}

