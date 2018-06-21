#pragma once

#include <unordered_set>
#include <vector>

class GameObject;
class InputMemoryBitStream;
class OutputMemoryBitStream;

class ReplicationManager
{
public:
	void ReplicateWorldState(OutputMemoryBitStream& outputStream, const std::vector<GameObject*>& objects);
	void ReceiveReplicatedWorldState(InputMemoryBitStream& inputStream);

private:
	std::unordered_set<GameObject*> m_replicatedEntities;

	void ReplicateIntoStream(OutputMemoryBitStream& outputStream, GameObject* gameObject);
	GameObject* ReceiveReplicatedObject(InputMemoryBitStream& inputStream);
};