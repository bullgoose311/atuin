#include "entity_registry.h"

GameObject* EntityFactory::CreateEntity(EntityClassId_t classId)
{
	// We only know how to create the object, we do not manage it's lifetime
	EntityCreationFunc creationFunc = m_classIdToCreateFuncMap[classId];
	return creationFunc();
}