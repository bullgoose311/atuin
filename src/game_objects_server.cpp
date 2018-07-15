#include "game_objects_server.h"

#include "network_manager_server.h"

void RoboCatServer::Tick()
{
	RoboCat::Tick();

	ClientProxyPtr client = NetworkManagerServer::Get().GetClientProxy(GetOwningPlayerId());
	if (client)
	{
		MoveList& moveList = client->GetUnprocessedMoveList();
		for (const Move& unprocessedMove : moveList)
		{
			const InputState& currentState = unprocessedMove.GetInputState();
			float deltaTime = unprocessedMove.GetDeltaTime();
			ProcessInput(deltaTime, currentState);
			SimulateMovement(deltaTime);
		}

		moveList.Clear();
	}

	HandleShooting();
}

void RoboCatServer::HandleShooting()
{

}