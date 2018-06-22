#include "replication_manager.h"

#include "bit_streams.h"
#include "entity_registry.h"
#include "linking_context.h"
#include "math_utils.h"

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