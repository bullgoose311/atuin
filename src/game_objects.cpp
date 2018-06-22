#include "game_objects.h"

#include "entity_registry.h"
#include "math_utils.h"

void RoboCat::Serialize(MemoryStream* stream)
{
	stream->Serialize(m_health);
	stream->Serialize(m_meowCount);
	stream->Serialize(m_homeBase);
	stream->Serialize(m_name);
	stream->Serialize(m_miceIndices);
	stream->Serialize(m_position);
}

void RoboCat::Write(OutputMemoryBitStream& outputStream, DirtyPropertyMask_t dirtyMask)
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

void MouseStatus::Write(OutputMemoryBitStream& outputStream, DirtyPropertyMask_t dirtyMask)
{
	outputStream.Write(dirtyMask, GetRequiredBits<MSP_MAX>::Value);
	
	if ((dirtyMask & MSP_NAME) != 0)
	{
		outputStream.Write(m_name);
	}

	if ((dirtyMask & MSP_LEG_COUNT) != 0)
	{
		outputStream.Write(m_legCount);
	}

	if ((dirtyMask & MSP_HEAD_COUNT) != 0)
	{
		outputStream.Write(m_headCount);
	}

	if ((dirtyMask & MSP_HEALTH) != 0)
	{
		outputStream.Write(m_health);
	}
}

void MouseStatus::Read(InputMemoryBitStream& inputStream)
{
	DirtyPropertyMask_t dirtyMask;
	inputStream.Read(dirtyMask, GetRequiredBits<MSP_MAX>::Value);
	if ((dirtyMask & MSP_NAME) != 0)
	{
		inputStream.Read(m_name);
	}

	if ((dirtyMask & MSP_LEG_COUNT) != 0)
	{
		inputStream.Read(m_legCount);
	}

	if ((dirtyMask & MSP_HEAD_COUNT) != 0)
	{
		inputStream.Read(m_headCount);
	}

	if ((dirtyMask & MSP_HEALTH) != 0)
	{
		inputStream.Read(m_health);
	}
}

bool Entities_Init()
{
	EntityFactory::Get().RegisterCreationFunction<RoboCat>();

	return true;
}