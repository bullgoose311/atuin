#include "game_objects.h"

#include "array_utils.h"

void RoboCat::Tick()
{

}

void RoboCat::Serialize(OutputMemoryStream& stream) const
{
	stream.Write(m_health);
	stream.Write(m_meowCount);
	//no solution for m_homeBase yet
	stream.Write(m_name, ARRAY_COUNT(m_name));
	//no solution for m_mouseIndices yet
}

void RoboCat::Deserialize(InputMemoryStream& stream)
{
	stream.Read(m_health);
	stream.Read(m_meowCount);
	//no solution for homeBase yet
	stream.Read(m_name, ARRAY_COUNT(m_name));
	//no solution for m_miceIndices yet
}

void Game_SendRoboCat(int socket, const RoboCat* roboCat)
{
	OutputMemoryStream stream;
	roboCat->Serialize(stream);
	// send(socket, stream.GetBufferPtr(), stream.GetLength(), 0);
}