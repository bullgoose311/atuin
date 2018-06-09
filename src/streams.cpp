#include "streams.h"

#include "game_objects.h"
#include "log.h"
#include "math_utils.h"

#include <algorithm>
#include <memory>

extern LinkingContext g_linkingContext;

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
	ObjectNetworkId networkId = g_linkingContext.GetNetworkId(gameObject);
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
	ObjectNetworkId networkId;
	Read(networkId);
	outGameObject = g_linkingContext.GetGameObject(networkId);
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

/********************
OutputMemoryBitStream
********************/

OutputMemoryBitStream::OutputMemoryBitStream()
{
	ReallocBuffer(256);
}

OutputMemoryBitStream::~OutputMemoryBitStream()
{
	std::free(m_buffer);
}

void OutputMemoryBitStream::WriteBits(uint8_t data, size_t bitCount)
{
	uint32_t nextBitHead = m_bitHead + static_cast<uint32_t>(bitCount);
	if (nextBitHead > m_bitCapacity)
	{
		ReallocBuffer(std::max(m_bitCapacity * 2, nextBitHead));
	}

	// calculate the byteOffset into our buffer by dividing the head by 8
	// and the bitOffset by taking the last 3 bits
	// In other words, shift away the last 3 bits to get byte offset, then the value of those 3 bits is our bit index
	uint32_t byteOffset = m_bitHead >> 3;
	uint32_t bitOffset = m_bitHead & 0x7; // 0111

	// Preserve what's currently in the byte then OR in remaining bits with the new data
	uint8_t currentMask = ~(0xff << bitOffset); // Mask used to preserve what we have
	m_buffer[byteOffset] = (m_buffer[byteOffset] & currentMask) | (data << bitOffset); // right shift new data to plug in new data

	// calculate how many bits were not yet used in our target byte in the buffer
	uint32_t bitsFreeThisByte = 8 - bitOffset;

	// if we needed more than that, carry to the next byte
	if (bitsFreeThisByte < bitCount)
	{
		m_buffer[byteOffset + 1] = data >> bitsFreeThisByte;
	}

	m_bitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* data, size_t bitCount)
{
	const char* srcByte = static_cast<const char*>(data);

	while (bitCount > 8)
	{
		WriteBits(*srcByte, 8);
		++srcByte;
		bitCount -= 8;
	}

	if (bitCount > 0)
	{
		WriteBits(*srcByte, bitCount);
	}
}

void OutputMemoryBitStream::ReallocBuffer(uint32_t newBitCapacity)
{
	newBitCapacity += (newBitCapacity % 8);

	Log_Info(LOG_LABEL_MEMORY, "reallocating bit buffer from size %d to size %d", m_bitCapacity, newBitCapacity);

	m_buffer = static_cast<char*>(std::realloc(m_buffer, newBitCapacity * 8));

	// TODO: Handler allocation failure...

	m_bitCapacity = newBitCapacity;
}