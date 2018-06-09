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

void RoboCat::Serialize(MemoryStream* stream)
{
	stream->Serialize(m_health);
	stream->Serialize(m_meowCount);
	stream->Serialize(m_homeBase);
	stream->Serialize(m_name);
	stream->Serialize(m_miceIndices);
	stream->Serialize(m_position);
}