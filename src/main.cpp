#include "sockets.h"

#include "game_objects.h"
#include "log.h"

#include <memory>
#include <stdint.h>

const uint32_t kMaxPacketSize = 1470;

void SerializationTest()
{
	GameObject home;
	RoboCat roboCatSrc;
	roboCatSrc.TestChange(&home);
	roboCatSrc.Print();

	OutputMemoryStream outputStream;

	roboCatSrc.Serialize(&outputStream);

	char* tempBuffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	std::memcpy(tempBuffer, outputStream.GetBufferPtr(), outputStream.GetLength());

	InputMemoryStream inputStream(tempBuffer, outputStream.GetLength());

	RoboCat roboCatDst;
	roboCatDst.Serialize(&inputStream);
	roboCatDst.Print();
}

int main(int argc, char** argv)
{
	bool ok = Sockets_Init();

	// ...frame the game...

	SerializationTest();

	ok = ok && Sockets_Shutdown();

	return ok ? 0 : 1;
}