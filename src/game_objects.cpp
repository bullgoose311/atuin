#include "game_objects.h"

#include "entity_registry.h"
#include "math_utils.h"

void RoboCat::ProcessInput(float deltaTime, const InputState& inputState)
{

}

void RoboCat::SimulateMovement(float deltaTime)
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

void MouseStatus::Serialize(BitStream* bitStream, DirtyPropertyMask_t dirtyMask)
{
	if ((dirtyMask & MSP_NAME) != 0)
	{
		bitStream->Serialize(m_name);
	}

	if ((dirtyMask & MSP_LEG_COUNT) != 0)
	{
		bitStream->Serialize(m_legCount);
	}

	if ((dirtyMask & MSP_HEAD_COUNT) != 0)
	{
		bitStream->Serialize(m_headCount);
	}

	if ((dirtyMask & MSP_HEALTH) != 0)
	{
		bitStream->Serialize(m_health);
	}
}

void MouseStatus::Write(OutputMemoryBitStream& outputStream, DirtyPropertyMask_t dirtyMask)
{
	outputStream.Write(dirtyMask, GetRequiredBits<MSP_MAX>::Value);
	
	Serialize(&outputStream, dirtyMask);
}

void MouseStatus::Read(InputMemoryBitStream& inputStream)
{
	DirtyPropertyMask_t dirtyMask;
	inputStream.Read(dirtyMask, GetRequiredBits<MSP_MAX>::Value);

	Serialize(&inputStream, dirtyMask);
}

bool Entities_Init()
{
	EntityFactory::Get().RegisterCreationFunction<RoboCat>();

	return true;
}