#include "tcp_socket.h"

#include "socket_address.h"
#include "socket_util.h"

TCPSocket::~TCPSocket()
{
	closesocket(m_socket);
}

bool TCPSocket::Connect(const SocketAddress& address)
{
	int error = connect(m_socket, &address.m_sockAddr, (int)address.GetSize());
	if (error < 0)
	{
		SocketUtil::ReportError("connect failed: %d", SocketUtil::GetLastError());
		return false;
	}

	return true;
}

bool TCPSocket::Listen(int backLog)
{
	int error = listen(m_socket, backLog);
	if (error < 0)
	{
		SocketUtil::ReportError("listen failed: %d", SocketUtil::GetLastError());
		return false;
	}

	return true;
}

std::shared_ptr<TCPSocket> TCPSocket::Accept(SocketAddress& fromAddress)
{
	int addrLength = (int)fromAddress.GetSize();
	SOCKET newSocket = accept(m_socket, &fromAddress.m_sockAddr, &addrLength);

	if (newSocket == INVALID_SOCKET)
	{
		SocketUtil::ReportError("accept failed: %d", SocketUtil::GetLastError());
		return nullptr;
	}

	return TCPSocketPtr(new TCPSocket(newSocket));
}

bool TCPSocket::Bind(const SocketAddress& bindAddress)
{
	int error = bind(m_socket, &bindAddress.m_sockAddr, (int)bindAddress.GetSize());
	if (error != 0)
	{
		SocketUtil::ReportError("tcp bind failed: %d", SocketUtil::GetLastError());
		return false;
	}

	return true;
}

int TCPSocket::Send(const void* data, int dataLength)
{
	int bytesSent = send(m_socket, static_cast<const char*>(data), dataLength, 0);
	if (bytesSent < 0)
	{
		SocketUtil::ReportError("send failed: %d", SocketUtil::GetLastError());
	}

	return bytesSent;
}

int TCPSocket::Receive(void* buffer, int bufferSize)
{
	int bytesRead = recv(m_socket, static_cast<char *>(buffer), bufferSize, 0);
	if (bytesRead < 0)
	{
		SocketUtil::ReportError("recv failed: %d", SocketUtil::GetLastError());
	}

	return bytesRead;
}