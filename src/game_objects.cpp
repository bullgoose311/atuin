#include "game_objects.h"

#include "array_utils.h"
#include "log.h"

// TODO: What's the best way to handle stuff like this?
LinkingContext g_linkingContext;

ObjectNetworkId LinkingContext::GetNetworkId(const GameObject* gameObject)
{
	if (!gameObject)
	{
		return INVALID_OBJECT_NETWORK_ID;
	}

	auto it = m_objectToIdMap.find(gameObject);
	if (it == m_objectToIdMap.end())
	{
		return INVALID_OBJECT_NETWORK_ID;
	}

	return it->second;
}

GameObject* LinkingContext::GetGameObject(ObjectNetworkId networkId)
{
	auto it = m_idToObjectMap.find(networkId);
	if (it != m_idToObjectMap.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

void RoboCat::Tick()
{

}

void RoboCat::Serialize(OutputMemoryStream& stream) const
{
	stream.Write(m_health);
	stream.Write(m_meowCount);
	stream.Write((const GameObject*)m_homeBase);
	uint8_t nameLength = static_cast<uint8_t>(strlen(m_name));
	stream.Write(nameLength);
	stream.Write(m_name, nameLength);
	stream.Write(m_miceIndices);
	stream.Write(m_position);
}

void RoboCat::Deserialize(InputMemoryStream& stream)
{
	stream.Read(m_health);
	stream.Read(m_meowCount);
	stream.Read(m_homeBase);
	uint8_t nameLength;
	stream.Read(nameLength);
	stream.Read(m_name, nameLength);
	stream.Read(m_miceIndices);
	stream.Read(m_position);
}

void Game_SendRoboCat(int socket, const RoboCat* roboCat)
{
	OutputMemoryStream stream;
	roboCat->Serialize(stream);
	// send(socket, stream.GetBufferPtr(), stream.GetLength(), 0);
}