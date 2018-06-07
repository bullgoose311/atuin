#pragma once

#include <stdint.h>

// TODO: Need utility for grabbing the number of millis since the app started

inline uint16_t ByteSwap2(uint16_t data)
{
	return (data >> 8) | (data << 8);
}

inline uint32_t ByteSwap4(uint32_t data)
{
	return 
		((data >> 24)	& 0x000000ff) |
		((data >> 8)	& 0x0000ff00) |
		((data << 8)	& 0x00ff0000) |
		((data << 24)	& 0xff000000);
}