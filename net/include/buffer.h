#pragma once

#include "common/endian.h"
#include "byteStream.h"

#include <memory>
#include <vector>
#include <string>

namespace lithe
{
class Buffer
{
public:
    Buffer() 
    {
    }
    void append(const char* data, size_t len);
    void append(std::string& data);
    void append(std::shared_ptr<Buffer> buffer);
    void append(Buffer& buffer);
    ssize_t fetch(char* data, size_t len);
    std::string fetchAllAsString();
    void retrieve(size_t len);

    void clear();
    ssize_t readFd(int fd, int* savedError);

    void writeInt64(int64_t value);
    void writeInt32(int32_t value);
    void writeInt16(int16_t value);
    void writeInt8 (int8_t  value);

    int64_t readInt64();
    int32_t readInt32();
    int16_t readInt16();
    int8_t  readInt8();

    size_t readableSize();
    size_t writableSize();

    ssize_t sendto(int fd, int* err);
    ssize_t recvfrom(int fd, int count, int* err);

private:
    ByteStream stream_;
};

} // namespace lithe