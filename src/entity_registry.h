#pragma once

#include "game_objects.h"

#include <cassert>
#include <unordered_map>

class EntityRegistry
{
public:
	static EntityRegistry& Get()
	{
		static EntityRegistry s_instance;
		return s_instance;
	}

	template<class T> void RegisterCreationFunction();
	GameObject* CreateEntity(EntityClassId_t classId);

private:
	std::unordered_map<EntityClassId_t, EntityCreationFunc> m_classIdToCreateFuncMap;
};

template<class T> void EntityRegistry::RegisterCreationFunction()
{
	assert(m_classIdToCreateFuncMap.find(T::kEntityClassId) == m_classIdToCreateFuncMap.end());

	m_classIdToCreateFuncMap[T::kEntityClassId] = T::CreateInstance;
}