#include "network_manager_server.h"

#include "log.h"
#include "network_manager_shared.h"
#include "timing_utils.h"

void ClientProxy::UpdateLastPacketTimestamp()
{
	m_lastPacketReceivedTimestamp = Timing::Get().GetTimef();
}

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

void NetworkManagerServer::WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& packet, ClientProxyPtr clientProxy)
{
	bool bLastMoveTimestampDirty = clientProxy->IsLastMoveTimestampDirty();
	packet.Write(bLastMoveTimestampDirty);
	if (bLastMoveTimestampDirty)
	{
		packet.Write(clientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp());
		clientProxy->MarkLastMoveTimestampNotDirty();
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

void NetworkManagerServer::ProcessClientPacket(ClientProxy* clientProxy, InputMemoryBitStream& inputStream)
{
	//remember we got a packet so we know not to disconnect for a bit
	clientProxy->UpdateLastPacketTimestamp();

	uint32_t packetType;
	inputStream.Read(packetType);
	switch (packetType)
	{
	case kHelloCC:
		//need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
		//otherwise something weird is going on...
		SendWelcomePacket(clientProxy);
		break;
	case kInputCC:
		HandleInputPacket(clientProxy, inputStream);
		break;
	default:
	// 	LOG("Unknown packet type received from %s", inClientProxy->GetSocketAddress().ToString().c_str());
		break;
	}
}

void NetworkManagerServer::HandleInputPacket(ClientProxy* clientProxy, InputMemoryBitStream& packet)
{
	uint32_t moveCount = 0;
	packet.Read(moveCount, 2);
	Move move;
	for (; moveCount > 0; --moveCount)
	{
		if (move.Read(packet))
		{
			if (clientProxy->GetUnprocessedMoveList().AddMoveIfNew(move))
			{
				clientProxy->MarkLastMoveTimestampDirty();
			}
		}
	}
}

void NetworkManagerServer::SendWelcomePacket(const ClientProxy* clientProxy)
{
	OutputMemoryBitStream welcomePacket;
	welcomePacket.Write(kWelcomeCC);
	welcomePacket.Write(clientProxy->m_playerId);

	SendPacket(welcomePacket, clientProxy->m_address);
}