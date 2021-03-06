#pragma once

#include "bit_streams.h"
#include "input.h"
#include "log.h"
#include "render_utils.h"
#include "streams.h"

#include <unordered_map>
#include <stdint.h>
#include <vector>

#define ENTITY_CLASS_IDENTIFICATION(code, clazz) \
	enum { kEntityClassId = code }; \
	virtual EntityClassId_t GetClassId() const { return kEntityClassId; } \
	static GameObject* CreateInstance() { return new clazz(); }

typedef uint32_t EntityClassId_t;

enum
{
	INVALID_ENTITY_CLASS_ID = 0
};

typedef uint32_t DirtyPropertyMask_t;

typedef GameObject* (*EntityCreationFunc)();

class GameObject
{
private:
	enum { kEntityClassId = 'GOBJ' };

public:
	virtual EntityClassId_t GetClassId() const { return kEntityClassId; }
	static GameObject* CreateInstance() { return nullptr; }

	virtual void Tick() {}

	virtual void Write(OutputMemoryBitStream& outputStream, DirtyPropertyMask_t dirtyMask) { (void)outputStream; };
	virtual void Read(InputMemoryBitStream& inputStream) { (void)inputStream; }

	virtual void Destroy() {}
};

class RoboCat : public GameObject
{
public:
	RoboCat() : m_health(10), m_meowCount(3) 
	{
		m_name[0] = '\0';
	}

	void ProcessInput(float deltaTime, const InputState& inputState);
	void SimulateMovement(float deltaTime);

	void Serialize(MemoryStream* stream);  // TODO: Delete very soon...
	virtual void Write(OutputMemoryBitStream& outputStream, DirtyPropertyMask_t dirtyMask) override;
	virtual void Read(InputMemoryBitStream& inputStream) override;

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

	ENTITY_CLASS_IDENTIFICATION('RBCT', RoboCat)

private:
	uint32_t m_health;
	uint32_t m_meowCount;
	GameObject* m_homeBase;
	char m_name[128];
	std::vector<int32_t> m_miceIndices;
	Vector3 m_position;
	Quaternion m_rotation;
};

enum MouseStatusProps
{
	MSP_NAME		= 1 << 0,
	MSP_LEG_COUNT		= 1 << 1,
	MSP_HEAD_COUNT	= 1 << 2,
	MSP_HEALTH		= 1 << 3,
	MSP_MAX
};

class MouseStatus : public GameObject
{
public:
	ENTITY_CLASS_IDENTIFICATION('MSTS', MouseStatus)

	virtual void Write(OutputMemoryBitStream& outputStream, DirtyPropertyMask_t dirtyMask) override;
	virtual void Read(InputMemoryBitStream& inputStream) override;

	virtual void Test() { strncpy_s(m_name, "jay-z", 5); m_legCount = 6; m_headCount = 2; m_health = 75; }
	virtual void Print() { Log_Info(LOG_LABEL_DEBUG, "\n---%s---\nlegs: %d\nheads: %d\nhealth: %d\n---------------", m_name, m_legCount, m_headCount, m_health); }

private:
	char m_name[128];
	uint32_t m_legCount;
	uint32_t m_headCount;
	uint32_t m_health;

	void Serialize(BitStream* stream, DirtyPropertyMask_t dirtyMask);
};

bool Entities_Init();