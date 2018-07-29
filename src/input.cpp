#include "input.h"

bool InputState::Write(OutputMemoryBitStream& outputStream) const
{
	return true;
}

bool InputState::Read(InputMemoryBitStream& outputStream)
{
	return true;
}

bool Move::Write(OutputMemoryBitStream& outputStream) const
{
	return true;
}

bool Move::Read(InputMemoryBitStream& outputStream)
{
	return true;
}

const Move& MoveList::AddMove(const InputState& inputState, float timestamp)
{
	// first move has 0 delta time
	float deltaTime = m_lastMoveTimestamp >= 0.f ? timestamp - m_lastMoveTimestamp : 0.f;

	m_moves.emplace_back(inputState, timestamp, deltaTime);
	m_lastMoveTimestamp = timestamp;
	return m_moves.back();
}

bool MoveList::AddMoveIfNew(const Move& move)
{
	float timestamp = move.GetTimestamp();
	if (timestamp > m_lastMoveTimestamp)
	{
		float deltaTime = m_lastMoveTimestamp >= 0.f ? timestamp - m_lastMoveTimestamp : 0.f;
		m_lastMoveTimestamp = timestamp;
		m_moves.emplace_back(move.GetInputState(), timestamp, deltaTime);
		return true;
	}

	return false;
}