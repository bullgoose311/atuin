#include "sockets.h"

#include "game_objects.h"
#include "log.h"

#include <memory>
#include <stdint.h>

const uint32_t kMaxPacketSize = 1470;

void SerializationTest()
{
	RoboCat roboCatSrc;
	roboCatSrc.TestChange();

	Log_Info(LOG_LABEL_DEBUG, "SRC: %d - %d - %s - %d", roboCatSrc.m_health, roboCatSrc.m_meowCount, roboCatSrc.m_name, roboCatSrc.m_miceIndices.size());

	OutputMemoryStream outputStream;

	roboCatSrc.Serialize(outputStream);

	char* tempBuffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	std::memcpy(tempBuffer, outputStream.GetBufferPtr(), outputStream.GetLength());

	InputMemoryStream inputStream(tempBuffer, outputStream.GetLength());

	RoboCat roboCatDst;
	roboCatDst.Deserialize(inputStream);

	Log_Info(LOG_LABEL_DEBUG, "DST: %d - %d - %s - %d", roboCatDst.m_health, roboCatDst.m_meowCount, roboCatDst.m_name, roboCatDst.m_miceIndices.size());
}

int main(int argc, char** argv)
{
	bool ok = Sockets_Init();

	// ...frame the game...

	SerializationTest();

	ok = ok && Sockets_Shutdown();

	return ok ? 0 : 1;
}