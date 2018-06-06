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
	Log_Info(LOG_LABEL_MEMORY, "output stream free'd");
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
	Log_Info(LOG_LABEL_MEMORY, "input stream free'd");
	std::free(m_buffer);
}

void InputMemoryStream::Read(void* outData, uint32_t byteCount)
{
	uint32_t bytesToCopy = std::min(GetRemainingDataSize(), byteCount);
	std::memcpy(outData, m_buffer + m_head, bytesToCopy);
	m_head += bytesToCopy;
}