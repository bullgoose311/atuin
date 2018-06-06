#pragma once

#include "tcp_socket.h"
#include "udp_socket.h"

#include <vector>
#include <WinSock2.h>

enum SocketAddressFamily
{
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil
{
public:
	static int GetLastError();
	static void ReportError(const char* fmt, ...);
	static UDPSocketPtr CreateUDPSocket(SocketAddressFamily addressFamily);
	static TCPSocketPtr CreateTCPSocket(SocketAddressFamily addressFamily);
	static int Select(
		const std::vector<TCPSocketPtr>* inReadSockets,
		std::vector<TCPSocketPtr>* outReadSockets,
		const std::vector<TCPSocketPtr>* inWriteSockets,
		std::vector<TCPSocketPtr>* outWriteSockets,
		const std::vector<TCPSocketPtr>* inExceptSockets,
		std::vector<TCPSocketPtr>* outExceptSockets);

private:
	static void FillSetFromVector(const std::vector<TCPSocketPtr>* inSockets, fd_set& outSet);
	static void FillVectorFromSet(const fd_set* inSet, const std::vector<TCPSocketPtr>* inSockets, std::vector<TCPSocketPtr>* outSockets);
};