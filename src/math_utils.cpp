#include "math_utils.h"

std::unique_ptr< RandGen > RandGen::s_instance = nullptr;

void RandGen::StaticInit()
{
	s_instance = std::make_unique<RandGen>();

	// We'll reseed later
	std::random_device rd;
	s_instance->m_generator.seed(rd());
}

void RandGen::Seed(uint32_t seed)
{
	m_generator.seed(seed);
}

uint32_t RandGen::GetRandomUInt32(uint32_t min, uint32_t max)
{
	std::uniform_int_distribution<uint32_t> dist(min, max);
	return dist(m_generator);
}