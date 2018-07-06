#pragma once

#include "network_manager_shared.h"

#include <stdint.h>
#include <unordered_map>

typedef uint32_t playerId_t;

struct ClientProxy
{
	playerId_t m_playerId;
	SocketAddress m_address;
};

class NetworkManagerServer : public NetworkManager
{
public:
	void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr);

private:
	std::unordered_map<SocketAddress, ClientProxy*> m_clientMap;

	void HandlePacketFromNewClient(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr);
	void ProcessClientPacket(const ClientProxy* clientProxy, InputMemoryBitStream& inputStream);

	void SendWelcomePacket(const ClientProxy* clientProxy);
};