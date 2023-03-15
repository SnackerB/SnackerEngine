#include "Network/Network.h"
#include "core/Log.h"
#include "core/Engine.h"
#include "Network/SERP.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <array>
#include <memory>
#include <optional>
#include <random>
#include <unordered_map>

namespace SnackerEngine
{

    static constexpr unsigned int maxMessageLength = 4000;
    static constexpr unsigned int socketTimeoutMilliseconds = 1;
    static constexpr unsigned int generatePortNumberTries = 10;
    std::array<uint8_t, maxMessageLength> networkBuffer;
    char* networkBufferPtr;
	static SOCKET clientSocket;
    static sockaddr_in clientAddress;
    static uint16_t clientID;
	static sockaddr_in serverAdress;
    static bool isSetup;
    static bool isConnectedToSERPServer;
    static unsigned int nextMessageID;
    using MessageID = unsigned int;
    struct IncomingMessage
    {
        double timeout;
        std::unique_ptr<NetworkManager::SMP_Message> message;
    };
    static std::unordered_map<MESSAGE_TYPE, std::vector<IncomingMessage>> incomingMessages;

    struct UnfinishedMessage
    {
        double timeout;
        std::unique_ptr<NetworkManager::SMP_Message> message;
        std::vector<std::vector<uint8_t>> dataParts;
    };
    static std::unordered_map<uint16_t, std::unordered_map<uint32_t, UnfinishedMessage>> unfinishedIncomingMessages;

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

    /// Returns the next message id and increments it by one
    static unsigned int getNextMessageID()
    {
        return nextMessageID++;
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
        for (unsigned int i = 0; i < generatePortNumberTries; ++i) {
            clientAddress = createSockAddress("0.0.0.0", getRandomPortNumber());
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
            errorLogger << LOGGER::BEGIN << "bind() failed after " << generatePortNumberTries << " tries!" << LOGGER::ENDL;
        }
        return success;
    }

    /// Tries to receive a message in the buffer. Returns an empty optional if a timeout or an error occurred.
    /// Returns the length of the message on success. Make sure remoteAddrLen is properly initialized:
    /// see https://stackoverflow.com/questions/17306405/c-udp-recvfrom-is-acting-strange-wsagetlasterror-10014
    static std::optional<int> recieveMessage(int sock, sockaddr& remoteAddr, int& remoteAddrLen)
    {
        int ret = recvfrom(sock, reinterpret_cast<char*>(networkBuffer.data()), maxMessageLength * sizeof(uint8_t), 0, &remoteAddr, &remoteAddrLen);
        if (ret == -1)
        {
            int error = WSAGetLastError();
            if (error != WSAETIMEDOUT) {
                WSAEFAULT;
                errorLogger << LOGGER::BEGIN << "recvfrom() failed with error code " << error << LOGGER::ENDL;
            }
            return {};
        }
        return ret;
    }

    /// Sends the message in the buffer to the given address. Returns true on success and false on failure.
    static bool sendMessage(SOCKET socket, const sockaddr_in& Addr, int len)
    {
        int ret = sendto(socket, networkBufferPtr, len, 0, reinterpret_cast<const sockaddr*>(&Addr), sizeof(sockaddr_in));
        if (ret == SOCKET_ERROR) {
            errorLogger << LOGGER::BEGIN << "sendto() failed with error code: " << WSAGetLastError() << LOGGER::ENDL;
        }
        return ret;
    }

    bool NetworkManager::initialize()
    {
        if (isSetup) return true;
        if (!startWinsock()) return false;
        auto resultSocket = createUDPSocket();
        if (!resultSocket.has_value()) return false;
        clientSocket = resultSocket.value();
        setSocketTimeoutMilliseconds(clientSocket, socketTimeoutMilliseconds);
        if (!bindSocketToAddress(clientSocket, clientAddress)) {
            closesocket(clientSocket);
            return false;
        }
        isSetup = true;
        clientID = 0;
        // Setup buffer ptr
        networkBufferPtr = (char*)(&networkBuffer[0]);
        serverAdress = createSockAddress(serpServerIP, serpServerPort);
        nextMessageID = 0;
        incomingMessages.clear();
        unfinishedIncomingMessages.clear();
        return true;
    }

