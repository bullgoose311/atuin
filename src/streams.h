#pragma once

#include "sys_utils.h"

#include <stdint.h>
#include <vector>

// TODO: How do we know what the stream endianness is?
#define STREAM_ENDIANNESS 1
#define PLATFORM_ENDIANNESS 1

class GameObject;
struct Vector3;

enum StreamDirection_e
{
	STREAM_DIR_INPUT,
	STREAM_DIR_OUTPUT
};

// MemoryStream

class MemoryStream
{
public:
	virtual void Serialize(void* ioData, uint32_t byteCount) = 0;
	virtual void Serialize(GameObject* gameObject) = 0;
	virtual void Serialize(Vector3& v3) = 0;
	virtual void Serialize(char* str);

	template<typename T> void Serialize(T& data);
	template<typename T> void Serialize(std::vector<T>& vector);

private:
	virtual StreamDirection_e GetDirection() = 0;
};

template<typename T> void MemoryStream::Serialize(T& ioData)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "generic serialize only supports primitive data types");

	if (STREAM_ENDIANNESS == PLATFORM_ENDIANNESS)
	{
		Serialize(&ioData, sizeof(ioData));
	}
	else
	{
		if (GetDirection() == STREAM_DIR_INPUT)
		{
			T data;
			Serialize(&data, sizeof(T));
			ioData = ByteSwap(data);
		}
		else
		{
			T swappedData = ByteSwap(ioData);
			Serialize(&swappedData, sizeof(swappedData));
		}
	}
}

template<typename T> void MemoryStream::Serialize(std::vector<T>& vector)
{
	if (GetDirection() == STREAM_DIR_INPUT)
	{
		uint32_t size;
		Serialize(size);
		vector.resize(size);
	}
	else
	{
		uint32_t size = static_cast<uint32_t>(vector.size());
		Serialize(size);
	}

	for (T& t : vector)
	{
		Serialize(t);
	}
}

// OutputMemoryStream

class OutputMemoryStream : public MemoryStream
{
public:
	OutputMemoryStream();
	~OutputMemoryStream();

	const char* GetBufferPtr() const { return m_buffer; }
	uint32_t GetLength() const { return m_head; }

	void Write(const void* data, size_t byteCount);
	void Write(const GameObject* gameObject);
	void Write(const Vector3& v3);
	
	template <typename T> void Write(const T& data);
	template <typename T> void Write(const std::vector<T>& vector);

	// MemoryStream
	virtual void Serialize(void* ioData, uint32_t byteCount) override;
	virtual void Serialize(GameObject* gameObject) override;
	virtual void Serialize(Vector3& v3) override;

private:
	char* m_buffer;
	uint32_t m_head;
	uint32_t m_capacity;

	void ReallocBuffer(uint32_t newLength);

	// MemoryStream
	virtual StreamDirection_e GetDirection() { return STREAM_DIR_OUTPUT; }
};

template<typename T> void OutputMemoryStream::Write(const T& data)
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

template<typename T> void OutputMemoryStream::Write(const std::vector<T>& vector)
{
	size_t size = vector.size();
	Write(size);
	for (const T& element : vector)
	{
		Write(element);
	}
}

// InputMemoryStream

class InputMemoryStream : public MemoryStream
{
public:
	InputMemoryStream(char* buffer, uint32_t size) : m_buffer(buffer), m_capacity(size), m_head(0) {}

	~InputMemoryStream();

	uint32_t GetRemainingDataSize() const { return m_capacity - m_head; }

	void Read(void* outData, uint32_t byteCount);
	void Read(GameObject* outGameObject);
	void Read(Vector3& outV3);

	template<typename T> void Read(T& data);
	template<typename T> void Read(std::vector<T>& vector);

	// MemoryStream
	virtual void Serialize(void* ioData, uint32_t byteCount) override;
	virtual void Serialize(GameObject* gameObject) override;
	virtual void Serialize(Vector3& v3) override;

private:
	char* m_buffer;
	uint32_t m_head;
	uint32_t m_capacity;

	// MemoryStream
	virtual StreamDirection_e GetDirection() { return STREAM_DIR_INPUT; }
};

template <typename T> void InputMemoryStream::Read(T& data)
{
	static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "generic read only works on primitive data types");

	Read(&data, sizeof(data));
}

template <typename T> void InputMemoryStream::Read(std::vector<T>& vector)
{
	size_t size;
	Read(size);
	vector.resize(size);
	for (T& element : vector)
	{
		Read(element);
	}
}