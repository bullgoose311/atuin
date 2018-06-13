#include "streams.h"

#include "game_objects.h"
#include "linking_context.h"
#include "log.h"
#include "math_utils.h"

#include <algorithm>
#include <memory>

// MemoryStream
void MemoryStream::Serialize(char* str)
{
	if (GetDirection() == STREAM_DIR_INPUT)
	{
		uint32_t size;
		Serialize(size);
		Serialize(str, size);
	}
	else
	{
		uint32_t size = static_cast<uint32_t>(strlen(str));
		Serialize(size);
		Serialize(str, size);
	}
}

/******************
OutputMemoryStream
******************/

OutputMemoryStream::OutputMemoryStream() :
	m_buffer(nullptr),
	m_head(0),
	m_capacity(0)
{
	ReallocBuffer(32);
}

OutputMemoryStream::~OutputMemoryStream() 
{ 
	Log_Info(LOG_LABEL_MEMORY, "output stream freed");
	std::free(m_buffer); 
}

void OutputMemoryStream::Write(const void* data, size_t byteCount)
{
	uint32_t resultHead = m_head + static_cast<uint32_t>(byteCount);
	if (resultHead > m_capacity)
	{
		ReallocBuffer(std::max(m_capacity * 2, resultHead));
	}

	std::memcpy(m_buffer + m_head, data, byteCount);

	m_head = resultHead;
}

void OutputMemoryStream::Write(const GameObject* gameObject)
{
	ObjectNetworkId_t networkId = LinkingContext::Get().GetNetworkId(gameObject, true);
	Write(networkId);
}

void OutputMemoryStream::Write(const Vector3& v3)
{
	// Game world is 4000 x 4000, centered at the origin.
	// X/Z min/max is -2000 to 2000
	// Client side positions only need to be accurate to within 0.1 game units
	// Total number of possible X values = ((max-min)/precision) + 1 = ((2000--2000)/0.1) + 1 = 40001
	// log 40001 base 2 = 15.3, so we need 16 bits to represent for X and Z components

	uint32_t compressedX = ConvertToFixed(v3.m_x, -2000.f, .1f);
	uint32_t compressedZ = ConvertToFixed(v3.m_z, -2000.f, .1f);
	Write(&compressedX, 2);
	Write(&compressedZ, 2);

	// TODO: We can compress Y better based on how often we think the object will be in the air
	Write(v3.m_y);
}

void OutputMemoryStream::ReallocBuffer(uint32_t newLength)
{
	Log_Info(LOG_LABEL_MEMORY, "reallocating buffer from size %d to size %d", m_capacity, newLength);

	m_buffer = static_cast<char*>(std::realloc(m_buffer, newLength));

	// TODO: Handler allocation failure...

	m_capacity = newLength;
}

void OutputMemoryStream::Serialize(void* ioData, uint32_t byteCount)
{
	Write(ioData, byteCount);
}

void OutputMemoryStream::Serialize(GameObject* gameObject)
{
	Write((const GameObject*)gameObject);
}

void OutputMemoryStream::Serialize(Vector3& v3)
{
	Write(v3);
}

/****************
InputMemoryStream
*****************/

InputMemoryStream::~InputMemoryStream()
{
	Log_Info(LOG_LABEL_MEMORY, "input stream freed");
	std::free(m_buffer);
}

void InputMemoryStream::Read(void* outData, uint32_t byteCount)
{
	if (GetRemainingDataSize() < byteCount)
	{
		Log_Error(LOG_LABEL_MEMORY, "attempted to read %d of %d bytes", byteCount, GetRemainingDataSize());
		return;
	}

	std::memcpy(outData, m_buffer + m_head, byteCount);
	m_head += byteCount;
}

void InputMemoryStream::Read(GameObject* outGameObject)
{
	ObjectNetworkId_t networkId;
	Read(networkId);
	outGameObject = LinkingContext::Get().GetGameObject(networkId);
}

void InputMemoryStream::Read(Vector3& outV3)
{
	uint16_t compressedX;
	Read(compressedX);
	uint16_t compressedZ;
	Read(compressedZ);

	outV3.m_x = ConvertFromFixed(compressedX, -2000.f, .1f);
	outV3.m_z = ConvertFromFixed(compressedZ, -2000.f, .1f);

	Read(outV3.m_y);
}

void InputMemoryStream::Serialize(void* ioData, uint32_t byteCount)
{
	Read(ioData, byteCount);
}

void InputMemoryStream::Serialize(GameObject* gameObject)
{
	Read(gameObject);
}

void InputMemoryStream::Serialize(Vector3& v3)
{
	Read(v3);
}