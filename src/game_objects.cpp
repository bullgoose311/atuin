#include "game_objects.h"

#include "entity_registry.h"

void RoboCat::Serialize(MemoryStream* stream)
{
	stream->Serialize(m_health);
	stream->Serialize(m_meowCount);
	stream->Serialize(m_homeBase);
	stream->Serialize(m_name);
	stream->Serialize(m_miceIndices);
	stream->Serialize(m_position);
}

void Entities_Init()
{
	EntityRegistry::Get().RegisterCreationFunction<RoboCat>();
}