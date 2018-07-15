#include "delivery_notification_manager.h"

#include "timing_utils.h"

void DeliveryNotificationHandler::HandleDeliverySuccess(DeliveryNotificationManager* dnm) const
{
}

void DeliveryNotificationHandler::HandleDeliveryFailure(DeliveryNotificationManager* dnm) const
{
}

InFlightPacket::InFlightPacket(packetSequenceNumber_t sequenceNumber) :
	m_sequenceNumber(sequenceNumber),
	m_timeDispatched(Timing::Get().GetTimeMS())
{
}

void InFlightPacket::SetNotificationHandler(deliveryNotificationHandlerKey_t key, DeliveryNotificationHandlerPtr handler)
{
	m_notificationHandlerMap[key] = handler;
}

DeliveryNotificationHandlerPtr InFlightPacket::GetNotificationHandler(deliveryNotificationHandlerKey_t key)
{
	auto it = m_notificationHandlerMap.find(key);
	return it != m_notificationHandlerMap.end() ? it->second : nullptr;
}

void InFlightPacket::HandleDeliverySuccess(DeliveryNotificationManager* dnm) const
{
	for (const auto& pair : m_notificationHandlerMap)
	{
		pair.second->HandleDeliverySuccess(dnm);
	}
}

void InFlightPacket::HandleDeliveryFailure(DeliveryNotificationManager* dnm) const
{
	for (const auto& pair : m_notificationHandlerMap)
	{
		pair.second->HandleDeliveryFailure(dnm);
	}
}

bool AckRange::TryExtendRange(packetSequenceNumber_t sequenceNumber)
{
	if (sequenceNumber == m_start + m_count)
	{
		++m_count;
		return true;
	}

	return false;
}

void AckRange::Write(OutputMemoryBitStream& packet)
{
	packet.Write(m_start);
	bool hasCount = m_count > 1;
	packet.Write(hasCount);
	if (hasCount)
	{
		// Assuming we'll never surpass 255 sequential acks so we can use 8 bits
		uint32_t countMinusOne = m_count - 1;
		uint8_t countToAck = countMinusOne > 255 ? 255 : static_cast<uint8_t>(countMinusOne);
		packet.Write(countToAck);
	}
}

void AckRange::Read(InputMemoryBitStream& packet)
{
	packet.Read(m_start);
	bool hasCount;
	packet.Read(hasCount);
	if (hasCount)
	{
		packet.Read(m_count);
	}
	else
	{
		m_count = 1;
	}
}

InFlightPacket* DeliveryNotificationManager::WriteSequenceNumber(OutputMemoryBitStream& packet)
{
	packetSequenceNumber_t sequenceNumber = m_nextOutgoingSequenceNumber++;

	packet.Write(sequenceNumber);

	++m_dispatchedPacketCount;

	m_inFlightPackets.emplace_back(sequenceNumber);

	return &m_inFlightPackets.back();
}

bool DeliveryNotificationManager::ProcessSequenceNumber(InputMemoryBitStream& packet)
{
	packetSequenceNumber_t sequenceNumber;
	packet.Read(sequenceNumber);
	if (sequenceNumber == m_nextExpectedIncomingSequenceNumber)
	{
		m_nextExpectedIncomingSequenceNumber = sequenceNumber + 1;
		AddPendingAck(sequenceNumber);
		return true;
	}
	else if (sequenceNumber < m_nextExpectedIncomingSequenceNumber)
	{
		// We've already processed a packet newer than this one, so to prevent out of order processing we have to silently drop it
		return false;
	}
	else if (sequenceNumber > m_nextExpectedIncomingSequenceNumber)
	{
		// Some packets were dropped or have been delayed, but because the DNM only guarantees
		// to not process things out of order it can behave exactly the same was as the first case above
		m_nextExpectedIncomingSequenceNumber = sequenceNumber + 1;
		AddPendingAck(sequenceNumber);
		return true;
	}

	return false;
}

void DeliveryNotificationManager::WritePendingAcks(OutputMemoryBitStream& packet)
{
	bool haveAcks = m_pendingAcks.size() > 0;
	packet.Write(haveAcks);
	if (haveAcks)
	{
		// We're assuming that there should only ever be a single ack range (no packet loss)
		// Otherwise we'd have to indicate how many ack ranges there are, possibly bloating the packet
		m_pendingAcks.front().Write(packet);
		m_pendingAcks.pop_front();
	}
}

void DeliveryNotificationManager::ProcessAcks(InputMemoryBitStream& packet)
{
	// Because our in-flight packets are already ordered by sequence number,
	// we can start at the beginning and assume all packets we find before the start of the
	// range have been dropped

	bool hasAcks;
	packet.Read(hasAcks);
	if (!hasAcks)
	{
		return;
	}

	AckRange ackRange;
	ackRange.Read(packet);
	packetSequenceNumber_t nextAckdSequenceNumber = ackRange.GetStart();
	uint32_t onePastAckRange = nextAckdSequenceNumber + ackRange.GetCount();

	while (nextAckdSequenceNumber < onePastAckRange && !m_inFlightPackets.empty())
	{
		const InFlightPacket& nextInFlightPacket = m_inFlightPackets.front();
		if (nextInFlightPacket.GetSequenceNumber() < nextAckdSequenceNumber)
		{
			// We're receiving an ack for a sequence number greater than this inflight packet
			// This most likely means the packet was lost

			// Take a copy and remove it before processing so that upstream modules don't see it as a live packet
			InFlightPacket copyOfInFlightPacket = nextInFlightPacket;
			m_inFlightPackets.pop_front();
			HandlePacketDeliveryFailure(copyOfInFlightPacket);
		}
		else if (nextInFlightPacket.GetSequenceNumber() == nextAckdSequenceNumber)
		{
			HandlePacketDeliverySuccess(nextInFlightPacket);
			m_inFlightPackets.pop_front();
			++nextAckdSequenceNumber;
		}
		else if (nextInFlightPacket.GetSequenceNumber() > nextAckdSequenceNumber)
		{
			// indicates that some in-flight packets had been timed out previous to this one
			// so let's just jump to this packets sequence number in the range and mark the
			// rest of them as ack'd
			nextAckdSequenceNumber = nextInFlightPacket.GetSequenceNumber();
		}
	}
}

void DeliveryNotificationManager::ProcessTimedOutPackets()
{
	// Because packets are already sorted by dispatch time, we don't need to traverse
	// the entire list

	uint64_t timeoutTimestamp = Timing::Get().GetTimeMS() - kAckTimeout;
	while (!m_inFlightPackets.empty())
	{
		const InFlightPacket& nextInFlightPacket = m_inFlightPackets.front();
		if (nextInFlightPacket.GetTimeDispatched() < timeoutTimestamp)
		{
			HandlePacketDeliveryFailure(nextInFlightPacket);
			m_inFlightPackets.pop_front();
		}
		else
		{
			break;
		}
	}
}

void DeliveryNotificationManager::AddPendingAck(packetSequenceNumber_t sequenceNumber)
{
	if (m_pendingAcks.size() == 0 || !m_pendingAcks.back().TryExtendRange(sequenceNumber))
	{
		m_pendingAcks.emplace_back(sequenceNumber);
	}
}

void DeliveryNotificationManager::HandlePacketDeliverySuccess(const InFlightPacket& inFlightPacket)
{
	++m_deliveredPacketCount;
	inFlightPacket.HandleDeliverySuccess(this);
}

void DeliveryNotificationManager::HandlePacketDeliveryFailure(const InFlightPacket& inFlightPacket)
{
	++m_droppedPacketCount;
	inFlightPacket.HandleDeliveryFailure(this);
}