#include "include/byteStream.h"
#include "include/log/log.h"


#include <assert.h>
#include <vector>
#include <math.h>
namespace lithe
{
static auto g_logger = LOG_ROOT();


ByteStream::~ByteStream()
{
    if(readableSize() > 0)
    {
        LOG_WARN(g_logger) << "There is still data unread readablesize = " << readableSize();
    }
    if(root_)
    {
        Node* current = root_;
        while(current)
        {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
}
uint64_t ByteStream::encodeZigZag64(int64_t &value)
{
    return (uint64_t)((value << 1) ^ (value >> 63));
}

int64_t ByteStream::decodeZigZag64(uint64_t& value)
{
    return (int64_t)(value >> 1) ^ -(value & 1);
}

uint32_t ByteStream::encodeZigZag32(int32_t& value)
{
    return (uint32_t)((value << 1) ^ (value >> 31));
}

int32_t ByteStream::decodeZigZag32(uint32_t& value)
{
    return (int32_t)(value >> 1) ^ -(value & 1);
}

ssize_t ByteStream::readableSize() const
{
    ssize_t size = (w_pos_ - r_pos_);
    assert(size >= 0);
    return size;
    // return (w_pos_ - r_pos_) > 0 ? (w_pos_ - r_pos_) : 0;
}
ssize_t ByteStream::writableSize() const
{
    size_t size = capacity_ - w_pos_;
    assert(size >= 0);
    return size;
}
void ByteStream::addCapacity(size_t size)
{
    if(size == 0)
    {
        return;
    }
    size_t old_cap = getCapacity();
    if(size < old_cap)
    {
        return;
    }

    size = size - old_cap;

    //////////////////////
    if(size == 0)   
    {
        size = 1;   
    }
    //////////////////////

    int count = ceil(1.0 * size / baseSize_);
    Node* first = nullptr;
    Node* tmp = w_cur_;
    while(tmp->next)
    {
        tmp = tmp->next;
    }
    for(int i = 0; i < count; i++)
    {
        tmp->next = new Node();
        if(first == nullptr)
        {
            first = tmp->next;
        }
        capacity_ += baseSize_;
        tmp = tmp->next;
    }

    if(old_cap == 0)
    {
        w_cur_ = first;
    }
}

size_t ByteStream::getCapacity()
{
    return size_t(capacity_ - w_pos_);
}

void ByteStream::write(const void *value, size_t len)
{
    if(len == 0)
        return ;

    addCapacity(len);
    size_t nw = len;    //nw = not write
    size_t aw = 0;      //aw = already write
    size_t cur_pos = w_pos_ % baseSize_;
    size_t n = baseSize_ - cur_pos; 

    while(nw > 0)
    {
        if(nw <= n)
        {
            // memcpy(, value, nw);
            int f1 = w_cur_->ptr->append(((const char*)value + aw), nw);
            assert(f1);
            if(cur_pos + nw == baseSize_)
            {
                w_cur_ = w_cur_->next;
            }
            w_pos_ += nw;
            aw += nw;
            nw = 0;
        }
        else
        {
            // memcpy(cur_, value, n);
            int f2 = w_cur_->ptr->append(((const char*)value + aw), n);
            assert(f2);
            w_pos_ += n;
            aw += n;
            nw -= n;
            w_cur_ = w_cur_->next;
            cur_pos = 0;
            n = baseSize_;
        }
    }
    assert(nw == 0);

    // char buf[readableSize()];
    // size_t rs = read(buf, readableSize());
    // std::string rss(buf, rs);
    //     printf("(2)--------------ByteStream::write data = %s, size = %d, w_pos = %d, r_pos = %d\n", rss.c_str(), rs, w_pos_, r_pos_);
    
    if(size_ < w_pos_)
    {
        size_ = w_pos_;
    }
}

size_t ByteStream::read(void *value, size_t len)
{
    if(len == 0)
    {
        return 0;
    }
    if(readableSize() < len)
    {
        return 0;
    }
    size_t nr = len;
    size_t ar = 0;
    size_t cur_pos = r_pos_ % baseSize_;
    size_t cur_size = baseSize_ - cur_pos;  //当前节点可读大小
    std::vector<Node*> nodes;

    while(nr > 0)
    {
        if(nr <= cur_size)
        {
            size_t read_size = r_cur_->ptr->fetch((char*)value, nr);
            assert(nr == read_size);
            if(cur_pos + read_size == baseSize_)
            {
                nodes.push_back(r_cur_);
                r_cur_ = r_cur_->next;
            }
            r_pos_ += read_size;
            ar += read_size;
            cur_pos += read_size;
            nr = 0;
        }   
        else
        {
            size_t read_size = r_cur_->ptr->fetch((char*)value, cur_size);
            // assert(nr == cur_size);
            // r_pos_ += cur_size;
            r_pos_ += read_size;
            ar += read_size;
            nr -= read_size;
            value += read_size;
            nodes.push_back(r_cur_);
            r_cur_ = r_cur_->next;
            cur_size = baseSize_; 
            cur_pos = 0;
        }
    }
    assert(nr == 0);
    if(!nodes.empty())
    {
        for(auto i : nodes)
        {
            delete i;
        }
        root_ = r_cur_;
        assert(root_);
    }
    // printf("ByteStream::read value = %s, len = %ld\n", value, len);

    // printf("w_pos = %d, r_pos = %d\n\n", w_pos_, r_pos_);

    return ar;
}

//  encoding
//-1 ->  1111 1111          1->     0000 0001
//<< 1   1111 1110          << 1    0000 0010
//>> 7   1111 1111          >> 7    0000 0000
//^      0000 0001 -> 1     ^       0000 0010 -> 2


void ByteStream::serialize(Serializable &data)
{
    data.serialize(*this);
}

void ByteStream::deserialize(Serializable &data)
{
    data.deserialize(*this);
}

void lithe::ByteStream::writeUuint32(uint32_t value)
{
    write7bitEncoded32(value);
}

void ByteStream::writeUuint64(uint64_t value)
{
    write7bitEncoded64(value);
}

void ByteStream::writeUint32(int32_t value)
{
    write7bitEncoded32(encodeZigZag32(value));
}

void ByteStream::writeUint64(int64_t value)
{
    write7bitEncoded64(encodeZigZag64(value));
}

void ByteStream::writeFuint8(uint8_t value)
{
    write(&value, sizeof(value));
}

void ByteStream::writeFuint16(uint16_t value)
{
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }

    write(&value, sizeof(value));
}

void ByteStream::writeFuint32(uint32_t value)
{
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteStream::writeFuint64(uint64_t value)
{
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteStream::writeFint8(int8_t value)
{
    write(&value, sizeof(value));
}

void ByteStream::writeFint16(int16_t value)
{
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteStream::writeFint32(int32_t value)
{
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteStream::writeFint64(int64_t value)
{
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteStream::writeFloat(float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint32(v);
}

void ByteStream::writeDouble(double value)
{
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint64(v);
}

void ByteStream::writeStringF16(std::string &value)
{
    writeFuint16(value.size());
    write(value.c_str(), value.size());
}

void ByteStream::writeStringF32(std::string &value)
{
    writeFuint32(value.size());
    write(value.c_str(), value.size());
}

void ByteStream::writeStringF64(std::string &value)
{
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}

void ByteStream::writeStringVint(std::string &value)
{
    writeUuint64(value.size());
    write(value.c_str(), value.size());
}

uint32_t ByteStream::readUuint32()
{
    uint32_t value;
    read7bitEncoded32(&value);
    return value;
}

uint64_t ByteStream::readUuint64()
{
    uint64_t value;
    read7bitEncoded64(&value);
    return value;
}

int32_t ByteStream::readUint32()
{
    uint32_t value;
    read7bitEncoded32(&value);
    return decodeZigZag32(value);
}

int64_t ByteStream::readUint64()
{
    uint64_t value;
    read7bitEncoded64(&value);
    return decodeZigZag64(value);
}

uint8_t ByteStream::readFuint8()
{
    uint8_t value = 0;
    read(&value, sizeof(value));
    return value;
}

uint16_t ByteStream::readFuint16()
{
    uint16_t value;
    read(&value, sizeof(value));
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }

    return value;
}

uint32_t ByteStream::readFuint32()
{
    uint32_t value;
    read(&value, sizeof(value));
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    return value;
}

uint64_t ByteStream::readFuint64()
{
    uint64_t value;
    read(&value, sizeof(value));
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    return value;
}

int8_t ByteStream::readFint8()
{
    int8_t value;
    read(&value, sizeof(value));
    return value;
}

int16_t ByteStream::readFint16()
{
    int16_t value;
    read(&value, sizeof(value));
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    return value;
}

int32_t ByteStream::readFint32()
{
    int32_t value;
    read(&value, sizeof(value));
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    return value;
}

int64_t ByteStream::readFint64()
{
    int64_t value;
    read(&value, sizeof(value));
    if(endian_ != LITHE_ORDER)
    {
        value = byteswap(value);
    }
    return value;
}

float ByteStream::readFloat()
{
    uint32_t value = readFuint32();
    float fv;
    memcpy(&fv, &value, sizeof(value));
    return fv;
}

double ByteStream::readDouble()
{
    uint64_t value = readFuint64();
    double dv;
    memcpy(&dv, &value, sizeof(value));
    return dv;
}

std::string ByteStream::readStringF16()
{
    uint16_t len = readFuint16();
    std::string str;
    str.resize(len);
    read(&str[0], len);
    return str;
}

std::string ByteStream::readStringF32()
{
    uint32_t len = readFuint32();
    std::string str;
    str.resize(len);
    read(&str[0], len);
    return str;
}

std::string ByteStream::readStringF64()
{
    uint64_t len = readFuint64();
    std::string str;
    str.resize(len);
    read(&str[0], len);
    return str;
}


std::string ByteStream::readStringVint()
{
    uint64_t len = readUuint64();
    std::string str;
    str.resize(len);
    read(&str[0], len);
    return str;
}

void ByteStream::write7bitEncoded32(uint32_t value)
{
    do
    {
        uint8_t c = (uint8_t) value & 0x7f;
        value >>= 7;
        if(value)
            c |= 0x80;
        write(&c, 1);
    } while (value);
}

void ByteStream::write7bitEncoded64(uint64_t value)
{
    do
    {
        uint8_t c = (uint8_t) value & 0x7f;
        value >>= 7;
        if(value)
            c |= 0x80;
        write(&c, 1);
    } while (value);
}

void ByteStream::read7bitEncoded32(uint32_t *value)
{
    uint8_t c; 
    uint32_t ov;
    uint32_t v = 0;
    int offset = 0;
    int max = 5;
    do
    {
        c = readFuint8();
        ov = (c & 0x7f);
        ov <<= offset;
        v += ov;
        offset += 7;
        --max;
    } while ((c & 0x80) && (max > 0));
    memcpy(value, &v, sizeof(uint32_t));
}
//   0000 0010
// 1 0000 0000

void ByteStream::read7bitEncoded64(uint64_t *value)
{
    uint8_t c; 
    uint64_t ov;
    uint64_t v = 0;
    int offset = 0;
    int max = 10;
    do
    {
        c = readFuint8();
        ov = (c & 0x7f);
        ov <<= offset;
        v += ov;
        offset += 7;
        --max;
    } while ((c & 0x80) && (max > 0));
    memcpy(value, &v, sizeof(uint64_t));
}

void ByteStream::reset()
{
        // size_ = 0;
        // w_pos_ = 0;
        // r_pos_ = 0;
        // root_(new Node()),
        // w_cur_(root_),
        // r_cur_(root_),
}

} // namespace lithe
