#pragma once

#include "Network/SMP.h"

#include <vector>
#include <bit>
#include <cinttypes>
#include <unordered_map>
#include <optional>

namespace SnackerEngine
{

	/// Forward declaration of network data
	struct NetworkData;
	/// Forward declaration of SMP_Message
	struct SMP_Message;

	/// Represents an unfinished incoming message in not safe mode.
	struct IncomingUnfinishedMessageData
	{
		/// Time that gets counted up during updates. When it is larger than the timeout,
		/// this can be deleted.
		double timeSinceFirstReceive;
		/// The messageID
		uint32_t messageID;
		/// The sourceID of this message
		uint16_t src;
		/// The flags that are set for this message
		uint32_t flags;
		/// The SMP_Header of the message
		SMP_Header smpHeader;
		/// Vector containing the data parts of all packets to be received
		std::vector<std::vector<std::byte>> dataParts;
		/// Integer that counts the number of data parts that are still not received
		int dataPartsNotReceivedCount;
		/// Receives the given dataPart. data should be the pure data vector, ie. not containing any headers
		virtual void receiveDataPart(uint16_t dataPart, const std::byte* data, unsigned int dataLengthBytes);
		/// Constructs a SMP_Message from this incoming message. Should only
		/// be called when the message was fully received, ie. when dataPartsNotReceivedCount is zero!
		virtual SMP_Message constructSMP_Message();
		/// Constructors
		IncomingUnfinishedMessageData() = default;
		IncomingUnfinishedMessageData(uint32_t messageID, uint16_t src, uint32_t flags, SMP_Header smpHeader, uint16_t total);
	};

	/// Represents an finished incoming message in safe mode. This just stores the timeout (finished messages are kept a while,
	/// such that incoming messageParts that were already received can be answered with a MESSAGE_RECEIVED message)
	struct SafeIncomingMessageData
	{
		/// Time that gets counted up during updates. When it is larger than the timeout,
		/// this can be deleted.
		double timeSinceFirstReceive;
	};

}