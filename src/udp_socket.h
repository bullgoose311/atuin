#pragma once

#include "socket_address.h"

#include <memory>

class UDPSocket
{
public:
	~UDPSocket();
	bool Bind(const SocketAddress& bindAddress);
	int SendTo(const void* data, int len, const SocketAddress& to);
	int ReceiveFrom(void* buffer, int bufferLength, SocketAddress& outFrom);

private:
	friend class SocketUtil;

	SOCKET m_socket;

	UDPSocket(SOCKET socket) : m_socket(socket) {}
};

typedef std::shared_ptr<UDPSocket> UDPSocketPtr;