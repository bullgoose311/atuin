#pragma once

#include "input.h"
#include "network_manager_shared.h"
#include "replication_manager_server.h"

#include <stdint.h>
#include <unordered_map>

typedef uint32_t playerId_t;

class ClientProxy
{
public:
	ClientProxy(const SocketAddress& socketAddress, const std::string& name, playerId_t playerId)
		: m_address(socketAddress), m_name(name), m_playerId(playerId), m_bLastMoveTimestampDirty(false), m_lastPacketReceivedTimestamp(0)
	{}

	MoveList& GetUnprocessedMoveList() { return m_unprocessedMoveList; }

	void UpdateLastPacketTimestamp();

	void MarkLastMoveTimestampDirty() { m_bLastMoveTimestampDirty = true; }
	void MarkLastMoveTimestampNotDirty() { m_bLastMoveTimestampDirty = false; }
	bool IsLastMoveTimestampDirty() { return m_bLastMoveTimestampDirty; }

private:
	friend class NetworkManagerServer;

	ReplicationManagerServer m_replicationManager;

	SocketAddress m_address;
	std::string m_name;
	playerId_t m_playerId;
	MoveList m_unprocessedMoveList;
	bool m_bLastMoveTimestampDirty;
	float m_lastPacketReceivedTimestamp;
};

typedef std::shared_ptr<ClientProxy> ClientProxyPtr;

class NetworkManagerServer : public NetworkManager
{
public:
	static NetworkManagerServer& Get()
	{
		static NetworkManagerServer s_networkManager;
		return s_networkManager;
	}

	ClientProxyPtr GetClientProxy(playerId_t playerId);

	void ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr);

	void WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& packet, ClientProxyPtr clientProxy);

private:
	NetworkManagerServer() {}

	std::unordered_map<SocketAddress, ClientProxy*> m_clientMap;

	void HandlePacketFromNewClient(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr);
	
	void ProcessClientPacket(ClientProxy* clientProxy, InputMemoryBitStream& inputStream);
	void HandleInputPacket(ClientProxy* clientProxy, InputMemoryBitStream& packet);

	void SendWelcomePacket(const ClientProxy* clientProxy);
};