#include "entity_registry.h"

GameObject* EntityRegistry::CreateEntity(EntityClassId_t classId)
{
	// TODO: Why raw pointers?  Who's in charge of entity object life time?
	EntityCreationFunc creationFunc = m_classIdToCreateFuncMap[classId];
	return creationFunc();
}