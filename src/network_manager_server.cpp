#include "network_manager_server.h"

#include "log.h"
#include "network_manager_shared.h"

ClientProxyPtr NetworkManagerServer::GetClientProxy(playerId_t playerId)
{
	return nullptr;
}

void NetworkManagerServer::ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr)
{
	SocketAddress sockAddr;
	auto it = m_clientMap.find(fromAddr);
	if (it == m_clientMap.end())
	{
		HandlePacketFromNewClient(inputStream, fromAddr);
	}
	else
	{
		ProcessClientPacket((*it).second, inputStream);
	}
}

void NetworkManagerServer::HandlePacketFromNewClient(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr)
{
	uint32_t packetType;
	inputStream.Read(packetType);
	if (packetType != kHelloCC)
	{
		Log_Error(LOB_LABEL_NETWORK_SERVER, "expected hello packet but got %d", packetType);
		return;
	}

	std::string name;
	inputStream.Read(name);

	// TODO: Where do we store the client proxy?
	playerId_t playerId = 1;
	ClientProxy clientProxy(fromAddr, name, playerId);

	SendWelcomePacket(&clientProxy);

	// TODO: Init replication manager for this client...
}

void NetworkManagerServer::ProcessClientPacket(const ClientProxy* clientProxy, InputMemoryBitStream& inputStream)
{

}

void NetworkManagerServer::SendWelcomePacket(const ClientProxy* clientProxy)
{
	OutputMemoryBitStream welcomePacket;
	welcomePacket.Write(kWelcomeCC);
	welcomePacket.Write(clientProxy->m_playerId);

	SendPacket(welcomePacket, clientProxy->m_address);
}