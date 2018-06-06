#include "socket_address.h"

#include "socket_util.h"

#include <WS2tcpip.h> // InetPton

SocketAddress::SocketAddress()
{
	// Used when creating socket addresses for incoming clients
}

SocketAddress::SocketAddress(uint16_t port)
{
	// Bind to all NICs
	Init(INADDR_ANY, port);
}

SocketAddress::SocketAddress(uint32_t address, uint16_t port)
{
	// Bind to specific NIC
	Init(address, port);
}

SocketAddress::SocketAddress(const char* address, uint16_t port)
{
	// Bind to specific NIC
	uint32_t iAddress;
	InetPton(AF_INET, address, &iAddress);
	Init(iAddress, port);
}

SocketAddress::SocketAddress(const sockaddr& sockAddr)
{
	memcpy(&m_sockAddr, &sockAddr, sizeof(sockAddr));
}

void SocketAddress::Init(uint32_t address, uint16_t port)
{
	GetAsSockAddrIn()->sin_family = AF_INET;
	GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(address);
	GetAsSockAddrIn()->sin_port = htons(port);
}

SocketAddressPtr SocketAddressFactory::CreateIPv4SocketAddress(const std::string& address)
{
	size_t pos = address.find_last_of(':');
	std::string host, service;
	if (pos != std::string::npos)
	{
		host = address.substr(0, pos);
		service = address.substr(pos + 1);
	}
	else
	{
		host = address;
		service = "0";
	}

	addrinfo hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;

	addrinfo* result;
	int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);

	addrinfo *initResult = result;
	if (error != 0)
	{
		SocketUtil::ReportError("getaddrinfo failed with return code %d for address %s", error, address.c_str());
		freeaddrinfo(initResult);
		return nullptr;
	}

	while (!result->ai_addr && result->ai_next)
	{
		result = result->ai_next;
	}

	if (!result->ai_addr)
	{
		SocketUtil::ReportError("no ai addresses found for address %s", address);
		freeaddrinfo(initResult);
		return nullptr;
	}

	SocketAddressPtr socketAddressPtr = std::make_shared<SocketAddress>(*result->ai_addr);

	freeaddrinfo(initResult);

	return socketAddressPtr;
}