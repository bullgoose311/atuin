#pragma once

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
	ObjectNetworkId GetNetworkId(GameObject* gameObject);
	GameObject* GetGameObject(ObjectNetworkId networkId);

private:
	std::unordered_map<ObjectNetworkId, GameObject*> m_idToObjectMap;
	std::unordered_map<GameObject*, ObjectNetworkId> m_objectToIdMap;
};

struct Vector3
{
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

	void TestChange() { m_health = 7; m_meowCount = 2; strncpy_s(m_name, "nickj", 5); m_miceIndices.push_back(3); m_miceIndices.push_back(2); m_miceIndices.push_back(9); };

	void Serialize(OutputMemoryStream& stream) const;
	void Deserialize(InputMemoryStream& stream);

	// TODO: Make private after testing
	uint32_t m_health;
	uint32_t m_meowCount;
	GameObject* m_homeBase;
	char m_name[128];
	std::vector<int32_t> m_miceIndices;

	Vector3 m_position;
	Quaternion m_rotation;

private:
	//uint32_t m_health;
	//uint32_t m_meowCount;
	//GameObject* m_homeBase;
	//char m_name[128];
	//std::vector<int32_t> m_miceIndices;
};

void Game_SendRoboCat(int socket, const RoboCat* roboCat);