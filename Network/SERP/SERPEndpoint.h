#pragma once

#include "Network/TCP/TCPEndpoint.h"
#include "SERP.h"
#include <queue>

namespace SnackerEngine
{

	class SERPEndpoint
	{
		/// MessageID for the next message
		uint32_t nextMessageID = 0;
		/// The TCP Endpoint through which messages are received and sent
		TCPEndpoint endpointTCP{};
		/// Header of an unfinished to be received message that is yet to be fully parsed
		std::optional<SERPHeader> tempMessageHeader = std::nullopt;
		/// Buffer storing the content of the temporary message to be received
		Buffer tempMessageBuffer{};
		/// StorageBuffer used for storing partial messages
		Buffer storageBuffer{};
		/// Bytes left to parse for the temporary message
		std::size_t tempMessageBytesLeftToParse = 0;
		/// Queue of messages to be sent
		std::queue<Buffer> messagesToBeSent{};
		/// Number of bytes of the current messages that were already sent
		std::size_t sentBytes{ 0 };
		/// Helper function that parses as many messages as possible from the given buffer. Returns pointers to all fully parsed messages.
		/// If a message is only partially obtained, it is placed in the temporaryMessage member variable. If at any point an error occurs,
		/// eg. an invalid header is parsed, the parsing is aborted and all messages that were parsed up to this point are returned.
		/// After this function is called, the buffer can be emptied as all messages have been extracted. If at the end of parsing there remains
		/// a buffer that is smaller than a SERPHeader, the buffer is returned and must be saved for the next call of this function.
		std::pair<std::vector<std::unique_ptr<SERPMessage>>, ConstantBufferView> parseMessages(ConstantBufferView buffer);
	public:
		/// Constructor
		SERPEndpoint() = default;
		SERPEndpoint(SocketTCP socket)
			: endpointTCP(std::move(socket)) {}
		/// Resets the socket. Kills active connection.
		void resetSocket() { endpointTCP.resetSocket(); };
		/// Returns a const reference to the underlying TCP Endpoint
		TCPEndpoint& getTCPEndpoint() { return endpointTCP; };
		/// Tries to connect to SERP 
		ConnectResult connectToSERPServer() { return endpointTCP.connectToSERPServer(); }
		/// Tries to receive messages through the endpoint
		std::vector<std::unique_ptr<SERPMessage>> receiveMessages();
		/// Finalizes and sends a given message
		void finalizeAndSendMessage(SERPMessage& message, bool setMessageID = true);
		/// Finalizes a message, but doesn't send it.
		void finalizeMessage(SERPMessage& message, bool setMessageID = true);
		/// sends a message (message should already be finalized)
		void sendMessage(SERPMessage& message);
		/// Returns true if there are still (partly) unsent messages in the queue, in which case updateSend() should be called
		bool hasUnsentMessages() { return !messagesToBeSent.empty(); }
		/// Performs one unblocking send call on the first message in the messagesToBeSent queue. Should be called regularly.
		void updateSend();
	};

}