#pragma once

#include <stdint.h>

typedef uint32_t EntityNetworkId_t;

enum PacketType
{
	PT_HELLO,
	PT_REPLICATION_DATA,
	PT_DISCONNECT,
	PT_MAX
};