#include "replication_manager_shared.h"

#include "bit_streams.h"
#include "entity_registry.h"
#include "linking_context.h"
#include "math_utils.h"
#include "delivery_notification_manager.h"

struct ReplicationTransmission
{
	ReplicationTransmission(EntityNetworkId_t networkId, ReplicationAction action, DirtyPropertyMask_t dirtyState) :
		m_networkId(networkId),
		m_action(action),
		m_dirtyState(dirtyState)
	{}

	EntityNetworkId_t m_networkId;
	ReplicationAction m_action;
	DirtyPropertyMask_t m_dirtyState;
};

class ReplicationManagerTransmissionRecord;
typedef std::shared_ptr<ReplicationManagerTransmissionRecord> ReplicationManagerTransmissionRecordPtr;

class ReplicationManagerTransmissionRecord : public DataTransmissionRecord
{
public:
	ReplicationManagerTransmissionRecord(ReplicationManager* repManager) :
		m_repManager(repManager)
	{
	}

	virtual void HandleDeliverySuccess(DeliveryNotificationManager* dnm) const override
	{
		for (const ReplicationTransmission& rt : m_replications)
		{
			EntityNetworkId_t networkId = rt.m_networkId;
			switch (rt.m_action)
			{
			case RA_CREATE:
				m_repManager->HandleCreateAckd(networkId);
				break;
			case RA_DESTROY:
				m_repManager->HandleDestroyAckd(networkId);
				break;
			}
		}
	}

	virtual void HandleDeliveryFailure(DeliveryNotificationManager* dnm) const override
	{
		for (const ReplicationTransmission& rt : m_replications)
		{
			int networkId = rt.m_networkId;
			GameObject* entity;
			switch (rt.m_action)
			{
			case RA_CREATE:
			{
				// Re-create if not destroyed...
				entity = LinkingContext::Get().GetGameObject(rt.m_networkId);
				if (entity)
				{
					m_repManager->BatchCreate(rt.m_networkId, rt.m_dirtyState);
				}
			}
			break;

			case RA_UPDATE:
			{
				// Get the latest state of the entity and re-send it...
				entity = LinkingContext::Get().GetGameObject(rt.m_networkId);
				if (entity)
				{
					DirtyPropertyMask_t dirtyState = rt.m_dirtyState;
					for (const auto& inFlightPacket : dnm->GetInFlightPackets())
					{
						ReplicationManagerTransmissionRecordPtr transmissionRecord =
							std::static_pointer_cast<ReplicationManagerTransmissionRecord>(inFlightPacket.GetTransmissionRecord('RPLM'));
						if (transmissionRecord)
						{
							for (const ReplicationTransmission& otherRT : transmissionRecord->m_replications)
							{
								if (otherRT.m_networkId == networkId)
								{
									// If there's already a packet in flight for this entity, let's make sure
									// we're not wasting bandwidth by sending the same dirty state
									dirtyState &= ~otherRT.m_dirtyState;
								}
							}
						}
					}
					// If there's still any dirty state not in flight, re-batch it
					if (dirtyState)
					{
						m_repManager->BatchDirty(rt.m_networkId, rt.m_dirtyState);
					}
				}
			}
			break;

			case RA_DESTROY:
			{
				m_repManager->BatchDestroy(rt.m_networkId);
			}
			break;
			}
		}
	}

	void TrackReplication(EntityNetworkId_t networkId, ReplicationAction action, DirtyPropertyMask_t dirtyState)
	{
		m_replications.emplace_back(networkId, action, dirtyState);
	}

private:
	ReplicationManager * m_repManager;
	std::vector<ReplicationTransmission> m_replications;
};
typedef std::shared_ptr<ReplicationManagerTransmissionRecord> ReplicationManagerTransmissionRecordPtr;

void ReplicationHeader::Write(OutputMemoryBitStream& outputStream)
{
	outputStream.WriteBits(m_replicationAction, GetRequiredBits<RA_MAX>::Value);
	if (m_replicationAction != RA_RPC)
	{
		outputStream.Write(m_networkId);
		if (m_replicationAction != RA_DESTROY)
		{
			outputStream.Write(m_classId);
		}
	}
}

void ReplicationHeader::Read(InputMemoryBitStream& inputStream)
{
	inputStream.Read(m_replicationAction, GetRequiredBits<RA_MAX>::Value);
	if (m_replicationAction != RA_RPC)
	{
		inputStream.Read(m_networkId);
		if (m_replicationAction != RA_DESTROY)
		{
			inputStream.Read(m_classId);
		}
	}
}

/*
void ReplicationManager::ReplicateWorldState(OutputMemoryBitStream& outputStream, const std::vector<GameObject*>& objects)
{
	outputStream.WriteBits(PT_REPLICATION_DATA, GetRequiredBits<PT_MAX>::Value);

	for (GameObject* go : objects)
	{
		ReplicateIntoStream(outputStream, go);
	}
}
*/

void ReplicationManager::ReplicateEntity(ReplicationAction action, OutputMemoryBitStream& outputStream, GameObject* entity)
{
	EntityNetworkId_t networkId = LinkingContext::Get().GetNetworkId(entity, action == RA_CREATE);

	ReplicationHeader header(action, networkId, entity->GetClassId());

	header.Write(outputStream);

	// TODO: Where does the updated dirty state come from?  It'll need to be pased in here, so we'll need separate
	// functions for create, update and destroy
	entity->Write(outputStream, 0xffffffff);
}

