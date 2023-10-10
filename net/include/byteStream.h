#pragma once

#include "common/fixedBuffer.h"

#include "common/endian.h"
#include "serializable.h"

#include <string>
#include <list>

namespace lithe
{


class ByteStream
{
public:
    friend class Buffer;
    typedef FixedBuffer<SmallFile> Buffer;
    // template<int SIZE>
    struct Node
    {
        Node() : 
            ptr(new Buffer()),
            next(nullptr)
        {
        }
        ~Node()
        {
            if(ptr)
            {
                delete ptr;
            }
        }
        // FixedBuffer<SIZE>* ptr;
        Buffer* ptr;
        Node* next;
    };  
    ByteStream() :
        size_(0),
        w_pos_(0),
        r_pos_(0),
        capacity_(SmallFile),
        baseSize_(SmallFile),
        root_(new Node()),
        w_cur_(root_),
        r_cur_(root_),
        endian_(LITHE_BIG_ENDING)
    {
    }
    ~ByteStream();

    void serialize(Serializable& value);
    void deserialize(Serializable& value);

    void writeUuint32(uint32_t value);
    void writeUuint64(uint64_t value);
    void writeUint32(int32_t value);
    void writeUint64(int64_t value);

    void writeFuint8(uint8_t value);
    void writeFuint16(uint16_t value);
    void writeFuint32(uint32_t value);
    void writeFuint64(uint64_t value);
    void writeFint8(int8_t value);
    void writeFint16(int16_t value);
    void writeFint32(int32_t value);
    void writeFint64(int64_t value);

    void writeFloat(float value);
    void writeDouble(double value);

    void writeStringF16(std::string& value);
    void writeStringF32(std::string& value);
    void writeStringF64(std::string& value);
    void writeStringVint(std::string& value);

    uint32_t readUuint32();
    uint64_t readUuint64();
    int32_t  readUint32();
    int64_t  readUint64();

    uint8_t readFuint8();
    uint16_t readFuint16();
    uint32_t readFuint32();
    uint64_t readFuint64();
    int8_t readFint8();
    int16_t readFint16();
    int32_t readFint32();
    int64_t readFint64();

    float readFloat();
    double readDouble();

    std::string readStringF16();
    std::string readStringF32();
    std::string readStringF64();
    std::string readStringVint(); 
    
    void write7bitEncoded32(uint32_t value);
    void write7bitEncoded64(uint64_t value);
    void read7bitEncoded32(uint32_t* value);
    void read7bitEncoded64(uint64_t* value);
    
    uint64_t encodeZigZag64(int64_t& value);
    int64_t decodeZigZag64(uint64_t& value);

    uint32_t encodeZigZag32(int32_t& value);
    int32_t decodeZigZag32(uint32_t& value);

    void addCapacity(size_t size);
    size_t getCapacity();

    ssize_t readableSize() const;
    ssize_t writableSize() const;

    void reset();


private:
    void write(const void* value, size_t len);
    size_t read(void* value, size_t len);
    
    size_t size_;
    size_t w_pos_;  //写下标 只会增加
    size_t r_pos_;  //读下标 只会增加
    size_t capacity_;
    size_t baseSize_;
    Node* root_;
    Node* w_cur_;
    Node* r_cur_;
    int endian_;
};


} // namespace lithe

// template <typename T>
// inline void BinaryStream::write7bitEncoded(T value, std::string &buf)
// {
//     do
//     {
//         unsigned char c = (unsigned char)value & 0x7F;
//         value = value >> 7;
//         if(value)
//             c |= 0x80;
//         buf.append(1, c);
//     } while (value);
// }

// template <typename T>
// inline void BinaryStream::read7bitEncoded(T& value, std::string &buf)
// {   
//     value = 0;
//     char* str = (char*)buf.c_str();
//     char c = '\0';
//     int bitCount = 0;
//     do
//     {
//         c = *str++;
//         T x = (c & 0x7F);
//         x <<= bitCount;
//         value += x;
//         bitCount += 7;
//     } while ((*str != '\0') && (c & 0x80));
// }

