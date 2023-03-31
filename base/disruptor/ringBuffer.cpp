#include "ringBuffer.h"

namespace lithe
{

template <typename T, int SIZE>
inline RingBuffer<T, SIZE>::RingBuffer() : 
                                writeIndex_(0),
                                readIndex_(0)
{
}

template <typename T, int SIZE>
T RingBuffer<T, SIZE>::take()
{   
    
    return T();
}

template <typename T, int SIZE>
void RingBuffer<T, SIZE>::put(T &elem)
{
    data_[writeIndex_++];
}

}   //namespace lithe