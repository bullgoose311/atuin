#pragma once

#include <stdint.h>

#include "sys_utils.h"

// TODO: How do we know what the stream endianness is?
#define STREAM_ENDIANNESS 1
#define PLATFORM_ENDIANNESS 1

class OutputMemoryStream
{
public:
	OutputMemoryStream();
	~OutputMemoryStream();

	const char* GetBufferPtr() const { return m_buffer; }
	uint32_t GetLength() const { return m_head; }

	void Write(const void* data, size_t byteCount);
	//void Write(uint32_t data) { Write(&data, sizeof(data)); }
	//void Write(int32_t data) { Write(&data, sizeof(data)); }
	
	template <typename T> void Write(T data);

private:
	char* m_buffer;
	uint32_t m_head;
	uint32_t m_capacity;

	void ReallocBuffer(uint32_t newLength);
};

template<typename T> void OutputMemoryStream::Write(T data)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "generic write only supports primitive types");

	if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS)
	{
		Write(&data, sizeof(data));
	}
	else
	{
		T swappedData = ByteSwap(data);
		Write(&swappedData, sizeof(swappedData));
	}
}

class InputMemoryStream
{
public:
	InputMemoryStream(char* buffer, uint32_t size) : m_buffer(buffer), m_capacity(size), m_head(0) {}

	~InputMemoryStream();

	uint32_t GetRemainingDataSize() const { return m_capacity - m_head; }

	bool Read(void* outData, uint32_t byteCount);
	bool Read(uint32_t& outData) { return Read(&outData, sizeof(outData)); }
	bool Read(int32_t& outData) { return Read(&outData, sizeof(outData)); }

private:
	char* m_buffer;
	uint32_t m_head;
	uint32_t m_capacity;
};

class OutputMemoryBitStream
{
public:
	OutputMemoryBitStream();
	~OutputMemoryBitStream();

	void WriteBits(const void* data, size_t bitCount);
	template<typename T> void Write(T data, size_t bitCount = sizeof(T) * 8);

	const char* GetBufferPtr() const { return m_buffer; }
	uint32_t GetBitLength() const { return m_bitHead; }
	uint32_t GetByteLength() const { return (m_bitHead + 7) >> 3; }

private:
	char* m_buffer;
	uint32_t m_bitHead;
	uint32_t m_bitCapacity;

	void ReallocBuffer(uint32_t newBitCapacity);
	void WriteBits(uint8_t data, size_t bitCount);
};

template<typename T> void OutputMemoryBitStream::Write(T data, size_t bitCount)
{
	// TODO: Shouldn't write 8 bits for bool, should have a specialized function for that 

	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "generic write only supports primitive data types");

	if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS)
	{
		Write(&data, sizeof(data));
	}
	else
	{
		T swappedData = ByteSwap(data);
		Write(&swappedData, sizeof(swappedData));
	}
}

// TODO: Need InputMemoryBitStream