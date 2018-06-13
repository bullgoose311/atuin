#include "sockets.h"

#include "bit_streams.h"
#include "game_objects.h"
#include "log.h"

#include <cassert>
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

void BitSerializationTest()
{
	OutputMemoryBitStream outputBitStream;

	int out = 0xE;			// 1110
	int expectedIn = 0x6;	//  110

	outputBitStream.Write(out, 3);

	char* tempBuffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	std::memcpy(tempBuffer, outputBitStream.GetBufferPtr(), outputBitStream.GetBitLength());

	InputMemoryBitStream inputBitStream(tempBuffer, outputBitStream.GetBitLength());

	int in = 0;
	inputBitStream.Read(in, 3);

	assert(expectedIn == in);

	Log_Info(LOG_LABEL_DEBUG, "bit serialization test passed");
}

int main(int argc, char** argv)
{
	bool ok = Sockets_Init();

	// ...frame the game...

	SerializationTest();
	BitSerializationTest();

	ok = ok && Sockets_Shutdown();

	return ok ? 0 : 1;
}