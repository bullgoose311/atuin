#pragma once

#include "game_objects.h"
#include "network_manager_server.h"

class GameObjectServer
{
public:
	GameObjectServer(playerId_t playerId)
		: m_owningPlayerId(playerId)
	{}

protected:
	playerId_t GetOwningPlayerId() { return m_owningPlayerId; }

private:
	playerId_t m_owningPlayerId;
};

class RoboCatServer : public RoboCat, public GameObjectServer
{
public:
	virtual void Tick() override;

private:
	void HandleShooting();
};