#pragma once

#include "game_objects.h"
#include "network_utils.h"

#include <unordered_map>
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
	RA_RPC,
	// RA_RMI,		// TODO: This would need a network ID for the object along with a method identifier
	RA_MAX
};

// TODO: Make this class private to ReplicationManager
class ReplicationHeader
{
public:
	ReplicationHeader() {}
	ReplicationHeader(ReplicationAction replicationAction) : ReplicationHeader(replicationAction, INVALID_ENTITY_NETWORK_ID, INVALID_ENTITY_CLASS_ID) {}
	ReplicationHeader(ReplicationAction replicationAction, EntityNetworkId_t networkId, EntityClassId_t classId)
		: m_replicationAction(replicationAction), m_networkId(networkId), m_classId(classId) {}

	ReplicationAction m_replicationAction;
	EntityNetworkId_t m_networkId;
	EntityClassId_t m_classId;

	void Write(OutputMemoryBitStream& outputStream);
	void Read(InputMemoryBitStream& inputStream);
};

class ReplicationCommand
{
public:
	ReplicationCommand() :
		m_action(RA_CREATE),
		m_dirtyState(0)
	{}

	ReplicationCommand(DirtyPropertyMask_t dirtyState) :
		m_action(RA_CREATE),
		m_dirtyState(dirtyState)
	{}

	void SetDestroy()
	{
		m_action = RA_DESTROY;
	}

	void AddDirtyState(DirtyPropertyMask_t dirtyState)
	{
		m_dirtyState |= dirtyState;
	}

	bool HasDirtyState()
	{
		return m_action == RA_DESTROY || m_dirtyState != 0;
	}

	void ClearDirtyState(DirtyPropertyMask_t stateToClear)
	{
		m_dirtyState &= ~stateToClear;
		if (m_action == RA_DESTROY)
		{
			m_action = RA_UPDATE;
		}
	}

	ReplicationAction GetAction() { return m_action; }
	DirtyPropertyMask_t GetDirtyState() { return m_dirtyState; }

private:
	ReplicationAction m_action;
	DirtyPropertyMask_t m_dirtyState;
};

class ReplicationManager
{
public:
	// void ReplicateWorldState(OutputMemoryBitStream& outputStream, const std::vector<GameObject*>& objects);

	// TODO: Should be server only
	void ReplicateEntity(ReplicationAction action, OutputMemoryBitStream& outputStream, GameObject* entity);

	// void ReceiveReplicatedWorldState(InputMemoryBitStream& inputStream);

	// TODO: Should be client only
	void ProcessReplicationAction(InputMemoryBitStream& inputStream);

	void BatchCreate(EntityNetworkId_t networkId, DirtyPropertyMask_t initialDirtyState);
	void BatchDestroy(EntityNetworkId_t networkId);
	void BatchDirty(EntityNetworkId_t networkId, DirtyPropertyMask_t dirtyState);
	void WriteBatchedCommands(OutputMemoryBitStream& outgoingPacket, class InFlightPacket* inFlightPacket);

	void HandleCreateAckd(EntityNetworkId_t networkId);
	void HandleDestroyAckd(EntityNetworkId_t networkId);

private:
	std::unordered_set<GameObject*> m_replicatedEntities;
	std::unordered_map<EntityNetworkId_t, ReplicationCommand> m_entityToReplicationCommandMap;

	//void ReplicateIntoStream(OutputMemoryBitStream& outputStream, GameObject* gameObject);
	//GameObject* ReceiveReplicatedObject(InputMemoryBitStream& inputStream);
};

void DebugRPC(OutputMemoryBitStream& outputStream, const std::string& debugMessage);