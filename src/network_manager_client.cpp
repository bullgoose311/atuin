#include "network_manager_client.h"

#include "input.h"
#include "log.h"
#include "network_manager_shared.h"
#include "timing_utils.h"

static const uint32_t kHelloInterval = 5000;

void NetworkManagerClient::TickOutgoingPackets()
{
	switch (m_state)
	{
		case NCS_SAYING_HELLO:
		{
			TickSayingHelloState();
			break;
		}

		case NCS_WELCOMED:
		{
			TickWelcomedState();
			break;
		}
	}
}

void NetworkManagerClient::ProcessPacket(InputMemoryBitStream& inputStream, const SocketAddress& fromAddr)
{
	uint32_t packetType;
	inputStream.Read(packetType);
	switch (packetType)
	{
		case kWelcomeCC:
		{
			HandleWelcomePacket(inputStream);
			break;
		}

		case kWorldStateCC:
		{
			HandleWorldStatePacket(inputStream);
			break;
		}
	}
}

void NetworkManagerClient::TickSayingHelloState()
{
	float time = Timing::Get().GetTimef();

	if (time > m_lastHelloTimestamp + kHelloInterval)
	{
		SendHelloPacket();
		m_lastHelloTimestamp = time;
	}
}

void NetworkManagerClient::TickWelcomedState()
{
	// TODO: Send inputs...
}

void NetworkManagerClient::HandleWelcomePacket(InputMemoryBitStream& inputStream)
{
	if (m_state != NCS_SAYING_HELLO)
	{
		return;
	}

	int playerId;
	inputStream.Read(playerId);
	m_playerId = playerId;
	m_state = NCS_WELCOMED;
	Log_Info(LOG_LABEL_NETWORK_CLIENT, "we've been welcomed with player id %d", m_playerId);
}

void NetworkManagerClient::HandleWorldStatePacket(InputMemoryBitStream& inputStream)
{
	// TODO: Verify that our simulation is in sync with the server's simulation
}

void NetworkManagerClient::SendHelloPacket()
{
	OutputMemoryBitStream helloPacket;
	helloPacket.Write(kHelloCC);
	helloPacket.Write(m_name);

	SendPacket(helloPacket, m_serverAddress);
}

void NetworkManagerClient::SendInputPacket()
{
	MoveList& moveList = InputManager::Get().GetMoveList();
	if (!moveList.HasMoves())
	{
		return;
	}

	OutputMemoryBitStream inputPacket;
	inputPacket.Write(kInputCC);

	// TODO: How many moves do we want to send per packet?  Right now we're sending 3, but that's arbitrary...
	int moveCount = static_cast<int>(moveList.GetMoveCount());
	int startIndex = moveCount > 3 ? moveCount - 3 - 1 : 0;
	inputPacket.Write(moveCount - startIndex, 2);
	for (int i = startIndex; i < moveCount; ++i)
	{
		moveList[i].Write(inputPacket);
	}

	SendPacket(inputPacket, m_serverAddress);
	moveList.Clear();
}