#include "linking_context.h"

EntityNetworkId_t LinkingContext::GetNetworkId(const GameObject* gameObject, bool bCreateIfNeeded)
{
	if (!gameObject)
	{
		return INVALID_ENTITY_NETWORK_ID;
	}

	auto it = m_objectToIdMap.find(gameObject);
	if (it != m_objectToIdMap.end())
	{
		return it->second;
	}
	else if (bCreateIfNeeded)
	{
		EntityNetworkId_t networkId = m_nextNetworkId++;
		AddGameObject((GameObject*)gameObject, networkId);
		return networkId;
	}
	else
	{
		return INVALID_ENTITY_NETWORK_ID;
	}
}

void LinkingContext::AddGameObject(const GameObject* gameObject, EntityNetworkId_t networkId)
{
	m_idToObjectMap[networkId] = (GameObject*)gameObject;
	m_objectToIdMap[gameObject] = networkId;
}

void LinkingContext::RemoveGameObject(const GameObject* gameObject)
{
	EntityNetworkId_t networkId = m_objectToIdMap[gameObject];
	m_objectToIdMap.erase(gameObject);
	m_idToObjectMap.erase(networkId);
}

GameObject* LinkingContext::GetGameObject(EntityNetworkId_t networkId)
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