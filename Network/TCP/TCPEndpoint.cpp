#include "TCPEndpoint.h"

namespace SnackerEngine
{

	TCPEndpoint::TCPEndpoint(int port)
		: socket(), storageBuffer(10000)
	{
		resetSocket(port);
	}

	TCPEndpoint::TCPEndpoint(SocketTCP socket)
		: socket(std::move(socket)), storageBuffer(10000)
	{
	}

	void TCPEndpoint::resetSocket(int port)
	{
		std::optional<SocketTCP> result = createSocketTCP(port);
		if (result.has_value()) {
			socket = std::move(result.value());
			setToNonBlocking(socket);
		}
	}

	ConnectResult TCPEndpoint::connectToSERPServer()
	{
		return connectToNonBlocking(socket, getSERPServerAdressTCP());
	}

	ReceiveFromResult TCPEndpoint::receiveData()
	{
		return receiveFromNonBlocking(socket, storageBuffer.getBufferView());
	}

	std::size_t TCPEndpoint::sendData(ConstantBufferView data)
	{
		return sendToNonBlocking(socket, data);
	}

}