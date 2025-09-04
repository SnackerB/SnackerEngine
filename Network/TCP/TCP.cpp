#include "TCP.h"

#ifdef _WINDOWS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#endif // _WINDOWS
#ifdef _LINUX
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cerrno>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif // _LINUX

#include <iostream>

namespace SnackerEngine
{

	int getSERPServerPort()
	{
		return 7777;
	}

	int getSERPServerDataPort()
	{
		return 7778;
	}

	sockaddr_in getSERPServerAdressTCP()
	{
		sockaddr_in result{};
		memset(&result, 0, sizeof(sockaddr_in));
		result.sin_family = AF_INET;
#ifdef _WINDOWS
		//InetPton(AF_INET, _T("127.0.0.1"), &result.sin_addr.s_addr);
		InetPton(AF_INET, _T("45.131.109.173"), &result.sin_addr.s_addr);
#endif // _WINDOWS
#ifdef _LINUX
		//inet_pton(AF_INET, "127.0.0.1", &result.sin_addr.s_addr);
		inet_pton(AF_INET, "45.131.109.173", &result.sin_addr.s_addr);
#endif // _LINUX
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
#ifdef _WINDOWS
		result.sock = socket(AF_INET, SOCK_STREAM, NULL);
#endif // _WINDOWS
#ifdef _LINUX
		result.sock = socket(AF_INET, SOCK_STREAM, 0);
#endif // _LINUX
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
#ifdef _WINDOWS
		u_long mode = 1;  // 1 to enable non-blocking socket
		return ioctlsocket(socket.sock, FIONBIO, &mode) == 0;
#endif // _WINDOWS
#ifdef _LINUX
		int flags = fcntl(socket.sock, F_GETFL, 0);
    	if (flags == -1) return false;
    	flags |= O_NONBLOCK;
    	return (fcntl(socket.sock, F_SETFL, flags) != -1);
#endif // _LINUX
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

	ConnectResult connectToNonBlocking(SocketTCP& socket, const sockaddr_in& addr)
	{
		int result = connect(socket.sock, (const sockaddr*)&addr, sizeof(sockaddr_in));
		if (result == 0) {
			socket.valid = true;
			return ConnectResult{ ConnectResult::Result::SUCCESS, 0 };
		}
		else {
#ifdef _WINDOWS
			int error = WSAGetLastError();
			if (error == WSAEINPROGRESS || error == WSAEWOULDBLOCK || error == WSAEALREADY) {
				return ConnectResult{ ConnectResult::Result::PENDING, 0 };
			}
			else if (error == WSAEISCONN) {
				socket.valid = true;
				return ConnectResult{ ConnectResult::Result::SUCCESS, 0 };
			}
			else {
				return ConnectResult{ ConnectResult::Result::ERROR, error };
			}
#endif // _WINDOWS
#ifdef _LINUX
			int error = errno;
			if (error == EINPROGRESS || error == EWOULDBLOCK || error == EALREADY) {
				return ConnectResult{ ConnectResult::Result::PENDING, 0 };
			}
			else if (error == EISCONN) {
				socket.valid = true;
				return ConnectResult{ ConnectResult::Result::SUCCESS, 0 };
			}
			else {
				return ConnectResult{ ConnectResult::Result::ERROR, error };
			}
#endif // _LINUX
		}
	}

	bool sendTo(const SocketTCP& socket, ConstantBufferView buffer)
	{
		return send(socket.sock, (const char*) buffer.getDataPtr(), static_cast<int>(buffer.size()), 0) >= 0;
	}

	bool sendToNonBlocking(const SocketTCP& socket, ConstantBufferView buffer)
	{
		int result = send(socket.sock, (const char*)buffer.getDataPtr(), static_cast<int>(buffer.size()), 0);
		std::cout << "sendToNonBlocking() called with buffer of size " << buffer.size() << ". Result = " << result << std::endl;
		if (result >= 0) return true;
#ifdef _WINDOWS
		int error = WSAGetLastError();
		if (error == WSAEWOULDBLOCK) {
			std::cout << "endpointTCP: WSAEWOULDBLOCK detected." << std::endl; // DEBUG
			return true;
		}
#endif // _WINDOWS
#ifdef _LINUX
		int error = errno;
		if (error == EWOULDBLOCK) {
			std::cout << "endpointTCP: WSAEWOULDBLOCK detected." << std::endl; // DEBUG
			return true;
		}
#endif // _LINUX
		else {
			std::cout << "send() failed with error code " << error << std::endl;
			return false;
		}
	}

	std::optional<Buffer> receiveFrom(const SocketTCP& socket, BufferView storageBuffer)
	{
		std::vector<std::byte> data{};
		while (true) {
#ifdef _WINDOWS
			int result = recv(socket.sock, (char*)storageBuffer.getDataPtr(), static_cast<int>(storageBuffer.size()), NULL);
#endif // _WINDOWS
#ifdef _LINUX
			int result = recv(socket.sock, (char*)storageBuffer.getDataPtr(), static_cast<int>(storageBuffer.size()), 0);
#endif // _LINUX
			if (result > 0) {
				data.resize(data.size() + result);
				memcpy(&(data[0]), storageBuffer.getDataPtr(), result);
				return std::move(Buffer(std::move(data)));
			}
			else {
#ifdef _WINDOWS
				int error = WSAGetLastError();
				if (error == WSAEMSGSIZE) {
#endif // _WINDOWS
#ifdef _LINUX
				int error = errno;
				if (error == EMSGSIZE) {
#endif // _LINUX
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

	std::optional<Buffer> receiveFromNonBlocking(const SocketTCP& socket, BufferView storageBuffer)
	{
		std::vector<std::byte> data{};
		while (true) {
#ifdef _WINDOWS
			int result = recv(socket.sock, (char*)storageBuffer.getDataPtr(), static_cast<int>(storageBuffer.size()), NULL);
#endif // _WINDOWS
#ifdef _LINUX
			int result = recv(socket.sock, (char*)storageBuffer.getDataPtr(), static_cast<int>(storageBuffer.size()), 0);
#endif // _LINUX			
			if (result > 0) {
				data.resize(data.size() + result);
				memcpy(&(data[0]), storageBuffer.getDataPtr(), result);
				return std::move(Buffer(std::move(data)));
			}
			else {
#ifdef _WINDOWS
				int error = WSAGetLastError();
				if (error == WSAEWOULDBLOCK) {
					return {};
				}
				if (error == WSAEMSGSIZE) {
#endif // _WINDOWS
#ifdef _LINUX
				int error = errno;
				if (error == EWOULDBLOCK) {
					return {};
				}
				if (error == EMSGSIZE) {
#endif // _LINUX
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
#ifdef _WINDOWS
			int addrLen = sizeof(sockaddr_in);
			newSocket.sock = accept(socket.sock, (sockaddr*)&newSocket.addr, &addrLen);
			if (newSocket.sock == -1) {
			std::cout << "accept() failed with errorcode " << WSAGetLastError() << std::endl;
#endif // _WINDOWS
#ifdef _LINUX
			unsigned int addrLen = sizeof(sockaddr_in);
			newSocket.sock = accept(socket.sock, (sockaddr*)&newSocket.addr, &addrLen);
			if (newSocket.sock == -1) {
			std::cout << "accept() failed with error " << errno << " " << strerror(errno) << std::endl;
#endif // _LINUX
			return std::nullopt;
		}
		return std::move(newSocket);
	}

	SocketTCP::SocketTCP()
		: sock{}, addr{}, valid{ false } {}

	SocketTCP::~SocketTCP()
	{
		if (valid) {
#ifdef _WINDOWS
			int result = closesocket(sock);
			if (result == -1) std::cout << "closesocket() failed with errorcode " << WSAGetLastError() << std::endl;
#endif // _WINDOWS
#ifdef _LINUX
			int result = close(sock);
			if (result == -1) std::cout << "close(sock) failed with error " << errno << " " << strerror(errno) << std::endl;
#endif // _LINUX
		}
		//else {
		//	// DEBUG
		//	std::cout << "destroyed invalid socket!" << std::endl;
		//}
	}

	SocketTCP::SocketTCP(SocketTCP&& other) noexcept
		: sock(other.sock), addr(other.addr), valid(other.valid)
	{
		other.valid = false;
	}

	SocketTCP& SocketTCP::operator=(SocketTCP&& other) noexcept
	{
		if (valid) {
#ifdef _WINDOWS
			closesocket(sock);
#endif // _WINDOWS
#ifdef _LINUX
			close(sock);
#endif // _LINUX
		}
		sock = other.sock;
		addr = other.addr;
		valid = other.valid;
		other.valid = false;
		return *this;
	}

}