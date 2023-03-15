#pragma once

#include "Network/SMP.h"

#include <vector>

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
			std::vector<uint8_t> data;
		};
		/// Initializes the network manager, opens a socket, creates an address, etc. Returns true on success
		/// and false on failure
		static bool initialize();
		/// Tries to connect to the SERP server
		static void connectToSERPServer();
		/// Returns true if we are currently connected to SERP server
		static bool isConnectedToServer();
		/// Send a given SMP_Message using SERP. Returns true if the message was successfully sent
		static bool sendMessage(const SMP_Message& message, uint16_t destination);
		/// Send a given SMP_Message to multiple destinations at the same time.
		/// Returns true if the message was successfully sent
		static bool sendMessageMulticast(const SMP_Message& message, const std::vector<uint16_t>& destinations);
		/// Updates the NetworkManager
		static void update(double dt);
		/// Cleans up the manager, closes all sockets etc. Should be called when the Engine terminates
		/// or when the network capabilities are no longer used.
		static void cleanup();
	};

}