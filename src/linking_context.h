#pragma once

#include "game_objects.h"

#include <stdint.h>
#include <unordered_map>

class LinkingContext
{
public:
	static LinkingContext& Get()
	{
		static LinkingContext s_instance;
		return s_instance;
	}

	EntityNetworkId_t GetNetworkId(const GameObject* gameObject, bool bCreateIfNeeded);
	GameObject* GetGameObject(EntityNetworkId_t networkId);
	void AddGameObject(const GameObject* gameObject, EntityNetworkId_t networkId);
	void RemoveGameObject(const GameObject* gameObject);

private:
	EntityNetworkId_t m_nextNetworkId;
	std::unordered_map<EntityNetworkId_t, GameObject*> m_idToObjectMap;
	std::unordered_map<const GameObject*, EntityNetworkId_t> m_objectToIdMap;

	LinkingContext() : m_nextNetworkId(0) {}
};