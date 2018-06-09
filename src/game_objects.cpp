#include "game_objects.h"

#include "array_utils.h"
#include "log.h"

void RoboCat::Tick()
{

}

void RoboCat::Serialize(OutputMemoryStream& stream) const
{
	stream.Write<uint32_t>(m_health);
	stream.Write(m_meowCount);
	//no solution for m_homeBase yet
	stream.Write(m_name, ARRAY_COUNT(m_name));
	//no solution for m_mouseIndices yet
}

void RoboCat::Deserialize(InputMemoryStream& stream)
{
	bool ok = stream.Read(m_health);
	ok = ok && stream.Read(m_meowCount);
	//no solution for homeBase yet
	ok = ok && stream.Read(m_name, ARRAY_COUNT(m_name));
	ok = ok && stream.Read(m_meowCount); // this should fail...
	//no solution for m_miceIndices yet

	if (!ok)
	{
		Log_Error(LOG_LABEL_DEFAULT, "failed to deserialize robocat");
	}
}

void Game_SendRoboCat(int socket, const RoboCat* roboCat)
{
	OutputMemoryStream stream;
	roboCat->Serialize(stream);
	// send(socket, stream.GetBufferPtr(), stream.GetLength(), 0);
}