#include "Network/Network.h"
#include "core/Log.h"
#include "core/Engine.h"

#include <winsock.h>
#include <ws2tcpip.h>
#include <array>
#include <memory>
#include <optional>
#include <random>

namespace SnackerEngine
{

#define MAX_MESSAGE_LENGTH 4000

	static std::array<uint8_t, MAX_MESSAGE_LENGTH> networkBuffer;
	static SOCKET networkSocket;
	static std::unique_ptr<SOCKADDR> serverAdress;

    /// Initializes winsock. Returns empty optional on failure.
    static std::optional<int> startWinsock()
    {
        WSADATA wsa;
        int ret = WSAStartup(MAKEWORD(2, 0), &wsa);
        if (ret != 0) {
            errorLogger << LOGGER::BEGIN << "startWinsock() faied with error code " << ret << LOGGER::ENDL;
            return {};
        }
        return ret;
    }

    /// Creates a UDP Socket. Returns empty optional on failure.
    std::optional<int> createUDPSocket()
    {
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1) {
            errorLogger << LOGGER::BEGIN << "socket() failed with error code " << WSAGetLastError() << LOGGER::ENDL;
            return {};
        }
        return sock;
    }

    /// Creates an internet socket addr
    SOCKADDR_IN createSockAddress(const std::string& ip, int port)
    {
        SOCKADDR_IN addr;
        memset(&addr, 0, sizeof(SOCKADDR_IN)); // zuerst alles auf 0 setzten 
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);
        return addr;
    }

    /// binds the given socket to the given address. Returns true on success and false on failure
    bool bindUDPSocket(int sock, const sockaddr& addr_server, int addr_len)
    {
        int ret = bind(sock, &addr_server, addr_len);
        if (ret == -1) {
            errorLogger << LOGGER::BEGIN << "socket() failed with error code " << WSAGetLastError() << LOGGER::ENDL;
            return false;
        }
        return true;
    }

    /// Generates a random port number
    unsigned int getRandomPortNumber()
    {
        std::uniform_int_distribution<unsigned> dist;
        return 49152 + (dist(Engine::getRandomEngine()) % (65535 - 29152 + 1));
    }

    /// Sets the timeout for the socket in milliseconds
    void setSocketTimeoutMilliseconds(int sock, int milliseconds)
    {
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&milliseconds, sizeof(milliseconds));
    }

    /// Tries to receive a message in the buffer. Returns an empty optional if a timeout or an error occurred.
    std::optional<int> recieveMessage(int sock, SOCKADDR& remoteAddr, int& remoteAddrLen)
    {
        int ret = recvfrom(sock, reinterpret_cast<char*>(networkBuffer.data()), MAX_MESSAGE_LENGTH, 0, &remoteAddr, &remoteAddrLen);
        if (ret == -1)
        {
            int error = WSAGetLastError();
            if (error != WSAETIMEDOUT) {
                errorLogger << LOGGER::BEGIN << "recvfrom() failed with error code: " << WSAGetLastError() << LOGGER::ENDL;
            }
            return {};
        }
        return ret;
    }

    /// Sends the message in the buffer to the given address. Returns true on success and false on failure.
    bool sendMessage(int sock, const sockaddr_in& Addr, int len)
    {
        int ret = sendto(sock, reinterpret_cast<char*>(networkBuffer.data()), len, 0, reinterpret_cast<const sockaddr*>(&Addr), sizeof(sockaddr_in));
        if (ret == SOCKET_ERROR) {
            errorLogger << LOGGER::BEGIN << "sendto() failed with error code: " << WSAGetLastError() << LOGGER::ENDL;
        }
        return ret;
    }


    bool NetworkManager::initialize()
    {
        return false;
    }

    void NetworkManager::connectToSERPServer()
    {
    }

    bool NetworkManager::isConnectedToServer()
    {
        return false;
    }

    bool NetworkManager::sendMessage(const SMP_Message& message)
    {
        return false;
    }

    bool NetworkManager::sendMessageMulticast(const NetworkManager::SMP_Message& message, const std::vector<uint16_t>& destinations)
    {
        return false;
    }

}