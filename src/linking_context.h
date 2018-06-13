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

	ObjectNetworkId_t GetNetworkId(const GameObject* gameObject, bool bCreateIfNeeded);
	GameObject* GetGameObject(ObjectNetworkId_t networkId);
	void AddGameObject(const GameObject* gameObject, ObjectNetworkId_t networkId);
	void RemoveGameObject(const GameObject* gameObject);

private:
	ObjectNetworkId_t m_nextNetworkId;
	std::unordered_map<ObjectNetworkId_t, GameObject*> m_idToObjectMap;
	std::unordered_map<const GameObject*, ObjectNetworkId_t> m_objectToIdMap;

	LinkingContext() : m_nextNetworkId(0) {}
};