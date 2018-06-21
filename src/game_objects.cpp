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

void RoboCat::Write(OutputMemoryBitStream& outputStream)
{
	outputStream.Write(m_health);
	// outputStream.Write(m_position);
}

void RoboCat::Read(InputMemoryBitStream& inputStream)
{
	// This may never end up getting called?
	// What happen to bi-directional serialization so that the caller didn't need to know?
	//inputStream.Read(m_health);
	//inputStream.Read(m_position);
}

void Entities_Init()
{
	EntityFactory::Get().RegisterCreationFunction<RoboCat>();
}