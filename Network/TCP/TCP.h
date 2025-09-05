#pragma once

#include <optional>
#include <memory>

#include "Utility/Buffer.h"

#ifdef _WINDOWS
#include <winsock2.h>
#endif
#ifdef _LINUX
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#endif

#ifdef ERROR
#undef ERROR
#endif

namespace SnackerEngine
{

	struct SocketTCP {
#ifdef _WINDOWS
		SOCKET sock;
#endif
#ifdef _LINUX
		int sock;
#endif
		sockaddr_in addr;
		bool valid;
		/// Constructor
		SocketTCP();
		/// Destructor
		~SocketTCP();
		/// Copy constructor and assignment operator
		SocketTCP(const SocketTCP& other) noexcept = delete;
		SocketTCP& operator=(const SocketTCP& other) noexcept = delete;
		// Default move constructor and assignment operator
		SocketTCP(SocketTCP&& other) noexcept;
		SocketTCP& operator=(SocketTCP&& other) noexcept;
	};

	/// Returns the port number of the SERP server in host byte order
	int getSERPServerPort();
	/// Returns the port number of the SERP server incomingDataRequest socket in host byte order
	int getSERPServerDataPort();
	/// Returns the address of the SERP server
	sockaddr_in getSERPServerAdressTCP();

	/// Compares two sockaddr_in and returns true when they are equal
	bool compare(const sockaddr_in& addr1, const sockaddr_in& addr2);

	/// Creates a blocking TCP socket and binds it to the given port.
	/// If port is set to zero, the system will chose the port number.
	std::optional<SocketTCP> createSocketTCP(int port = 0);

	/// Sets the given socket to non-blocking. Returns true on success
	bool setToNonBlocking(SocketTCP& socket);

	/// Marks the given socket as "listening", such that it can be used to
	/// accept incoming connection requests
	bool markAsListen(SocketTCP& socket, int backlog_queue_size = 10);

	/// Tries to connect the given socket to the given address
	/// Returns true on success. This is a blocking operation
	bool connectTo(SocketTCP& socket, const sockaddr_in& addr);

	/// Struct for the result of a non blocking connect
	struct ConnectResult
	{
		enum class Result
		{
			SUCCESS,
			ERROR,
			PENDING,
		};
		Result result;
		int error;
	};

	/// Tries to connect the given socket to the given address
	ConnectResult connectToNonBlocking(SocketTCP& socket, const sockaddr_in& addr);

	/// Tries to send data to through the given socket.
	/// This is a blocking operation
	bool sendTo(const SocketTCP& socket, ConstantBufferView buffer);

	/// Tries to send data through the given socket, without blocking.
	/// Returns the number of bytes sent
	std::size_t sendToNonBlocking(const SocketTCP& socket, ConstantBufferView buffer);

	/// struct returned by receiveFrom()
	struct ReceiveFromResult
	{
		std::optional<Buffer> buffer;
		std::optional<int> error;
	};

	/// Tries to receive data from the given socket.
	/// This is a blocking operation. Data will first be transferred
	/// to the intermediate storageBuffer.
	ReceiveFromResult receiveFrom(const SocketTCP& socket, BufferView storageBuffer);

	/// Tries to receive data from the given socket, without blocking.
	/// Data will first be transferred to the intermediate storageBuffer.
	ReceiveFromResult receiveFromNonBlocking(const SocketTCP& socket, BufferView storageBuffer);

	/// Accepts a connection request on the given socket. The given socket should
	/// be marked as listening. If currently no request is present, an empty optional
	/// is returned. If a request is present, the returned TCP socket can be used to
	/// communicate over the newly created connection
	std::optional<SocketTCP> acceptConnectionRequest(SocketTCP& socket);

}