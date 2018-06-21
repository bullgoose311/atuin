#pragma once

#include "game_objects.h"
#include "network_utils.h"

#include <unordered_set>
#include <vector>

class GameObject;
class InputMemoryBitStream;
class OutputMemoryBitStream;

enum ReplicationAction
{
	RA_CREATE,
	RA_UPDATE,
	RA_DESTROY,
	RA_MAX
};

// TODO: Make this class private to ReplicationManager
class ReplicationHeader
{
public:
	ReplicationHeader() {}
	ReplicationHeader(ReplicationAction replicationAction, EntityNetworkId_t networkId, EntityClassId_t classId)
		: m_replicationAction(replicationAction), m_networkId(networkId), m_classId(classId) {}

	ReplicationAction m_replicationAction;
	EntityNetworkId_t m_networkId;
	EntityClassId_t m_classId;

	void Write(OutputMemoryBitStream& outputStream);
	void Read(InputMemoryBitStream& inputStream);
};

class ReplicationManager
{
public:
	// void ReplicateWorldState(OutputMemoryBitStream& outputStream, const std::vector<GameObject*>& objects);

	void ReplicateEntity(ReplicationAction action, OutputMemoryBitStream& outputStream, GameObject* entity);

	// void ReceiveReplicatedWorldState(InputMemoryBitStream& inputStream);

	void ProcessReplicationAction(InputMemoryBitStream& inputStream);

private:
	std::unordered_set<GameObject*> m_replicatedEntities;

	void ReplicateIntoStream(OutputMemoryBitStream& outputStream, GameObject* gameObject);
	GameObject* ReceiveReplicatedObject(InputMemoryBitStream& inputStream);
};