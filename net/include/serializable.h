#pragma once

#include "byteStream.h"
namespace lithe
{
class ByteStream;
class Serializable
{
public:
    virtual void serialize(ByteStream& data) = 0;
    virtual void deserialize(ByteStream& data) = 0;
};

#define SERIALIZE(...)

#define SERIALIZE_MEMBER()



#define DESERIALIZE_MEMBER()

}   //namespace lithe
