#pragma once

#include <byteswap.h>
#include <stdint.h>
#include <type_traits>

#ifdef __ORDER_LITTLE_ENDIAN__ 
#define LITHE_ORDER __ORDER_LITTLE_ENDIAN__

#else
#define LITHE_ORDER __ORDER_BIG_ENDIAN__

#endif

#define LITHE_BIG_ENDING __BIG_ENDIAN
#define LITHE_LITTLE_ENDING __LITTLE_ENDIAN

namespace lithe
{

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value)
{
    return (T)bswap_64((uint64_t)value);
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value)
{
    return (T)bswap_32((uint32_t)value);
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value)
{
    return (T)bswap_16((uint16_t)value);
}


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

template<typename T>
T byteswapOnLitteEndian(T value)
{
    return byteswap(value);
}
template<typename T>
T byteswapOnBigEndian(T value)
{
    return value;
}
#else

template<typename T>
T byteswapOnLitteEndian(T value)
{
    return value;
}
template<typename T>
T byteswapOnBigEndian(T value)
{
    return byteswap(value);
}
#endif 
}