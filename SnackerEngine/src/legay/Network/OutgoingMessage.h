#pragma once

#include "Network/SMP.h"

#include <vector>
#include <bit>
#include <optional>
#include <cinttypes>
#include <unordered_map>

namespace SnackerEngine
{

	/// Forward declaration of NetworkData
	struct NetworkData;
	/// Forward declaration of SMP_Message
	struct SMP_Message;

	/// This class represents a message that is sent from the client to the server or to another client.
	/// unique pointers to OutgoingMessage objects are stored in a queue and sent out one after the other
	/// when the update() function of the NetworkManager is called.
	struct OutgoingMessage
	{
	protected:
		/// Protected constructor
		OutgoingMessage() {};
	public:
		/// Actually sends the Message, if its message length is smaller than the given length (in bytes)
		/// Returns the messageLength if the message was sent, else returns an empty optional.
		virtual std::optional<unsigned int> send(NetworkData& networkData, unsigned int maxLengthBytes) { return false; };
	};

	/// A basic outgoing message that just stores the message itself. After it is sent, the message
	/// is not stored. This should be used for fast-paced messages where it is not that bad if a single
	/// message gets lost.
	struct BasicOutgoingMessage : public OutgoingMessage
	{
		/// The data of the message, stored as a byte vector
 		std::vector<std::byte> data;
		/// Actually sends the Message, if its message length is smaller than the given length (in bytes)
		/// Returns the messageLength if the message was sent, else returns an empty optional.
		virtual std::optional<unsigned int> send(NetworkData& networkData, unsigned int maxLengthBytes) override;
		/// Constructors
		BasicOutgoingMessage() = default;
	};

	/// An outgoing message that is used for the safe send mode. The object doesn't store the message
	/// itself, but only the messageID. Additionally, the part
	/// index of the message is stored (for messages consisting of multiple packets)
	struct SafeOutgoingMessage : public OutgoingMessage
	{
		/// The messageID of the message
		uint32_t messageID;
		/// the part of the message that should be sent (is ignored for single-packet messages)
		uint16_t messagePart;
		/// Constructor
		SafeOutgoingMessage(uint32_t messageID, uint16_t messagePart)
			: messageID(messageID), messagePart(messagePart) {}
		/// Actually sends the Message, if its message length is smaller than the given length (in bytes)
		/// Returns the messageLength if the message was sent, else returns an empty optional.
		virtual std::optional<unsigned int> send(NetworkData& networkData, unsigned int maxLengthBytes) override;
	};

	/// Represents a outgoing message in safe mode. Supports functionality to receive MESSAGE_RECEIVED messages 
	/// from other clients.
	struct SafeOutgoingMessageData
	{
	public:
		/// The time that elapsed since the message was first sent
		double timeSinceFirstSend;
		/// The time that elapsed since parts of the message were last sent
		double timeSinceLastSend;
		/// The messageID
		uint32_t messageID;
		/// The flags that are set for this message
		uint32_t flags;
		/// The SMP_Header of the message
		SMP_Header smpHeader;
		/// Vector of destination clients
		std::vector<uint16_t> destinations;
		/// Vector containing the data parts of all packets to be sent
		std::vector<std::vector<std::byte>> dataParts;
		/// Vector containing booleans for checking if a given client has received the given dataPart.
		/// The first entry of the pair counts the number of clients who have yet to receive the dataPart.
		std::vector<std::pair<int, std::vector<bool>>> receivedDataPart;
		/// Integer that counts the number of dataParts that are still not received by all clients
		int dataPartsNotReceivedCount;
		/// Vector that stores for each client the number of packets they have not yet received
		std::vector<int> dataPartsNotReceivedCountPerClient;
		/// Map that maps a clientID to an index into the receivedDataPart vector
		std::unordered_map<uint16_t, std::size_t> clientIDToIndex;
		/// Creates a new SafeOutgoingMessage. If this is successful, the nextMessageID of the networkData
		/// should be incremented!
		static std::optional<SafeOutgoingMessageData> CreateSafeOutgoingMessageData(const NetworkData& networkData, const SMP_Message& message, const std::vector<uint16_t>& destinations);
		/// Notifies this MessageData that the given client has received the given messagePart
		void notifyReceivedMessagePart(uint16_t clientID, uint16_t messagePartIndex);
		/// Sends a given message part to all clients who have not received it yet.
		/// Returns the size of the sent message in byte if everythin went right.
		/// False is returned when the messageLength was larger than the given maxMessageLength.
		/// If maxMessageLength is set to 0 it is ignored.
		/// The only member of networkData that is changed in this function call is the networkBuffer.
		std::optional<unsigned int> sendMessagePart(uint16_t messagePartIndex, NetworkData& networkData, unsigned int maxMessageLength = 0);
		/// Inserts packets to be sent into the outgoingMessageQueue. This will iterate through the dataParts
		/// vector and send out all packets that haven't been received by all clients yet. Sends only up to
		/// sendMaxPackets packets. If sendMaxPackets is set to 0, this function will send as many packets 
		/// as possible. The only member of networkData that is changed in this function call is the oputgoingMessageQueue.
		void pushPacketsToSendQueue(NetworkData& networkData, unsigned int sendMaxPackets = 0);
		/// Default constructor
		SafeOutgoingMessageData();
	};

}