#pragma once

#include "Network/IncomingMessage.h"
#include "Network/OutgoingMessage.h"
#include "Network/SERP.h"
#include "Network/SMP.h"

#include <array>
#include <bit>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cinttypes>
#include <memory>
#include <queue>
#include <unordered_map>

namespace SnackerEngine
{

	/// This struct contains everything the NetworkManager needs to store
	struct NetworkData
	{
		using MessageID = uint32_t;

		//==============================================================================================
		// constants
		//==============================================================================================

		/// The maximum allowed length of a single packet in bytes
		static constexpr unsigned int maxMessageLength = 4000;
		/// Socket timeout decides how long we wait when receiving a message until a timeout occurs
		static constexpr unsigned int socketTimeoutMilliseconds = 1;
		/// How many times to try to generate a port number during initialization
		static constexpr unsigned int generatePortNumberTries = 10;
		/// Timeout for sending messages in safe mode (in seconds)
		static constexpr double timeoutSendingMessagesSafeMode = 5.0;
		/// How long we wait before resending messages in safe mode
		static constexpr double durationBeforeResendSafeMode = 0.1;
		/// Timeout for incoming messages
		static constexpr double timeoutIncomingMessages = 10.0;

		/// How many bytes per second we are allowed to send
		unsigned int bytesPerSecondSend;

		//==============================================================================================
		// Addresses and IDs
		//==============================================================================================

		/// The socket file descriptor and internet address we are using
		SOCKET clientSocket;
		sockaddr_in clientAddress;
		/// The SERPID of this client
		uint16_t clientID;
		/// The internet address of the server
		sockaddr_in serverAdress;

		//==============================================================================================
		// State variables
		//==============================================================================================

		/// bool that is set if the initialization is made successfully 
		/// (setting up the socket and addresses)
		bool setup;
		/// bool that is set if a connection to the SERP server is currently made (if this is false,
		/// the clientID and serverAdress member variables should not be used)
		bool connectedToSERPServer;
		/// The messageID of the next message to be sent
		MessageID nextMessageID;
		/// accumulative time since the last send() was made. 
		/// This variable is used to make sure that we send only as many bytes per second as we are
		/// allowed to do (can be customized via the bytesPerSecond member variable)
		double accumulativeTimeSinceLastSend;

		//==============================================================================================
		// Buffers and containers
		//==============================================================================================

		/// A buffer that can be used for sending and receiving messages
		std::array<std::byte, maxMessageLength> networkBuffer;
		/// Queue of outgoing messages to be sent
		std::queue<std::unique_ptr<OutgoingMessage>> outgoingMessages;
		/// Map of outgoing messages in safe send mode, with their messageIDs as keys
		std::unordered_map<uint32_t, SafeOutgoingMessageData> safeOutgoingMessageDatas;
		/// Map of Maps of unfinished incoming messages in both send modes, with the source clientID and the 
		/// messageID as keys
		std::unordered_map<uint32_t, std::unordered_map<uint16_t, IncomingUnfinishedMessageData>> incomingUnfinishedMessageDatas;
		/// Map of finished incoming messages (used to send MESSAGE_RECEIVED messages to other clients. 
		/// all objects stored in this map will time out after a while)
		std::unordered_map<uint32_t, std::unordered_map<uint16_t, SafeIncomingMessageData>> safeIncomingMessageData;
		/// vector of all incoming messages
		std::vector<SMP_Message> incomingMessages;

		//==============================================================================================
		// Helper functions
		//==============================================================================================

		NetworkData();
		/// Sends a message that is stored in the given byte buffer and has the given messageLength in bytes
		/// to the SERP server. Returns true on success
		bool sendMessageToServer(const std::byte* buffer, unsigned int messageLength) const;
		/// Writes a singlecast message to the network buffer and sends the message to the server
		/// Returns true on success. The fields of all headers must already be set accordingly
		bool sendMessageToServerSinglecast(const SERP_Header& serpHeader, const SMP_Header& smpHeader, const std::vector<std::byte>& data);
		/// Writes a multicast message to the network buffer and sends the message to the server
		/// Returns true on success. The fields of all headers must already be set accordingly
		bool sendMessageToServerMulticast(const SERP_Header& serpHeader, const SMP_Header& smpHeader, const std::vector<std::byte>& data, const std::vector<uint16_t>& destinations);
		/// Reads a SERP Header from the network buffer
		SERP_Header readSERPHeader();
		/// Reads an SMP Header from the network buffer
		SMP_Header readSMPHeader();
		/// Reads data with the given length from the network buffer (if the buffer is large enough)
		std::vector<std::byte> readData(unsigned int numBytes);
		/// Transforms the given SMP message to a set of BasicOutgoingMessage objects and pushes them to
		/// the outgoing message queue. Returns true on success. If true is returned, the nextMessageID
		/// should be incremented
		bool createBasicOutgoingMessages(const SMP_Message& message, uint16_t destination);
		bool createBasicOutgoingMessages(const SMP_Message& message, const std::vector<uint16_t>& destinations);
		/// Handles an ADVERTISMENT message coming from the server
		void handleServerMessageAdvertisement(const SERP_Header& serpHeader, const SMP_Header& smpHeader);
		/// Handles an ECHO message coming from the server
		void handleServerMessageEcho(const SERP_Header& serpHeader, const SMP_Header& smpHeader);
		/// Handles an error message coming from the server
		void handleServerMessageError(const SERP_Header& serpHeader, const SMP_Header& smpHeader);
		/// Handles the incoming message that is currently in the networkBuffer
		void handleIncomingMessageInNetworkBuffer();
		/// Inserts the message currently in the networkBuffer into the finished message buffer.
		void insertMessageFromNetworkBufferIntoFinishedMessageBuffer(const SERP_Header& serpHeader, const SMP_Header& smpHeader);
		/// Inserts the message currently in the networkBuffer into the unfinished message buffer.
		/// If the message is completed by this part, the whole message is moved to the finished message buffer.
		/// If the message was sent in safe mode, a MESSAGE_RECEIVED is sent to the source.
		void insertMessageFromNetworkBufferIntoUnfinishedMessageBuffer(const SERP_Header& serpHeader, const SMP_Header& smpHeader);
		/// Sends a MESSAGE_RECEIVED message to the client that sent the given SERP header
		void sendMessageReceivedMessage(const SERP_Header& serpHeader);
		/// Sends a DISCONNECT message to the server
		void sendDisconnectMessageToServer();
		/// Sends a CONNECT_REQUEST message to the server
		void sendConnectionRequestToServer();
		/// Inserts the message with the given SERP Header into the safeIncomingMessageData map
		void insertIntoSafeIncomingMessagesData(const SERP_Header& serpHeader, double timeSinceFirstReceive = 0.0);

		//==============================================================================================
		// Update functions
		//==============================================================================================

		/// Updates the outgoingMessageQueue
		void updateOutgoingMessageQueue(double dt);
		/// Tries to receive a message in the buffer. Returns an empty optional if a timeout or an error occurred.
		/// Returns the length of the message on success. Make sure remoteAddrLen is properly initialized:
		/// see https://stackoverflow.com/questions/17306405/c-udp-recvfrom-is-acting-strange-wsagetlasterror-10014
		std::optional<int> recieveMessage(int sock, sockaddr& remoteAddr, int& remoteAddrLen);
		/// Updates all containers
		void updateContainers(double dt);
	

	};

}