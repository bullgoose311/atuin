#include "network_manager_server.h"

#include "log.h"
#include "network_manager_shared.h"

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

	// TODO: Create client proxy...
	ClientProxy clientProxy;

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