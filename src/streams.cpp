#include "streams.h"

#include "log.h"

#include <algorithm>
#include <memory>

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

bool InputMemoryStream::Read(void* outData, uint32_t byteCount)
{
	if (GetRemainingDataSize() < byteCount)
	{
		Log_Error(LOG_LABEL_MEMORY, "attempted to read %d of %d bytes", byteCount, GetRemainingDataSize());
		return false;
	}

	std::memcpy(outData, m_buffer + m_head, byteCount);
	m_head += byteCount;
	return true;
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