#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <iostream>

#include "TCP/TCP.h"

namespace SnackerEngine
{

	int getSERPServerPort()
	{
		return 7777;
	}

	sockaddr_in getSERPServerAdressTCP()
	{
		// For now: local host
		sockaddr_in result{};
		memset(&result, 0, sizeof(sockaddr_in));
		result.sin_family = AF_INET;
		InetPton(AF_INET, _T("127.0.0.1"), &result.sin_addr.s_addr);
		result.sin_port = htons(getSERPServerPort());
		return result;
	}

	bool compare(const sockaddr_in& addr1, const sockaddr_in& addr2)
	{
		return
			addr1.sin_family == addr2.sin_family &&
			addr1.sin_addr.s_addr == addr2.sin_addr.s_addr &&
			addr1.sin_port == addr2.sin_port;
	}

	std::optional<SocketTCP> createSocketTCP(int port)
	{
		SocketTCP result{};
		// Create socket
		result.sock = socket(AF_INET, SOCK_STREAM, NULL);
		if (result.sock == -1) return std::nullopt;
		// bind socket to port
		result.addr.sin_family = AF_INET;
		result.addr.sin_port = htons(port);
		result.addr.sin_addr.s_addr = INADDR_ANY;
		if (bind(result.sock, (sockaddr*)&result.addr, sizeof(result.addr)) != 0) return std::nullopt;
		return std::move(result);
	}

	bool setToNonBlocking(SocketTCP& socket)
	{
		u_long mode = 1;  // 1 to enable non-blocking socket
		return ioctlsocket(socket.sock, FIONBIO, &mode) == 0;
	}

	bool markAsListen(SocketTCP& socket, int backlog_queue_size)
	{
		return listen(socket.sock, backlog_queue_size) == 0;
	}

	bool connectTo(SocketTCP& socket, const sockaddr_in& addr)
	{
		int result = connect(socket.sock, (const sockaddr*)&addr, sizeof(sockaddr_in));
		if (result == 0) {
			socket.valid = true;
			return true;
		}
		return false;
	}

	bool sendTo(const SocketTCP& socket, const Buffer& buffer)
	{
		return send(socket.sock, (const char*) buffer.getDataPtr(), buffer.size(), 0) == 0;
	}

	bool sendToNonBlocking(const SocketTCP& socket, const Buffer& buffer)
	{
		int result = send(socket.sock, (const char*)buffer.getDataPtr(), buffer.size(), 0);
		if (result == 0) return true;
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK) return true;
		return false;
	}

	std::optional<Buffer> receiveFrom(const SocketTCP& socket, Buffer& storageBuffer)
	{
		int result = recv(socket.sock, (char*)storageBuffer.getDataPtr(), storageBuffer.size(), NULL);
		if (result > 0) {
			std::vector<std::byte> resultBuffer(result);
			memcpy(&(resultBuffer[0]), storageBuffer.getDataPtr(), result);
			return std::move(resultBuffer);
		}
		else if (result == SOCKET_ERROR) {
			int error = WSAGetLastError();
			std::cout << "recv() failed with error code " << error << std::endl;
			return std::nullopt;
		}
		return std::nullopt;
	}

	std::optional<Buffer> receiveFromNonBlocking(const SocketTCP& socket, Buffer& storageBuffer)
	{
		std::vector<std::byte> data{};
		while (true) {
			int result = recv(socket.sock, (char*)storageBuffer.getDataPtr(), storageBuffer.size(), NULL);
			if (result > 0) {
				data.resize(data.size() + result);
				memcpy(&(data[0]), storageBuffer.getDataPtr(), result);
				return std::move(Buffer(std::move(data)));
			}
			else {
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK) {
					return {};
				}
				if (error == WSAEMSGSIZE) {
					data.resize(data.size() + storageBuffer.size());
					memcpy(&(data[0]), storageBuffer.getDataPtr(), storageBuffer.size());
				}
				else {
					std::cout << "recv() failed with error code " << error << std::endl;
					return std::nullopt;
				}
			}
		}
		return std::nullopt;
	}

	std::optional<SocketTCP> acceptConnectionRequest(SocketTCP& socket)
	{
		SocketTCP newSocket{};
		int addrLen = sizeof(sockaddr_in);
		newSocket.sock = accept(socket.sock, (sockaddr*)&newSocket.addr, &addrLen);
		if (newSocket.sock == -1) {
			std::cout << "accept() failed with errorcode " << WSAGetLastError() << std::endl;
			return std::nullopt;
		}
		return std::move(newSocket);
	}

	SocketTCP::SocketTCP()
		: sock{}, addr{}, valid{ false } {}

	SocketTCP::~SocketTCP()
	{
		if (valid) {
			int result = closesocket(sock);
			if (result == -1) std::cout << "closesocket() failed with errorcode " << WSAGetLastError() << std::endl;
		}
	}

	SocketTCP::SocketTCP(SocketTCP&& other) noexcept
		: sock(other.sock), addr(other.addr), valid(other.valid)
	{
		other.valid = false;
	}

	SocketTCP& SocketTCP::operator=(SocketTCP&& other) noexcept
	{
		if (valid) {
			closesocket(sock);
		}
		sock = other.sock;
		addr = other.addr;
		valid = other.valid;
		other.valid = false;
		return *this;
	}

	

}