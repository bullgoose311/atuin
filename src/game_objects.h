#pragma once

#include "streams.h"

#include <stdint.h>
#include <vector>

class GameObject
{

};

class RoboCat : public GameObject
{
public:
	RoboCat() : m_health(10), m_meowCount(3) 
	{
		m_name[0] = '\0';
	}

	virtual void Tick();

	void TestChange() { m_health = 7; m_meowCount = 2; strncpy_s(m_name, "nickj", 5); };

	void Serialize(OutputMemoryStream& stream) const;
	void Deserialize(InputMemoryStream& stream);

	uint32_t m_health;
	uint32_t m_meowCount;
	GameObject* m_homeBase;
	char m_name[128];
	std::vector<int32_t> m_miceIndices;

private:
	//uint32_t m_health;
	//uint32_t m_meowCount;
	//GameObject* m_homeBase;
	//char m_name[128];
	//std::vector<int32_t> m_miceIndices;
};

void Game_SendRoboCat(int socket, const RoboCat* roboCat);