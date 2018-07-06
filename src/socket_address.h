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

	bool operator==(const SocketAddress& other) const
	{
		return (m_sockAddr.sa_family == AF_INET &&
			GetAsSockAddrIn()->sin_port == other.GetAsSockAddrIn()->sin_port) &&
			(GetIP4Ref() == other.GetIP4Ref());
	}

	// Needed so that we can use SocketAddress as a key in a std::map
	size_t GetHash() const
	{
		return (GetIP4Ref()) |
			((static_cast< uint32_t >(GetAsSockAddrIn()->sin_port)) << 13) |
			m_sockAddr.sa_family;
	}

private:
	friend class UDPSocket;
	friend class TCPSocket;

	sockaddr m_sockAddr;

	void Init(uint32_t address, uint16_t port);

#if _WIN32
	uint32_t&				GetIP4Ref() { return *reinterpret_cast< uint32_t* >(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
	const uint32_t&			GetIP4Ref()			const { return *reinterpret_cast< const uint32_t* >(&GetAsSockAddrIn()->sin_addr.S_un.S_addr); }
#else
	uint32_t&				GetIP4Ref() { return GetAsSockAddrIn()->sin_addr.s_addr; }
	const uint32_t&			GetIP4Ref()			const { return GetAsSockAddrIn()->sin_addr.s_addr; }
#endif

	sockaddr_in*			GetAsSockAddrIn() { return reinterpret_cast< sockaddr_in* >(&m_sockAddr); }
	const	sockaddr_in*	GetAsSockAddrIn()	const { return reinterpret_cast< const sockaddr_in* >(&m_sockAddr); }
};

// Needed so that we can use SocketAddress as a key in a std::map
namespace std
{
	template<> struct hash< SocketAddress >
	{
		size_t operator()(const SocketAddress& address) const
		{
			return address.GetHash();
		}
	};
}

typedef std::shared_ptr<SocketAddress> SocketAddressPtr;

class SocketAddressFactory
{
public:
	static SocketAddressPtr CreateIPv4SocketAddress(const std::string& address);
};