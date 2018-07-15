#pragma once

#include <stdint.h>
#include <memory>
#include <random>

inline uint32_t ConvertToFixed(float number, float min, float precision)
{
	return static_cast<uint32_t>((number - min) / precision);
}

inline float ConvertFromFixed(uint32_t number, float min, float precision)
{
	return (static_cast<float>(number) * precision) + min;
}

template< int tValue, int tBits >
struct GetRequiredBitsHelper
{
	enum { Value = GetRequiredBitsHelper< (tValue >> 1), tBits + 1 >::Value };
};

template< int tBits >
struct GetRequiredBitsHelper< 0, tBits >
{
	enum { Value = tBits };
};

template< int tValue >
struct GetRequiredBits
{
	enum { Value = GetRequiredBitsHelper< tValue, 0 >::Value };
};

class RandGen
{
public:
	static std::unique_ptr<RandGen> s_instance;

	static void StaticInit();
	void Seed(uint32_t seed);
	std::mt19937& GetGeneratorRef() { return m_generator; }

	// float GetRandomFloat();
	uint32_t GetRandomUInt32(uint32_t min, uint32_t max);
	//int32_t GetRandomInt(int32_t min, int32_t max);

private:
	std::mt19937 m_generator;
	// std::uniform_real_distribution<float> m_floatDistr;
};