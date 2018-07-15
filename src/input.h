#pragma once

#include "bit_streams.h"

#include <deque>

class InputState
{
public:
	InputState() :
		m_desiredLeftAmount(0),
		m_desiredRightAmount(0),
		m_desiredForwardAmount(0),
		m_desiredBackAmount(0),
		m_bShooting(false)
	{
	}

	float GetDesiredHorizontalDelta() const { return m_desiredRightAmount - m_desiredLeftAmount; }
	float GetDesiredVerticalDelta() const { return m_desiredForwardAmount - m_desiredBackAmount; }
	bool IsShooting() { return m_bShooting; }

	bool Write(OutputMemoryBitStream& outputStream) const;
	bool Read(InputMemoryBitStream& outputStream);

private:
	friend class InputManager;

	float m_desiredLeftAmount;
	float m_desiredRightAmount;
	float m_desiredForwardAmount;
	float m_desiredBackAmount;
	bool m_bShooting;
};

class Move
{
public:
	Move() {}
	Move(const InputState& inputState, float timestamp, float deltaTime) :
		m_inputState(inputState),
		m_timestamp(timestamp),
		m_deltaTime(deltaTime)
	{
	}

	const InputState& GetInputState() const { return m_inputState; }
	float GetTimestamp() const { return m_timestamp; }
	float GetDeltaTime() const { return m_deltaTime; }

	bool Write(OutputMemoryBitStream& outputStream) const;
	bool Read(InputMemoryBitStream& inputStream);

private:
	InputState m_inputState;
	float m_timestamp;
	float m_deltaTime;
};

class MoveList
{
public:
	typedef std::deque< Move >::const_iterator const_iterator;
	typedef std::deque< Move >::const_reverse_iterator const_reverse_iterator;

	MoveList()
		: m_lastMoveTimestamp(0)
	{}

	const Move& AddMove(const InputState& inputState, float timestamp);
	bool HasMoves() { return m_moves.size() > 0; }
	size_t GetMoveCount() { return m_moves.size(); }
	void Clear() { m_moves.clear(); }

	const Move& operator[](int index) const
	{
		return m_moves[index];
	}

	const_iterator begin() const { return m_moves.begin(); }
	const_iterator end() const { return m_moves.end(); }

private:
	std::deque<Move> m_moves;
	float m_lastMoveTimestamp;
};

class InputManager
{
public:
	static InputManager& Get()
	{
		static InputManager s_instance;
		return s_instance;
	}

	MoveList& GetMoveList() { return m_moveList; }

private:
	MoveList m_moveList;

	InputManager() {}
};