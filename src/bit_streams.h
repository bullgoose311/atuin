#pragma once

#include "render_utils.h"

#include <stdint.h>
#include <string>

class OutputMemoryBitStream
{
public:
	OutputMemoryBitStream();
	~OutputMemoryBitStream();

	void WriteBits(uint8_t data, size_t bitCount);
	void WriteBits(const void* data, size_t bitCount);

	template<typename T> void Write(T data, size_t bitCount = sizeof(T) * 8)
	{
		// TODO: Shouldn't write 8 bits for bool, should have a specialized function for that 
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "generic write only supports primitive data types");
		WriteBits(&data, bitCount);
	}

	void Write(const std::string& string);

	const char* GetBufferPtr() const { return m_buffer; }
	uint32_t GetBitLength() const { return m_bitHead; }
	uint32_t GetByteLength() const { return (m_bitHead + 7) >> 3; }

private:
	char* m_buffer;
	uint32_t m_bitHead;
	uint32_t m_bitCapacity;

	void ReallocBuffer(uint32_t newBitCapacity);
};

class InputMemoryBitStream
{
public:
	InputMemoryBitStream(char* buffer, uint32_t size) : m_buffer(buffer), m_capacity(size) {}
	~InputMemoryBitStream();

	const char*	GetBufferPtr() const { return m_buffer; }
	uint32_t GetRemainingBitCount() const { return m_capacity - m_head; }

	void ReadBits(void* outData, uint32_t bitCount);
	void ReadBits(uint8_t& outData, uint32_t bitCount);

	template<typename T> void Read(T& data, uint32_t bitCount = sizeof(T) * 8)
	{
		static_assert(std::is_arithmetic<T>::value || std::is_enum< T >::value, "generic read only supports primitive data types");
		ReadBits(&data, bitCount);
	}

	void Read(std::string& str);

private:
	char* m_buffer;
	uint32_t m_head;
	uint32_t m_capacity;
};