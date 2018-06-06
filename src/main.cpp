#include "sockets.h"

int main(int argc, char** argv)
{
	bool ok = Sockets_Init();

	// ...frame the game...

	ok = ok && Sockets_Shutdown();

	return ok ? 0 : 1;
}