#include "socket_util.h"

#include "log.h"
#include "udp_socket.h"

#include <stdarg.h>
#include <WinSock2.h>

int SocketUtil::GetLastError()
{
	return WSAGetLastError();
}

void SocketUtil::ReportError(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Log_Error(LOG_LABEL_SOCKETS, fmt, args);
	va_end(args);
}

UDPSocketPtr SocketUtil::CreateUDPSocket(SocketAddressFamily addressFamily)
{
	SOCKET s = socket(addressFamily, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET)
	{
		SocketUtil::ReportError("failed to create UDP socket");
		return nullptr;
	}

	return UDPSocketPtr(new UDPSocket(s));
}

TCPSocketPtr SocketUtil::CreateTCPSocket(SocketAddressFamily addressFamily)
{
	SOCKET s = socket(addressFamily, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET)
	{
		SocketUtil::ReportError("failed to create TCP socket");
		return nullptr;
	}

	return TCPSocketPtr(new TCPSocket(s));
}

int SocketUtil::Select(
	const std::vector<TCPSocketPtr>* inReadSockets,
	std::vector<TCPSocketPtr>* outReadSockets,
	const std::vector<TCPSocketPtr>* inWriteSockets,
	std::vector<TCPSocketPtr>* outWriteSockets,
	const std::vector<TCPSocketPtr>* inExceptSockets,
	std::vector<TCPSocketPtr>* outExceptSockets)
{
	fd_set readSet, writeSet, exceptSet;

	FillSetFromVector(inReadSockets, readSet);
	FillSetFromVector(inWriteSockets, writeSet);
	FillSetFromVector(inExceptSockets, exceptSet);

	int toRet = select(0, &readSet, &writeSet, &exceptSet, nullptr);

	if (!toRet)
	{
		return 0;
	}

	FillVectorFromSet(&readSet, inReadSockets, outReadSockets);
	FillVectorFromSet(&writeSet, inWriteSockets, outWriteSockets);
	FillVectorFromSet(&exceptSet, inExceptSockets, outExceptSockets);

	return toRet;
}

void SocketUtil::FillSetFromVector(const std::vector<TCPSocketPtr>* sockets, fd_set& outSet)
{
	if (!sockets)
	{
		return;
	}

	FD_ZERO(&outSet);
	for (const TCPSocketPtr& socket : *sockets)
	{
		FD_SET(socket->m_socket, &outSet);
	}
}

void SocketUtil::FillVectorFromSet(
	const fd_set* inSet, 
	const std::vector<TCPSocketPtr>* inSockets,
	std::vector<TCPSocketPtr>* outSockets)
{
	if (!(inSockets && outSockets))
	{
		return;
	}

	outSockets->clear();
	for (const TCPSocketPtr& socket : *inSockets)
	{
		if (FD_ISSET(socket->m_socket, &inSet))
		{
			outSockets->push_back(socket);
		}
	}
}