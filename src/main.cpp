#include "sockets.h"

#include "bit_streams.h"
#include "game_objects.h"
#include "log.h"
#include "multiplayer.h"

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
	MouseStatus mouseStatusSrc;
	mouseStatusSrc.Test();
	mouseStatusSrc.Print();

	OutputMemoryBitStream outputStream;
	mouseStatusSrc.Write(outputStream, 0xffffffff);

	char* tempBuffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	std::memcpy(tempBuffer, outputStream.GetBufferPtr(), outputStream.GetBitLength() / 8);

	InputMemoryBitStream inputStream(tempBuffer, outputStream.GetBitLength());

	MouseStatus mouseStatusDst;
	mouseStatusDst.Read(inputStream);
	mouseStatusDst.Print();
}

int main(int argc, char** argv)
{
	bool ok = Sockets_Init();
	ok = ok && Entities_Init();
	ok = ok && Multiplayer_Init();

	if (ok)
	{
		// run game loop
	}

	//SerializationTest();
	BitSerializationTest();

	Sockets_Shutdown();

	return ok ? 0 : 1;
}