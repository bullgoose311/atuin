#pragma once

#include "bit_streams.h"
#include "socket_address.h"

#include <stdint.h>

static const uint32_t kHelloCC =		'HELO';
static const uint32_t kWelcomeCC =		'WLCM';
static const uint32_t kWorldStateCC =	'WST8';
static const uint32_t kInputCC =		'INPT';

class NetworkManager
{
public:
	void SendPacket(OutputMemoryBitStream& packet, const SocketAddress& toAddr);
};