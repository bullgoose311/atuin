#pragma once

#include <stdint.h>

inline uint32_t ConvertToFixed(float number, float min, float precision)
{
	return static_cast<uint32_t>((number - min) / precision);
}

inline float ConvertFromFixed(uint32_t number, float min, float precision)
{
	return (static_cast<float>(number) * precision) + min;
}

template< int tValue, int tBits >
struct GetRequiredBitsHelper
{
	enum { Value = GetRequiredBitsHelper< (tValue >> 1), tBits + 1 >::Value };
};

template< int tBits >
struct GetRequiredBitsHelper< 0, tBits >
{
	enum { Value = tBits };
};

template< int tValue >
struct GetRequiredBits
{
	enum { Value = GetRequiredBitsHelper< tValue, 0 >::Value };
};