    /// Transforms the header to network byte order and copies it into the buffer
    static void writeSERPHeader(SERP_Header serpHeader)
    {
        serpHeader.turnToNetworkByteOrder();
        std::memcpy(networkBufferPtr, &serpHeader, sizeof(SERP_Header));
    }

    /// Transforms the header to network byte order and copies it into the buffer at the correct position
    static void writeSMPHeader(SMP_Header smpHeader)
    {
        smpHeader.turnToNetworkByteOrder();
        std::memcpy(networkBufferPtr + sizeof(SERP_Header), &smpHeader, sizeof(SMP_Header));
    }

    /// Writes data to the buffer
    static void writeData(const std::vector<uint8_t>& data)
    {
        std::memcpy(networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header), data.data(), min(data.size(), maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header)));
    }

    /// Turns the destinations to network byte order and writes them to the end of the buffer. 
    /// Returns true on success and false on failure.
    static bool writeDestinations(unsigned int dataSizeInBytes, std::vector<uint16_t> destinations)
    {
        if (maxMessageLength < sizeof(SERP_Header) + sizeof(SMP_Header) + dataSizeInBytes + destinations.size() * sizeof(uint16_t)) return false;
        for (auto& destination : destinations) {
            destination = htons(destination);
        }
        std::memcpy(networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header) + dataSizeInBytes, destinations.data(), destinations.size() * sizeof(uint16_t));
        return true;
    }

    static SERP_Header readSERPHeader()
    {
        SERP_Header result;
        std::memcpy(&result, networkBufferPtr, sizeof(SERP_Header));
        result.turnToHostByteOrder();
        return result;
    }

    static SMP_Header readSMPHeader()
    {
        SMP_Header result;
        std::memcpy(&result, networkBufferPtr + sizeof(SERP_Header), sizeof(SMP_Header));
        result.turnToHostByteOrder();
        return result;
    }

    static std::vector<uint8_t>&& readData(unsigned int numBytes)
    {
        std::vector<uint8_t> result;
        unsigned int copyBytes = maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header);
        result.resize(min(numBytes, copyBytes));
        std::memcpy(result.data(), networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header), result.size());
        return std::move(result);
    }

    bool NetworkManager::sendMessage(const SMP_Message& message, uint16_t destination)
    {
        if (!isConnectedToSERPServer) return false;
        unsigned int dataLength = message.data.size();
        unsigned int numberPackets = dataLength / (maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header));
        if (dataLength % (maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) != 0)) numberPackets++;
        /// Check if we can send in a single message!
        if (numberPackets == 1)
        {
            SERP_Header serpHeader(clientID, destination, sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(uint8_t) * message.data.size(), 0, 1, getNextMessageID());
            writeSERPHeader(serpHeader);
            writeSMPHeader(message.smpHeader);
            if (!message.data.empty()) writeData(message.data);
            SnackerEngine::sendMessage(clientSocket, serverAdress, sizeof(SERP_Header) + sizeof(SMP_Header) + dataLength);
            return true;
        }
        else {
            SERP_Header serpHeader(clientID, destination, maxMessageLength, 0, numberPackets, getNextMessageID());
            writeSMPHeader(message.smpHeader);
            unsigned int offset = 0;
            for (int i = 0; i < numberPackets - 1; ++i)
            {
                serpHeader.part = i;
                writeSERPHeader(serpHeader);
                std::memcpy(networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header));
                offset += maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header);
                SnackerEngine::sendMessage(clientSocket, serverAdress, maxMessageLength);
            }
            serpHeader.part = numberPackets - 1;
            unsigned int bytesLeft = message.data.size() - offset;
            serpHeader.len = bytesLeft + sizeof(SERP_Header) + sizeof(SMP_Header);
            writeSERPHeader(serpHeader);
            std::memcpy(networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, bytesLeft);
            SnackerEngine::sendMessage(clientSocket, serverAdress, bytesLeft + sizeof(SERP_Header) + sizeof(SMP_Header));
            return true;
        }
        return false;
    }

    bool NetworkManager::sendMessageMulticast(const NetworkManager::SMP_Message& message, const std::vector<uint16_t>& destinations)
    {
        if (!isConnectedToSERPServer) return false;
        if (destinations.size() * sizeof(uint16_t) + sizeof(SERP_Header) + sizeof(SMP_Header) >= maxMessageLength) return false;
        unsigned int dataLength = message.data.size();
        unsigned int numberPackets = dataLength / (maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinations.size() * sizeof(uint16_t));
        if (dataLength % (maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinations.size() * sizeof(uint16_t) != 0)) numberPackets++;
        /// Check if we can send in a single message!
        if (numberPackets == 1)
        {
            SERP_Header serpHeader(clientID, SERP_DST_MULTICAST, sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(uint8_t) * message.data.size(), 0, 1, getNextMessageID());
            writeSERPHeader(serpHeader);
            writeSMPHeader(message.smpHeader);
            if (!message.data.empty()) writeData(message.data);
            writeDestinations(message.data.size(), destinations);
            SnackerEngine::sendMessage(clientSocket, serverAdress, sizeof(SERP_Header) + sizeof(SMP_Header) + dataLength + destinations.size() * sizeof(uint16_t));
            return true;
        }
        else {
            SERP_Header serpHeader(clientID, SERP_DST_MULTICAST, maxMessageLength - destinations.size() * sizeof(uint16_t), 0, numberPackets, getNextMessageID());
            writeSMPHeader(message.smpHeader);
            unsigned int offset = 0;
            unsigned int destinationsSizeBytes = destinations.size() * sizeof(uint16_t);
            unsigned int destinationsOffset = maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinationsSizeBytes;
            for (int i = 0; i < numberPackets - 1; ++i)
            {
                serpHeader.part = i;
                writeSERPHeader(serpHeader);
                std::memcpy(networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinationsSizeBytes);
                writeDestinations(destinationsOffset, destinations);
                offset += maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinationsSizeBytes;
                SnackerEngine::sendMessage(clientSocket, serverAdress, maxMessageLength);
            }
            serpHeader.part = numberPackets - 1;
            unsigned int bytesLeft = message.data.size() - offset;
            serpHeader.len = bytesLeft + sizeof(SERP_Header) + sizeof(SMP_Header);
            writeSERPHeader(serpHeader);
            std::memcpy(networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, bytesLeft);
            writeDestinations(bytesLeft, destinations);
            SnackerEngine::sendMessage(clientSocket, serverAdress, bytesLeft + sizeof(SERP_Header) + sizeof(SMP_Header));
            return true;
        }
        return false;
    }

    void NetworkManager::connectToSERPServer()
    {
        // Send advertisement to server
        SERP_Header serpHeader(clientID, 0, sizeof(SERP_Header) + sizeof(SMP_Header), 0, 1, getNextMessageID());
        SMP_Header smpHeader(MESSAGE_TYPE::ADVERTISEMENT, static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::REQUEST));
        writeSERPHeader(serpHeader);
        writeSMPHeader(smpHeader);
        SnackerEngine::sendMessage(clientSocket, serverAdress, static_cast<int>(serpHeader.len));
    }

    bool NetworkManager::isConnectedToServer()
    {
        return isConnectedToSERPServer;
    }

    /// Inserts the given message into the incomingMessages map
    void insertIntoMessageBuffer(std::unique_ptr<NetworkManager::SMP_Message>&& message)
    {
        MESSAGE_TYPE messageType = static_cast<MESSAGE_TYPE>(message->smpHeader.type);
        auto it = incomingMessages.find(messageType);
        if (it == incomingMessages.end()) {
            std::vector<IncomingMessage> temp;
            temp.push_back({ 0.0, std::move(message) });
            incomingMessages.insert(std::make_pair(messageType, std::move(temp)));
        }
        else {
            it->second.push_back({ 0.0, std::move(message) });
        }
    }

    /// Inserts the message that is currently stored in the buffer into the incomingMessages map
    /// Call this function for messages that fit into a single packet
    void insertIntoMessageBuffer(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        std::unique_ptr<NetworkManager::SMP_Message> message = std::make_unique<NetworkManager::SMP_Message>(serpHeader.src, smpHeader, std::move(readData(serpHeader.len - sizeof(SERP_Header) - sizeof(SMP_Header))));
        insertIntoMessageBuffer(std::move(message));
    }

    /// Inserts the message that is currently stored in the buffer into the unfinishedMessages map
    /// Call this function for messages longer than a single packet
    void insertIntoUnfinishedMessageBuffer(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        auto it1 = unfinishedIncomingMessages.find(serpHeader.src);
        if (it1 == unfinishedIncomingMessages.end()) {
            it1 = unfinishedIncomingMessages.insert(std::make_pair(serpHeader.src, std::unordered_map<uint32_t, UnfinishedMessage>())).first;
        }
        auto it2 = it1->second.find(serpHeader.id);
        if (it2 == it1->second.end()) {
            UnfinishedMessage message{ 0.0, std::make_unique<NetworkManager::SMP_Message>(serpHeader.src, smpHeader, std::vector<uint8_t>{}), std::vector<std::vector<uint8_t>>(serpHeader.total) };
            it2 = it1->second.insert(std::make_pair(serpHeader.id, UnfinishedMessage{})).first;
        }
        // Insert data at the correct location
        if (serpHeader.part < it2->second.dataParts.size()) {
            it2->second.dataParts[serpHeader.part] = std::move(readData(serpHeader.len - sizeof(SERP_Header) - sizeof(SMP_Header)));
        }
        // Check if the message is now complete
        for (const auto& dataPart : it2->second.dataParts) {
            if (dataPart.empty()) return;
        }
        // The message is complete! Add it to the message buffer
        unsigned int totalDataLength = 0;
        // Determine total data legth
        for (const auto& dataPart : it2->second.dataParts) {
            totalDataLength += dataPart.size();
        }
        // Copy data
        it2->second.message->data.resize(totalDataLength);
        unsigned int dataOffset = 0;
        for (const auto& dataPart : it2->second.dataParts)
        {
            std::memcpy(it2->second.message->data.data() + dataOffset, dataPart.data(), dataPart.size());
            dataOffset += dataPart.size();
        }
        insertIntoMessageBuffer(std::move(it2->second.message));
        // Delete message from unfinished message buffer!
        it1->second.erase(serpHeader.id);
        if (it1->second.empty()) {
            unfinishedIncomingMessages.erase(serpHeader.src);
        }
    }

    /// Handles an ADVERTISMENT message coming from the server
    void handleServerMessageAdvertisement(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        switch (smpHeader.options)
        {
        case static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT):
        {
            isConnectedToSERPServer = false;
            infoLogger << LOGGER::BEGIN << "Disconnected from SERP server" << LOGGER::ENDL;
            break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::OK):
        {
            // Extract clientID
            if (serpHeader.len != sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(uint16_t)) {
                warningLogger << LOGGER::BEGIN << "Advertisement message from SERP server had unexpected length" << LOGGER::ENDL;
            }
            else {
                for (unsigned int i = 0; i < sizeof(uint16_t); ++i) {
                    std::memcpy(reinterpret_cast<uint8_t*>(&clientID) + i, networkBufferPtr + sizeof(SERP_Header) + sizeof(SMP_Header) + i, sizeof(uint8_t));
                }
                clientID = ntohs(clientID);
                isConnectedToSERPServer = true;
                infoLogger << LOGGER::BEGIN << "Connected to SERP server with clientID " << clientID << "!" << LOGGER::ENDL;
            }
            break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::REQUEST):
        {
            warningLogger << LOGGER::BEGIN << "Server send a connection request" << LOGGER::ENDL;
            break;
        }
        default:
            break;
        }
    }

    /// Handles an ECHO message coming from the server
    void handleServerMessageEcho(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        switch (smpHeader.options)
        {
        case static_cast<uint16_t>(MESSAGE_OPTION_ECHO::ECHO_REPLY):
        {
            break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ECHO::ECHO_REQUEST):
        {
            SERP_Header replySERPHeader(clientID, 0, serpHeader.len, 0, 1, getNextMessageID());
            SMP_Header replySMPHeader(MESSAGE_TYPE::ECHO, static_cast<uint16_t>(MESSAGE_OPTION_ECHO::ECHO_REPLY));
            writeSERPHeader(replySERPHeader);
            writeSMPHeader(replySMPHeader);
            sendMessage(clientSocket, serverAdress, serpHeader.len);
            break;
        }
        default:
        {
            warningLogger << LOGGER::BEGIN << "Got unexpected message option from SERP server for message type echo" << LOGGER::ENDL;
            break;
        }
        }
    }

    /// Handles an error message coming from the server
    void handleServerMessageError(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        switch (smpHeader.options)
        {
        case static_cast<uint16_t>(MESSAGE_OPTION_ERROR::BAD_OPTION):
        {
            errorLogger << LOGGER::BEGIN << "SERP server sent: Bad option"; break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ERROR::BAD_TYPE):
        {
            errorLogger << LOGGER::BEGIN << "SERP server sent: Bad type"; break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ERROR::NOT_FOUND):
        {
            errorLogger << LOGGER::BEGIN << "SERP server sent: Not found"; break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ERROR::TIMEOUT):
        {
            errorLogger << LOGGER::BEGIN << "SERP server sent: timeout"; break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ERROR::TOO_MANY_CLIENTS):
        {
            errorLogger << LOGGER::BEGIN << "SERP server sent: Too many clients"; break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ERROR::UNSPECIFIED):
        default:
        {
            errorLogger << LOGGER::BEGIN << "SERP server sent: Unspecified"; break;
        }
        }
        if (serpHeader.len > sizeof(SERP_Header) + sizeof(SMP_Header)) {
            // Print additional message
            for (unsigned int i = sizeof(SERP_Header) + sizeof(SMP_Header); i < serpHeader.len; ++i) {
                errorLogger << networkBuffer[i];
            }
        }
        errorLogger << LOGGER::ENDL;
    }

    void NetworkManager::update(double dt)
    {
        // Check if there are any incoming messages
        if (isSetup) {
            sockaddr remoteAddr;
            int remoteAddrLen = sizeof(sockaddr); // TODO: Check if this is wrong?
            while (auto len = recieveMessage(clientSocket, remoteAddr, remoteAddrLen).has_value())
            {
                if (remoteAddrLen != sizeof(sockaddr_in) || !compareAddresses(*(reinterpret_cast<sockaddr_in*>(&remoteAddr)), serverAdress)) {
                    warningLogger << LOGGER::BEGIN << "Received a message that was not sent from the SERP server!" << LOGGER::ENDL;
                    continue;
                }
                // Extract message header
                SERP_Header serpHeader = readSERPHeader();
                SMP_Header smpHeader = readSMPHeader();
                // If the message is from the server, we need to do something with it
                if (serpHeader.src == 0) {
                    switch (static_cast<MESSAGE_TYPE>(smpHeader.type))
                    {
                    case MESSAGE_TYPE::ADVERTISEMENT:
                    {
                        handleServerMessageAdvertisement(serpHeader, smpHeader);
                        break;
                    }
                    case MESSAGE_TYPE::ECHO:
                    {
                        handleServerMessageEcho(serpHeader, smpHeader);
                        break;
                    }
                    case MESSAGE_TYPE::ERROR_MESSAGE:
                    {
                        handleServerMessageError(serpHeader, smpHeader);
                        break;
                    }
                    default:
                        break;
                    }
                }
                else {
                    // If the message consists of only a single packet, we can put it directly into the message buffer
                    if (serpHeader.total == 1) {
                        insertIntoMessageBuffer(serpHeader, smpHeader);
                    }
                    // If the message consitst of more than one part, we have to put it into the unfinished message buffer
                    else {
                        insertIntoUnfinishedMessageBuffer(serpHeader, smpHeader);
                    }
                }
            }
            // TODO: Update timeouts!
        }
    }

    void NetworkManager::cleanup()
    {
        if (isConnectedToSERPServer)
        {
            // Send disconnect message
            SERP_Header serpHeader(clientID, 0, sizeof(SERP_Header) + sizeof(SMP_Header), 0, 1, getNextMessageID());
            SMP_Header smpHeader(MESSAGE_TYPE::ADVERTISEMENT, static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT));
            writeSERPHeader(serpHeader);
            writeSMPHeader(smpHeader);
            SnackerEngine::sendMessage(clientSocket, serverAdress, static_cast<int>(serpHeader.len));
        }
        if (isSetup)
        {
            closesocket(clientSocket);
        }
    }

}