#pragma once

#include "thread/mutex.h"
#include "common/singleton.h"

#include <stdint.h>
#include <vector>
#include <memory>


namespace lithe
{

class Fd
{
public:
    //
    Fd(int fd);
    ~Fd();
    int getFd()                     { return fd_; }
    bool isClosed()                 { return isClosed_; }
    bool isSocket()                 { return isSocket_; }
    void setSysNonblock(bool flag);
    void setUserNonblock(bool flag) { userNonblock_ = flag;  }
    void setTimeout(int type, uint64_t timeout);
    uint64_t getTimeout(int type);
    bool getUserNonblock()          { return userNonblock_; }
    bool getSysNonblock()          { return sysNonblock_; }
private:
    bool init();

    int fd_;
    bool isInit_        : 1;
    bool isClosed_      : 1;
    bool isSocket_      : 1;
    bool sysNonblock_   : 1;
    bool userNonblock_  : 1;
    uint64_t recvTimeout_;
    uint64_t sendTimeout_;
};

class FdManager
{
public:
    FdManager();
    ~FdManager();
    std::shared_ptr<Fd> get(int fd, bool autoCreate = true);

    void del(int fd);
private:
    std::vector<std::shared_ptr<Fd>> fds_;
    std::unique_ptr<Mutex> mutex_;
};

typedef SingletonPtr<FdManager> FdMgr;

}   //namespace lithe