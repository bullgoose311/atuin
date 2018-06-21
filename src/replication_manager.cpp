#include "replication_manager.h"

#include "bit_streams.h"
#include "entity_registry.h"
#include "game_objects.h"
#include "linking_context.h"
#include "math_utils.h"
#include "network_utils.h"

void ReplicationManager::ReplicateWorldState(OutputMemoryBitStream& outputStream, const std::vector<GameObject*>& objects)
{
	outputStream.WriteBits(PACKET_TYPE_REP_DATA, GetRequiredBits<PACKET_TYPE_MAX>::Value);

	for (GameObject* go : objects)
	{
		ReplicateIntoStream(outputStream, go);
	}
}

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