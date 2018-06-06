#pragma once

#include <stdint.h>

class OutputMemoryStream
{
public:
	OutputMemoryStream();
	~OutputMemoryStream();

	const char* GetBufferPtr() const { return m_buffer; }
	uint32_t GetLength() const { return m_head; }

	void Write(const void* data, size_t byteCount);
	void Write(uint32_t data) { Write(&data, sizeof(data)); }
	void Write(int32_t data) { Write(&data, sizeof(data)); }

private:
	char* m_buffer;
	uint32_t m_head;
	uint32_t m_capacity;

	void ReallocBuffer(uint32_t newLength);
};

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