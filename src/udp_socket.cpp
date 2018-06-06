#include "udp_socket.h"

#include "socket_address.h"
#include "socket_util.h"

UDPSocket::~UDPSocket()
{
	closesocket(m_socket);
}

bool UDPSocket::Bind(const SocketAddress& bindAddress)
{
	int error = bind(m_socket, &bindAddress.m_sockAddr, (int)bindAddress.GetSize());
	if (error != 0)
	{
		SocketUtil::ReportError("udp bind failed: %d", SocketUtil::GetLastError());
		return false;
	}

	return true;
}

int UDPSocket::SendTo(const void* data, int dataSize, const SocketAddress& toAddr)
{
	int bytesSent = sendto(m_socket, static_cast<const char*>(data), dataSize, 0, &toAddr.m_sockAddr, (int)toAddr.GetSize());

	if (bytesSent >= 0)
	{
		SocketUtil::ReportError("sendto failed: %d", SocketUtil::GetLastError());
		return SOCKET_ERROR;
	}

	return bytesSent;
}

int UDPSocket::ReceiveFrom(void* buffer, int bufferSize, SocketAddress& outFrom)
{
	int fromLength = (int)outFrom.GetSize();
	
	int bytesRead = recvfrom(m_socket, static_cast<char*>(buffer), bufferSize, 0, &outFrom.m_sockAddr, &fromLength);

	if (bytesRead < 0)
	{
		SocketUtil::ReportError("recvfrom failed: %d", SocketUtil::GetLastError());
		return SOCKET_ERROR;
	}

	return bytesRead;
}