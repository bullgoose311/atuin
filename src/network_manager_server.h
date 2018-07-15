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
		: m_address(socketAddress), m_name(name), m_playerId(playerId), m_bLastMoveTimestampDirty(false)
	{}

	MoveList& GetUnprocessedMoveList() { return m_unprocessedMoveList; }

private:
	friend class NetworkManagerServer;

	ReplicationManagerServer m_replicationManager;

	SocketAddress m_address;
	std::string m_name;
	playerId_t m_playerId;
	MoveList m_unprocessedMoveList;
	bool m_bLastMoveTimestampDirty;
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

private:
	NetworkManagerServer() {}

	std::unordered_map<SocketAddress, ClientProxy*> m_clientMap;

	void HandlePacketFromNewClient(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr);
	void ProcessClientPacket(const ClientProxy* clientProxy, InputMemoryBitStream& inputStream);

	void SendWelcomePacket(const ClientProxy* clientProxy);
};