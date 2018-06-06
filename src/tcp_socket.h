#pragma once

#include <memory>
#include <WinSock2.h>

class SocketAddress;

class TCPSocket
{
public:
	~TCPSocket();

	bool Connect(const SocketAddress& address);
	bool Listen(int backLog = 32);
	std::shared_ptr<TCPSocket> Accept(SocketAddress& fromAddress);
	bool Bind(const SocketAddress& toAddress);
	int Send(const void* data, int dataLength);
	int Receive(void* buffer, int bufferSize);

private:
	friend class SocketUtil;

	SOCKET m_socket;

	TCPSocket(SOCKET socket) : m_socket(socket) {}
};

typedef std::shared_ptr<TCPSocket> TCPSocketPtr;