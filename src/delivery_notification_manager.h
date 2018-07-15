#pragma once

#include "bit_streams.h"

#include <deque>
#include <memory>
#include <stdint.h>
#include <unordered_map>

typedef uint16_t packetSequenceNumber_t;

class DeliveryNotificationManager;

typedef int deliveryNotificationHandlerKey_t;

class DeliveryNotificationHandler
{
public:
	virtual void HandleDeliverySuccess(DeliveryNotificationManager* dnm) const = 0;
	virtual void HandleDeliveryFailure(DeliveryNotificationManager* dnm) const = 0;
};
typedef std::shared_ptr<DeliveryNotificationHandler> DeliveryNotificationHandlerPtr;

class InFlightPacket
{
public:
	InFlightPacket(packetSequenceNumber_t sequenceNumber);

	packetSequenceNumber_t GetSequenceNumber() const { return m_sequenceNumber; }
	uint64_t GetTimeDispatched() const { return m_timeDispatched; }

	void SetNotificationHandler(deliveryNotificationHandlerKey_t key, DeliveryNotificationHandlerPtr handler);
	DeliveryNotificationHandlerPtr GetNotificationHandler(deliveryNotificationHandlerKey_t key);

	void HandleDeliverySuccess(DeliveryNotificationManager* dnm) const;
	void HandleDeliveryFailure(DeliveryNotificationManager* dnm) const;

private:
	packetSequenceNumber_t m_sequenceNumber;
	uint64_t m_timeDispatched;

	// We have to iterate over these quite a bit, so an unordered map doesn't make sense
	// as it will result in a lot of cache misses.  Should consider using a sorted vector
	// or if we only have a few transmission types just have a dedicated member variable for each
	std::unordered_map<deliveryNotificationHandlerKey_t, DeliveryNotificationHandlerPtr> m_notificationHandlerMap;
};

class AckRange
{
public:
	AckRange() :
		m_start(0),
		m_count(0)
	{}

	AckRange(packetSequenceNumber_t start) :
		m_start(start),
		m_count(1)
	{}

	bool TryExtendRange(packetSequenceNumber_t sequenceNumber);

	packetSequenceNumber_t GetStart() { return m_start; }
	uint32_t GetCount() { return m_count; }

	void Write(OutputMemoryBitStream& packet);
	void Read(InputMemoryBitStream& packet);

private:
	packetSequenceNumber_t m_start;
	uint32_t m_count;
};

class DeliveryNotificationManager
{
public:
	DeliveryNotificationManager() : 
		m_nextOutgoingSequenceNumber(0),
		m_nextExpectedIncomingSequenceNumber(0),
		m_dispatchedPacketCount(0),
		m_droppedPacketCount(0),
		m_deliveredPacketCount(0)
	{}

	InFlightPacket* WriteSequenceNumber(OutputMemoryBitStream& packet);
	
	// Return value indicates whether application should process the packet
	bool ProcessSequenceNumber(InputMemoryBitStream& packet);

	void WritePendingAcks(OutputMemoryBitStream& packet);

	void ProcessAcks(InputMemoryBitStream& packet);

	void ProcessTimedOutPackets();

private:
	static const int kAckTimeout = 30000;

	packetSequenceNumber_t m_nextOutgoingSequenceNumber;
	packetSequenceNumber_t m_nextExpectedIncomingSequenceNumber;
	uint32_t m_dispatchedPacketCount;
	std::deque<InFlightPacket> m_inFlightPackets;
	std::deque<AckRange> m_pendingAcks;
	uint32_t m_droppedPacketCount;
	uint32_t m_deliveredPacketCount;

	void AddPendingAck(packetSequenceNumber_t sequenceNumber);

	void HandlePacketDeliverySuccess(const InFlightPacket& packet);
	void HandlePacketDeliveryFailure(const InFlightPacket& packet);
};