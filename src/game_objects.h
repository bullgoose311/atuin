#pragma once

#include "log.h"
#include "streams.h"

#include <unordered_map>
#include <stdint.h>
#include <vector>

class GameObject
{

};

typedef uint32_t ObjectNetworkId;

enum
{
	INVALID_OBJECT_NETWORK_ID = 0
};

class LinkingContext
{
public:
	ObjectNetworkId GetNetworkId(const GameObject* gameObject);
	GameObject* GetGameObject(ObjectNetworkId networkId);

private:
	std::unordered_map<ObjectNetworkId, GameObject*> m_idToObjectMap;
	std::unordered_map<const GameObject*, ObjectNetworkId> m_objectToIdMap;
};

struct Vector3
{
	Vector3() : m_x(0), m_y(0), m_z(0) {}
	Vector3(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

	float m_x;
	float m_y;
	float m_z;
};

struct Quaternion
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

	void Serialize(MemoryStream* stream);

	void TestChange(GameObject* home) 
	{ 
		m_health = 7; 
		m_meowCount = 2; 
		strncpy_s(m_name, "nickj", 5); 
		m_miceIndices.push_back(3); 
		m_miceIndices.push_back(2);
		m_miceIndices.push_back(9); 
		m_position.m_x = 1; 
		m_position.m_y = 2; 
		m_position.m_z = 3;
		m_homeBase = home;
	};
	
	void Print()
	{
		Log_Info(LOG_LABEL_DEBUG, "\n---%s---\nhealth: %d\nmeows: %d\nmice: %d\nx: %f\ny: %f\nz: %f\nhome: %s\n---------------", m_name, m_health, m_meowCount, m_miceIndices.size(), m_position.m_x, m_position.m_y, m_position.m_z, m_homeBase == nullptr ? "no" : "yes");
	}

private:
	uint32_t m_health;
	uint32_t m_meowCount;
	GameObject* m_homeBase;
	char m_name[128];
	std::vector<int32_t> m_miceIndices;
	Vector3 m_position;
	Quaternion m_rotation;
};