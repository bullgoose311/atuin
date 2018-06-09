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