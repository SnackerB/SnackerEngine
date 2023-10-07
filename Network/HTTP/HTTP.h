#pragma once

#include "Network\HTTP\Request.h"
#include "Network\HTTP\Response.h"
#include "Network\TCP\TCP.h"

namespace SnackerEngine
{

	/// Class that contains logic for receiving HTTP messages of arbitrary length
	/// from a TCP socket
	class HTTPEndpoint
	{
		/// The socket through which the messages are received.
		/// Designed for non-blocking sockets, but may also work with blocking sockets.
		SocketTCP socket;
		/// Wether this Endpoint is blocking or non-blocking
		bool blocking;
		/// The current message
		std::unique_ptr<HTTPMessage> currentMessage;
		/// Buffer for saving temporary data
		Buffer tempBuffer;
		/// Offset into the temporary buffer
		std::size_t tempBufferOffset;
		/// How many bytes are currently stored in the temporary buffer
		std::size_t tempBufferSize;
		/// Remaining number of bytes still to be received for the current message
		std::size_t remainingSize;
	public:
		/// Constructor
		HTTPEndpoint(SocketTCP&& socket, bool blocking = true)
			: socket(std::move(socket)), blocking(blocking), currentMessage(nullptr), tempBuffer(4000), tempBufferOffset(0), tempBufferSize(4000), remainingSize(0) {}
		/// Tries to receive messages from the socket. Returns all received messages.
		/// wallTimeSeconds is the maximum time spent in this function before it returns.
		/// If no more messages are to be received, the function will return earlier.
		/// If 0.0 is passed as the wallTime, messages are received until no new messages
		/// are in the queue.
		std::vector<std::unique_ptr<HTTPMessage>> receiveMessages(double wallTimeSeconds);
		/// Getters
		const SocketTCP& getSocket() const { return socket; }
		SocketTCP& getSocket() { return socket; }
		/// Setters
		void setSocket(SocketTCP&& socket) { this->socket = std::move(socket); }
	};

}