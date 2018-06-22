#ifdef __WINDOWS__

#include "sockets.h"

#include "array_utils.h"
#include "log.h"
#include "socket_util.h"

#include <stdint.h> // uint32_t, etc
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define MAX_SEGMENT_SIZE 1500 // That's the link layer MTU, what's a good segment size?

static WSAData s_wsaData;

void TestUDP();
void TestTCP();

bool Sockets_Init()
{
	WORD wsaVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wsaVersionRequested, &s_wsaData) != SOCKET_ERROR)
	{
		Log_Info(LOG_LABEL_SOCKETS, "Win sockets initialized");
	}
	else
	{
		Log_Error(LOG_LABEL_SOCKETS, "WSAStartup failed: %d", WSAGetLastError());
		return false;
	}

	// To perform DNS on a hostname and get the IP
	// getaddrinfo("www.google.com", "80", nullptr, results);

	// Send data out over a socket
	// sendto(udpSocket, buff, buffLen, flags, sockAddr, tolen)
	// This call is non-blocking and only queues the data to be sent
	// ***** Link layer MTU is 1500 bytes, to avoid fragmentation our payload shouldn't be bigger than 1300 bytes per datagram

	// Recv data on a socket
	// recvfrom(socket, buff, bufflen, flags, fromaddr, fromlen);

	TestUDP();
	TestTCP();

	return true;
}

static void ProcessNewClient(const TCPSocketPtr& socket, const SocketAddress& address)
{

}

static void ProcessDataFromClient(const TCPSocketPtr& socket, const char* data, size_t dataLength)
{

}

void Sockets_Tick()
{
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(INET);
	SocketAddress receivingAddress((uint32_t)INADDR_ANY, 48000);
	if (listenSocket->Bind(receivingAddress) != NO_ERROR)
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to bind listen socket");
		return;
	}

	std::vector<TCPSocketPtr> readBlockSockets;
	readBlockSockets.push_back(listenSocket);

	std::vector<TCPSocketPtr> readableSockets;

	static bool g_isGameRunning = true;
	while (g_isGameRunning)
	{
		if (!SocketUtil::Select(&readBlockSockets, &readableSockets, nullptr, nullptr, nullptr, nullptr))
		{
			// TODO: Do we sleep?
			continue;
		}

		for (const TCPSocketPtr& socket : readableSockets)
		{
			if (socket == listenSocket)
			{
				SocketAddress newClientAddress;
				TCPSocketPtr newClientSocket = listenSocket->Accept(newClientAddress);
				readBlockSockets.push_back(newClientSocket);
				ProcessNewClient(newClientSocket, newClientAddress);
			}
			else
			{
				char segment[MAX_SEGMENT_SIZE];
				int dataReceived = socket->Receive(segment, ARRAY_COUNT(segment));
				if (dataReceived > 0)
				{
					ProcessDataFromClient(socket, segment, dataReceived);
				}
			}
		}
	}
}

void Sockets_Shutdown()
{
	bool ok = WSACleanup() != SOCKET_ERROR;
	if (!ok)
	{
		Log_Error(LOG_LABEL_SOCKETS, "WSACleanup failed: %d", WSAGetLastError());
	}
}

void TestUDP()
{
	SocketAddressPtr udpSocketAddress = SocketAddressFactory::CreateIPv4SocketAddress("localhost:51983");
	if (!udpSocketAddress)
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to create socket address");
		return;
	}

	UDPSocketPtr udpSocket = SocketUtil::CreateUDPSocket(INET);
	if (!udpSocket)
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to create udp socket");
		return;
	}

	int result = udpSocket->Bind(*udpSocketAddress);
	if (result == SOCKET_ERROR)
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to bind");
	}
}

void TestTCP()
{
	SocketAddressPtr socketAddress = SocketAddressFactory::CreateIPv4SocketAddress("localhost:51983");
	if (!socketAddress)
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to create socket address");
		return;
	}

	TCPSocketPtr socket = SocketUtil::CreateTCPSocket(INET);
	if (!socket)
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to create tcp socket");
		return;
	}

	if (!socket->Bind(*socketAddress))
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to bind tcp socket");
		return;
	}

	if (!socket->Listen())
	{
		Log_Error(LOG_LABEL_SOCKETS, "failed to listen");
		return;
	}
}

#endif