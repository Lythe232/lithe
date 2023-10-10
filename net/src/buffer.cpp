#include "include/buffer.h"
#include "include/hookSysCall.h"
#include "include/log/log.h"

#include <string.h>

#include <unistd.h>
#include <fcntl.h>

namespace lithe
{
static auto g_logger = LOG_ROOT();
void Buffer::append(const char *data, size_t len)
{
    stream_.write(static_cast<const void*>(data), len);
}
void Buffer::append(std::string& data)
{
    append(data.c_str(), data.size());
}

void Buffer::append(std::shared_ptr<Buffer> buffer) //TODO 效率很低
{
    assert(buffer);
    size_t rsize = buffer->readableSize();
    char* buf = new char[rsize];
    buffer->fetch(buf, rsize);
    this->append(buf, rsize);
    delete [] buf;
    return ;
}

void Buffer::append(Buffer& buffer) //TODO  
{
    size_t rsize = buffer.readableSize();
    char* buf = new char[rsize];
    buffer.fetch(buf, rsize);
    this->append(buf, rsize);
    delete [] buf;
    return ;
}


ssize_t Buffer::fetch(char* data, size_t len)
{
    return stream_.read(static_cast<void*>(data), len);
}

std::string Buffer::fetchAllAsString()
{
    size_t len = stream_.readableSize();
    char data[len + 1];
    fetch(data, len);
    data[len] = '\0';
    std::string dataString(data);
    return dataString;
}

void Buffer::clear()
{

}

ssize_t Buffer::readFd(int fd, int* savedError)
{
    char buf[65536];
    ssize_t n = 0;
    ssize_t ar = 0;

    while(true)
    {
        memset(buf, 0, 65536);
        n = ::read(fd, buf, 65536);
        if(n <= 0)
        {
            if(n == -1 && errno == EAGAIN)
            {
                return ar;
            }
            *savedError = errno;
            return n;
        }
        append(buf, n);
        ar += n;
        continue;
    }
    return ar;
}

void Buffer::writeInt64(int64_t value)
{
    stream_.writeFint64(value);
}
void Buffer::writeInt32(int32_t value)
{
    stream_.writeFint32(value);
}
void Buffer::writeInt16(int16_t value)
{
    stream_.writeFint16(value);
}
void Buffer::writeInt8(int8_t value)
{
    stream_.writeFint8(value);
}

int64_t Buffer::readInt64()
{
    return stream_.readFint64();
}
int32_t Buffer::readInt32()
{
    return stream_.readFint32();
}
int16_t Buffer::readInt16()
{
    return stream_.readFint16();
}
int8_t Buffer::readInt8()
{
    return stream_.readFint8();
}

ssize_t Buffer::readableSize()
{
    return stream_.readableSize();
}

ssize_t Buffer::writableSize()
{
    return stream_.writableSize();
}

ssize_t Buffer::sendto(int fd, int* err)
{
    ssize_t rs = readableSize();
    char* data = new char[rs];   ////FIXME
    char* buf = data;
    stream_.read(buf, rs);

    ssize_t aw = 0;
    ssize_t nw = rs;
    while(aw < rs)
    {
        int n = ::write(fd, buf, nw);
        if(n <= 0)
        {
            if(n == 0)
            {
                break;
            }
            *err = errno;
            delete [] data;
            return n;
        }
        aw += n;
        nw -= n;
        buf += n;
    }

    delete [] data;
    return aw;
}

ssize_t Buffer::recvfrom(int fd, int count, int* err)
{
    char* buf = new char[count];
    memset(buf, 0, count);
    int n = ::read(fd, buf, count);
    if(n <= 0)
    {
        if(n == 0)
        {
            delete [] buf;
            return 0;
        }
        *err = errno;
        delete [] buf;
        return -1;
    }
    stream_.write(buf, n);
    delete [] buf;
    return n;
}

}   //lithe
