#pragma once

#include <stdint.h>
#include <unordered_map>

typedef uint32_t rpcId_t;
typedef void(*RPCUnwrapFunc)(class InputMemoryBitStream&);

class RPCManager
{
public:
	static RPCManager& Get()
	{
		static RPCManager m_instance;
		return m_instance;
	}

	void RegisterUnwrapFunction(rpcId_t rpcId, RPCUnwrapFunc func);
	void ProcessRPC(InputMemoryBitStream& stream);

private:
	std::unordered_map<rpcId_t, RPCUnwrapFunc> m_rpcUnwrapMap;

	RPCManager() {}
};

void DebugRPC(const std::string& debugMessage);

bool Multiplayer_Init();