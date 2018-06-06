#pragma once

#include <memory>
#include <stdint.h>
#include <string>
#include <WinSock2.h>

class SocketAddress
{
public:
	SocketAddress();
	SocketAddress(uint16_t port);
	SocketAddress(uint32_t address, uint16_t port);
	SocketAddress(const char* address, uint16_t port);
	SocketAddress(const sockaddr& sockAddr);

	size_t GetSize() const { return sizeof(m_sockAddr); }

private:
	friend class UDPSocket;
	friend class TCPSocket;

	sockaddr m_sockAddr;

	void Init(uint32_t address, uint16_t port);
	inline sockaddr_in* GetAsSockAddrIn() { return reinterpret_cast<sockaddr_in*>(&m_sockAddr); }
};

typedef std::shared_ptr<SocketAddress> SocketAddressPtr;

class SocketAddressFactory
{
public:
	static SocketAddressPtr CreateIPv4SocketAddress(const std::string& address);
};