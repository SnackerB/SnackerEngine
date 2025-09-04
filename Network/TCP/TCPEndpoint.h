#pragma once
#include "Utility/Buffer.h"
#include "TCP.h"

namespace SnackerEngine
{

	class TCPEndpoint
	{
		/// A TCP Endpoint handles a single socket through which data can be received and sent.
		/// Designed for non-blocking sockets, but may also work with blocking sockets.
		SocketTCP socket;
		/// Buffer for saving temporary data
		Buffer storageBuffer;
	public:
		/// Constructor
		TCPEndpoint(int port = 0);
		TCPEndpoint(SocketTCP socket);
		/// Resets the socket. Kills active connection.
		void resetSocket(int port = 0);
		/// Tries to connect to the SERP server. Can be called repeatedly until the answor is either
		/// SUCCESS or ERROR.
		ConnectResult connectToSERPServer();
		/// Tries to receive data from the socket. Returns a buffer containing the received data.
		/// wallTimeSeconds is the maximum time spent in this function before it returns.
		/// If no more data is to be received, the function will return earlier.
		/// If 0.0 is passed as the wallTime, data is received until no new data is in the queue.
		Buffer receiveData();
		/// Tries to send data through the socket. Returns the number of bytes sent.
		std::size_t sendData(ConstantBufferView data);
		/// Getters
		const SocketTCP& getSocket() const { return socket; }
		SocketTCP& getSocket() { return socket; }
		/// Setters
		void setSocket(SocketTCP&& socket) { this->socket = std::move(socket); }
	};

}