#pragma once

#include "SERP/SERP.h"
#include "TCP/TCP.h"

namespace SnackerEngine
{

	/// A SERPManager represents the Engines connection with the network. It is used to communicate with other clients 
	/// running SnackerEngine over the SERP server
	class SERPManager
	{
	private:
		/// Wether the SERP manager is connected to the SERP server
		bool connected = false;
		/// The socket that is used to communicate with the server
		SnackerNet::SocketTCP socket;
	public:
		/// Constructor
		SERPManager();
		/// Tries to connect to the SERP server
		void connectToSERPServer();

	};

}