void ReplicationManager::ProcessReplicationAction(InputMemoryBitStream& inputStream)
{
	ReplicationHeader header;
	header.Read(inputStream);

	switch (header.m_replicationAction)
	{
		case RA_CREATE:
		{
			GameObject* entity = EntityFactory::Get().CreateEntity(header.m_classId);
			LinkingContext::Get().AddGameObject(entity, header.m_networkId);
			entity->Read(inputStream);
			break;
		}

		case RA_UPDATE:
		{
			GameObject* entity = LinkingContext::Get().GetGameObject(header.m_networkId);
			if (entity)
			{
				entity->Read(inputStream);
			}
			else
			{
				// We don't have the entity yet, still need to move past it in the stream though
				entity = EntityFactory::Get().CreateEntity(header.m_classId);
				entity->Read(inputStream);
				delete entity;
			}
			break;
		}

		case RA_DESTROY:
		{
			GameObject * entity = LinkingContext::Get().GetGameObject(header.m_networkId);
			if (entity)
			{
				LinkingContext::Get().RemoveGameObject(entity);
				entity->Destroy();
				delete entity;
			}
			break;
		}
	}
}

void ReplicationManager::BatchCreate(EntityNetworkId_t networkId, DirtyPropertyMask_t initialDirtyState)
{
	m_entityToReplicationCommandMap[networkId] = ReplicationCommand(initialDirtyState);
}

void ReplicationManager::BatchDestroy(EntityNetworkId_t networkId)
{
	m_entityToReplicationCommandMap[networkId].SetDestroy();
}

void ReplicationManager::BatchDirty(EntityNetworkId_t networkId, DirtyPropertyMask_t dirtyState)
{
	m_entityToReplicationCommandMap[networkId].AddDirtyState(dirtyState);
}

void ReplicationManager::WriteBatchedCommands(OutputMemoryBitStream& outgoingPacket, InFlightPacket* inFlightPacket)
{
	ReplicationManagerTransmissionRecordPtr transmissionRecord = nullptr;
	for (auto& pair : m_entityToReplicationCommandMap)
	{
		ReplicationCommand& replicationCommand = pair.second;
		if (replicationCommand.HasDirtyState())
		{
			EntityNetworkId_t networkId = pair.first;
			GameObject* entity = LinkingContext::Get().GetGameObject(networkId);
			if (!entity)
			{
				continue;
			}

			ReplicationAction action = replicationCommand.GetAction();
			ReplicationHeader header(action, networkId, entity->GetClassId());
			header.Write(outgoingPacket);
			DirtyPropertyMask_t dirtyState = replicationCommand.GetDirtyState();
			if (action == RA_CREATE || action == RA_UPDATE)
			{
				entity->Write(outgoingPacket, dirtyState);
			}
			
			// We only want to add our transmission record to the in-flight packet if 
			// we actually have entity data to replicate
			if (!transmissionRecord)
			{
				transmissionRecord = std::make_shared<ReplicationManagerTransmissionRecord>(this);
				inFlightPacket->SetTransmissionRecord('RPLM', transmissionRecord);
			}

			transmissionRecord->TrackReplication(networkId, action, dirtyState);

			// Clear dirty state so that we don't try to replicate this object again until it changes
			replicationCommand.ClearDirtyState(dirtyState);
		}
	}
}

void ReplicationManager::HandleCreateAckd(EntityNetworkId_t networkId)
{
	// TODO: Change the replication state from "create" to "update"
}

void ReplicationManager::HandleDestroyAckd(EntityNetworkId_t networkId)
{
	// TODO: Stop replicating this entity
}

/*
void ReplicationManager::ReceiveReplicatedWorldState(InputMemoryBitStream& inputStream)
{
	std::unordered_set<GameObject*> receivedObjects;

	while (inputStream.GetRemainingBitCount() > 0)
	{
		GameObject* receivedGameObject = ReceiveReplicatedObject(inputStream);
		receivedObjects.insert(receivedGameObject);
	}

	for (GameObject* go : m_replicatedEntities)
	{
		if (receivedObjects.find(go) == receivedObjects.end())
		{
			LinkingContext::Get().RemoveGameObject(go);
			go->Destroy();
		}
	}

	m_replicatedEntities = receivedObjects;
}

void ReplicationManager::ReplicateIntoStream(OutputMemoryBitStream& outputStream, GameObject* gameObject)
{
	outputStream.Write(LinkingContext::Get().GetNetworkId(gameObject, true));
	outputStream.Write(gameObject->GetClassId());
	gameObject->Write(outputStream);
}

GameObject* ReplicationManager::ReceiveReplicatedObject(InputMemoryBitStream& inputStream)
{
	EntityNetworkId_t networkId;
	inputStream.Read(networkId);
	EntityClassId_t classId;
	inputStream.Read(classId);
	
	GameObject* entity = LinkingContext::Get().GetGameObject(networkId);
	if (!entity)
	{
		entity = EntityRegistry::Get().CreateEntity(classId);
		LinkingContext::Get().AddGameObject(entity, networkId);
	}

	entity->Read(inputStream);

	return entity;
}
*/

void DebugRPC(OutputMemoryBitStream& outputStream, const std::string& debugMessage)
{
	ReplicationHeader header(RA_RPC);
	header.Write(outputStream);
	outputStream.Write(debugMessage);
}