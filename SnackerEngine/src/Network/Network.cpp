#include "Network/Network.h"
#include "core/Log.h"
#include "core/Engine.h"
#include "Network/SERP.h"
#include "Network/NetworkData.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <array>
#include <memory>
#include <optional>
#include <random>
#include <unordered_map>
#include <queue>

namespace SnackerEngine
{

    /// Struct that stores all necessary variables, containers and implements important helper functions
    NetworkData networkData;

    /// Initializes winsock. Returns true on success and false on failure.
    static bool startWinsock()
    {
        WSADATA wsa;
        int ret = WSAStartup(MAKEWORD(2, 0), &wsa);
        if (ret != 0) {
            errorLogger << LOGGER::BEGIN << "startWinsock() faied with error code " << ret << LOGGER::ENDL;
            return false;
        }
        return true;
    }

    /// Creates a UDP Socket. Returns empty optional on failure.
    static std::optional<int> createUDPSocket()
    {
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock == -1 || sock == INVALID_SOCKET) {
            errorLogger << LOGGER::BEGIN << "socket() failed with error code " << WSAGetLastError() << LOGGER::ENDL;
            return {};
        }
        return sock;
    }

    /// Creates an internet socket addr
    static sockaddr_in createSockAddress(const std::string& ip, int port)
    {
        sockaddr_in addr;
        memset(&addr, 0, sizeof(sockaddr_in)); // first set everything to 0
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);
        return addr;
    }

    /// binds the given socket to the given address. Returns true on success and false on failure
    static bool bindUDPSocket(int sock, const sockaddr& addr_server, int addr_len)
    {
        int ret = bind(sock, &addr_server, addr_len);
        if (ret == -1) {
            errorLogger << LOGGER::BEGIN << "socket() failed with error code " << WSAGetLastError() << LOGGER::ENDL;
            return false;
        }
        return true;
    }

    /// Generates a random port number
    static unsigned int getRandomPortNumber()
    {
        std::uniform_int_distribution<unsigned> dist;
        return 49152 + (dist(Engine::getRandomEngine()) % (65535 - 29152 + 1));
    }

    /// Sets the timeout for the socket in milliseconds
    static void setSocketTimeoutMilliseconds(SOCKET sock, int milliseconds)
    {
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&milliseconds, sizeof(milliseconds));
    }

    /// Compares two internet addresses
    static bool compareAddresses(const sockaddr_in& addr1, const sockaddr_in& addr2)
    {
        return addr1.sin_addr.S_un.S_addr == addr2.sin_addr.S_un.S_addr &&
            addr1.sin_family == addr2.sin_family &&
            addr1.sin_port == addr2.sin_port;
    }

    /// Binds a socket to the default address. Returns true on success and false on failure
    static bool bindSocketToAddress(SOCKET socket, sockaddr_in& address)
    {
        // Try to generate client address. This is done multiple times to try and get a valid port number.
        bool success = false;
        for (unsigned int i = 0; i < networkData.generatePortNumberTries; ++i) {
            networkData.clientAddress = createSockAddress("0.0.0.0", getRandomPortNumber());
            if (bind(socket, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_in)) == -1)
            {
                warningLogger << LOGGER::BEGIN << "bind() failed with error code " << WSAGetLastError() << "trying again ..." << LOGGER::ENDL;
            }
            else
            {
                success = true;
                break;
            }
        }
        if (!success) {
            errorLogger << LOGGER::BEGIN << "bind() failed after " << networkData.generatePortNumberTries << " tries!" << LOGGER::ENDL;
        }
        return success;
    }

    bool NetworkManager::initialize()
    {
        if (networkData.setup) return true;
        networkData = NetworkData();
        if (!startWinsock()) return false;
        auto resultSocket = createUDPSocket();
        if (!resultSocket.has_value()) return false;
        networkData.clientSocket = resultSocket.value();
        setSocketTimeoutMilliseconds(networkData.clientSocket, networkData.socketTimeoutMilliseconds);
        if (!bindSocketToAddress(networkData.clientSocket, networkData.clientAddress)) {
            closesocket(networkData.clientSocket);
            return false;
        }
        networkData.serverAdress = createSockAddress(serpServerIP, serpServerPort);
        networkData.setup = true;
        return true;
    }

    bool NetworkManager::sendMessage(const SMP_Message& message, uint16_t destination, bool safeSend)
    {
        if (!networkData.connectedToSERPServer) return false;
        if (safeSend)
        {
            std::optional<SafeOutgoingMessageData> outgoingMessage = std::move(SafeOutgoingMessageData::CreateSafeOutgoingMessageData(networkData, message, { destination }));
            if (!outgoingMessage.has_value()) return false;
            networkData.safeOutgoingMessageDatas[networkData.nextMessageID] = std::move(outgoingMessage.value());
            networkData.safeOutgoingMessageDatas[networkData.nextMessageID].pushPacketsToSendQueue(networkData);
            networkData.nextMessageID++;
        }
        else
        {
            if (!networkData.createBasicOutgoingMessages(message, destination)) return false;
            networkData.nextMessageID++;
        }
        return true;
    }

    bool NetworkManager::sendMessage(const SMP_Message& message, const std::vector<uint16_t>& destinations, bool safeSend)
    {
        if (!networkData.connectedToSERPServer) return false;
        if (safeSend)
        {
            std::optional<SafeOutgoingMessageData> outgoingMessage = std::move(SafeOutgoingMessageData::CreateSafeOutgoingMessageData(networkData, message, destinations));
            if (!outgoingMessage.has_value()) return false;
            networkData.safeOutgoingMessageDatas[networkData.nextMessageID] = std::move(outgoingMessage.value());
            networkData.safeOutgoingMessageDatas[networkData.nextMessageID].pushPacketsToSendQueue(networkData);
            networkData.nextMessageID++;
        }
        else
        {
            if (!networkData.createBasicOutgoingMessages(message, destinations)) return false;
            networkData.nextMessageID++;
        }
        return true;
    }

    void NetworkManager::connectToSERPServer()
    {
        // Send advertisement to server
        networkData.sendConnectionRequestToServer();
    }

    bool NetworkManager::isConnectedToSERPServer()
    {
        return networkData.connectedToSERPServer;
    }

    bool NetworkManager::isSetup()
    {
        return networkData.setup;
    }

    uint16_t NetworkManager::getClientID()
    {
        return networkData.clientID;
    }

    void NetworkManager::update(double dt)
    {
        // Check if there are any incoming messages
        if (networkData.setup) {
            // Receive messages
            sockaddr remoteAddr;
            int remoteAddrLen = sizeof(sockaddr); // TODO: Check if this is wrong?
            while (auto len = networkData.recieveMessage(networkData.clientSocket, remoteAddr, remoteAddrLen).has_value())
            {
                if (remoteAddrLen != sizeof(sockaddr_in) || !compareAddresses(*(reinterpret_cast<sockaddr_in*>(&remoteAddr)), networkData.serverAdress)) {
                    warningLogger << LOGGER::BEGIN << "Received a message that was not sent from the SERP server!" << LOGGER::ENDL;
                    continue;
                }
                networkData.handleIncomingMessageInNetworkBuffer();
            }
            // Send messages
            networkData.updateOutgoingMessageQueue(dt);
            // Update Containers
            networkData.updateContainers(dt);
        }
    }

    std::vector<SMP_Message> NetworkManager::getIncomingMessages(MESSAGE_TYPE messageType)
    {
        std::vector<SMP_Message> result;
        auto it = networkData.incomingMessages.begin();
        while (it != networkData.incomingMessages.end()) 
        {
            if (it->smpHeader.type == static_cast<uint16_t>(messageType)) {
                result.push_back(std::move(*it));
                it = networkData.incomingMessages.erase(it);
            }
            else ++it;
        }
        return result;
    }

    std::vector<SMP_Message> NetworkManager::getIncomingMessages()
    {
        std::vector<SMP_Message> result = std::move(networkData.incomingMessages);
        networkData.incomingMessages.clear();
        return result;
    }

    void NetworkManager::setBytesPerSecondsSend(unsigned int bytesPerSecond)
    {
        networkData.bytesPerSecondSend = bytesPerSecond;
        networkData.accumulativeTimeSinceLastSend = 0.0;
    }

    void NetworkManager::cleanup()
    {
        if (networkData.connectedToSERPServer)
        {
            networkData.sendDisconnectMessageToServer();
            networkData.connectedToSERPServer = false;
        }
        if (networkData.setup)
        {
            closesocket(networkData.clientSocket);
            networkData.setup = false;
        }
    }

}