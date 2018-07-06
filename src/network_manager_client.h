#pragma once

#include "network_manager_shared.h"

#include <string>

enum NetworkClientState
{
	NCS_UNINITIALIZED,
	NCS_SAYING_HELLO,
	NCS_WELCOMED
};

class NetworkManagerClient : public NetworkManager
{
public:
	NetworkManagerClient() : 
		m_state(NCS_UNINITIALIZED),
		m_lastHelloTimestamp(0) 
		{}

	void TickOutgoingPackets();
	void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr);

private:
	NetworkClientState m_state;
	float m_lastHelloTimestamp;
	std::string m_name;
	SocketAddress m_serverAddress;
	int m_playerId;

	void TickSayingHelloState();
	void TickWelcomedState();

	void HandleWelcomePacket(InputMemoryBitStream& inputStream);
	void HandleWorldStatePacket(InputMemoryBitStream& inputStream);

	void SendHelloPacket();
	void SendInputPacket();
};