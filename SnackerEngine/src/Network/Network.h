#pragma once

#include "Network/SMP.h"

#include <vector>
#include <memory>
#include <bit>

namespace SnackerEngine
{

	/// Static class managing all network related stuff!
	class NetworkManager
	{
	public:
		/// A single message that can be received or sent using SERP
		struct SMP_Message
		{
			uint16_t src;
			SMP_Header smpHeader;
			std::vector<std::byte> data;
			/// Constructors
			SMP_Message() = default;
			SMP_Message(uint16_t src, const SMP_Header& smpHeader, std::vector<std::byte>&& data = {})
				: src(src), smpHeader(smpHeader), data(std::move(data)) {}
			SMP_Message(const SMP_Header& smpHeader, std::vector<std::byte>&& data = {})
				: src(0), smpHeader(smpHeader), data(std::move(data)) {}
		};
		/// Initializes the network manager, opens a socket, creates an address, etc. Returns true on success
		/// and false on failure
		static bool initialize();
		/// Tries to connect to the SERP server
		static void connectToSERPServer();
		/// Returns true if we are currently connected to SERP server
		static bool isConnectedToServer();
		/// Returns the current clientID
		static uint16_t getClientID();
		/// Send a given SMP_Message using SERP. Returns true if the message was successfully sent
		static bool sendMessage(const SMP_Message& message, uint16_t destination);
		/// Send a given SMP_Message to multiple destinations at the same time.
		/// Returns true if the message was successfully sent
		static bool sendMessageMulticast(const SMP_Message& message, const std::vector<uint16_t>& destinations);
		/// Updates the NetworkManager
		static void update(double dt);
		/// Returns a vector of received messages with the given message type
		static std::vector<SMP_Message> getIncomingMessages(MESSAGE_TYPE messageType);
		/// Returns a vector of all received messages
		static std::vector<SMP_Message> getIncomingMessages();
		/// Sets the allowed bytes per seconds for sending messages
		static void setBytesPerSecondsSend(unsigned int bytesPerSecond);
		/// Cleans up the manager, closes all sockets etc. Should be called when the Engine terminates
		/// or when the network capabilities are no longer used.
		static void cleanup();
	};

}