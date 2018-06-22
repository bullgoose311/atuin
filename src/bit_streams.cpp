#include "bit_streams.h"

#include "log.h"

#include <algorithm>
#include <memory>

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

void OutputMemoryBitStream::Write(const std::string& str)
{
	uint32_t elementCount = static_cast< uint32_t >(str.size());
	Write(elementCount);
	for (const char& element : str)
	{
		Write(element);
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

InputMemoryBitStream::~InputMemoryBitStream()
{
	std::free(m_buffer);
}

void InputMemoryBitStream::ReadBits(void* outData, uint32_t bitCount)
{
	uint8_t* destByte = reinterpret_cast< uint8_t* >(outData);
	//write all the bytes
	while (bitCount > 8)
	{
		ReadBits(*destByte, 8);
		++destByte;
		bitCount -= 8;
	}
	//write anything left
	if (bitCount > 0)
	{
		ReadBits(*destByte, bitCount);
	}
}

void InputMemoryBitStream::ReadBits(uint8_t& outData, uint32_t bitCount)
{
	uint32_t byteOffset = m_head >> 3;
	uint32_t bitOffset = m_head & 0x7;

	outData = static_cast< uint8_t >(m_buffer[byteOffset]) >> bitOffset;

	uint32_t bitsFreeThisByte = 8 - bitOffset;
	if (bitsFreeThisByte < bitCount)
	{
		//we need another byte
		outData |= static_cast< uint8_t >(m_buffer[byteOffset + 1]) << bitsFreeThisByte;
	}

	//don't forget a mask so that we only read the bit we wanted...
	outData &= (~(0x00ff << bitCount));

	m_head += bitCount;
}

void InputMemoryBitStream::Read(std::string& str)
{
	uint32_t elementCount;
	Read(elementCount);
	str.resize(elementCount);
	for (auto& element : str)
	{
		Read(element);
	}
}