#include "Network/NetworkData.h"
#include "Core/Log.h"
#include "Network/Network.h"
#include "Utility/ByteBuffer.h"

namespace SnackerEngine
{

    /// Transforms the header to network byte order and copies it into the buffer
    static void writeSERPHeader(SERP_Header serpHeader, std::byte* buffer)
    {
        serpHeader.turnToNetworkByteOrder();
        std::memcpy(buffer, &serpHeader, sizeof(SERP_Header));
    }

    /// Transforms the header to network byte order and copies it into the buffer at the correct position
    static void writeSMPHeader(SMP_Header smpHeader, std::byte* buffer)
    {
        smpHeader.turnToNetworkByteOrder();
        std::memcpy(buffer + sizeof(SERP_Header), &smpHeader, sizeof(SMP_Header));
    }

    /// Writes data to the buffer
    static void writeData(const std::vector<std::byte>& data, std::byte* buffer)
    {
        std::memcpy(buffer + sizeof(SERP_Header) + sizeof(SMP_Header), data.data(), data.size());
    }

    /// Turns the destinations to network byte order and writes them to the end of the buffer.
    static void writeDestinations(unsigned int dataSizeInBytes, std::vector<uint16_t> destinations, std::byte* buffer)
    {
        for (auto& destination : destinations) {
            destination = htons(destination);
        }
        std::memcpy(buffer + sizeof(SERP_Header) + sizeof(SMP_Header) + dataSizeInBytes, destinations.data(), destinations.size() * sizeof(uint16_t));
    }

	/// Sends the message in the buffer to the given address. Returns true on success and false on failure.
    static bool sendMessage(SOCKET socket, const sockaddr_in& Addr, int len, const char* buffer)
    {
        int ret = sendto(socket, buffer, len, 0, reinterpret_cast<const sockaddr*>(&Addr), sizeof(sockaddr_in));
        if (ret == SOCKET_ERROR) {
            errorLogger << LOGGER::BEGIN << "sendto() failed with error code: " << WSAGetLastError() << LOGGER::ENDL;
        }
        return ret;
    }

    NetworkData::NetworkData()
        : bytesPerSecondSend(500'000 /* 0.5 MByte per second */), clientSocket{}, clientAddress{}, clientID{}, serverAdress{},
        setup(false), connectedToSERPServer(false), nextMessageID(0), accumulativeTimeSinceLastSend(0.0), networkBuffer{},
        outgoingMessages{}, safeOutgoingMessageDatas{}, incomingUnfinishedMessageDatas{}, safeIncomingMessageData{} {}

    bool NetworkData::sendMessageToServer(const std::byte* buffer, unsigned int messageLength) const
    {
        return sendMessage(clientSocket, serverAdress, messageLength, reinterpret_cast<const char*>(buffer));
    }

    bool  NetworkData::sendMessageToServerSinglecast(const SERP_Header& serpHeader, const SMP_Header& smpHeader, const std::vector<std::byte>& data)
    {
        if (data.size() > maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header))
        {
            warningLogger << LOGGER::BEGIN << "Tried to send singlecast message that was larger than the allowed maxMessageLength!" << LOGGER::ENDL;
            return false;
        }
        writeSERPHeader(serpHeader, networkBuffer.data());
        writeSMPHeader(smpHeader, networkBuffer.data());
        writeData(data, networkBuffer.data());
        return sendMessageToServer(networkBuffer.data(), sizeof(SERP_Header) + sizeof(SMP_Header) + data.size());
    }

    bool  NetworkData::sendMessageToServerMulticast(const SERP_Header& serpHeader, const SMP_Header& smpHeader, const std::vector<std::byte>& data, const std::vector<uint16_t>& destinations)
    {
        if (data.size() > maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - sizeof(uint16_t) * destinations.size())
        {
            warningLogger << LOGGER::BEGIN << "Tried to send multicast message that was larger than the allowed maxMessageLength!" << LOGGER::ENDL;
            return false;
        }
        writeSERPHeader(serpHeader, networkBuffer.data());
        writeSMPHeader(smpHeader, networkBuffer.data());
        if (!data.empty()) writeData(data, networkBuffer.data());
        writeDestinations(data.size(), destinations, networkBuffer.data());
        return sendMessageToServer(networkBuffer.data(), sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(std::byte) * data.size() + sizeof(uint16_t) * destinations.size());
    }

    SERP_Header NetworkData::readSERPHeader()
    {
        SERP_Header result;
        std::memcpy(&result, networkBuffer.data(), sizeof(SERP_Header));
        result.turnToHostByteOrder();
        return result;
    }

    SMP_Header NetworkData::readSMPHeader()
    {
        SMP_Header result;
        std::memcpy(&result, networkBuffer.data() + sizeof(SERP_Header), sizeof(SMP_Header));
        result.turnToHostByteOrder();
        return result;
    }

    std::vector<std::byte> NetworkData::readData(unsigned int numBytes)
    {
        std::vector<std::byte> result;
        unsigned int copyBytes = maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header);
        result.resize(min(numBytes, copyBytes));
        std::memcpy(result.data(), networkBuffer.data() + sizeof(SERP_Header) + sizeof(SMP_Header), result.size());
        return result;
    }

    bool NetworkData::createBasicOutgoingMessages(const SMP_Message& message, uint16_t destination)
    {
        unsigned int dataLength = message.data.size();
        unsigned int dataPerPacket = (maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header));
        unsigned int numberPackets = dataLength / dataPerPacket;
        if (dataLength % dataPerPacket != 0) numberPackets++;
        if (numberPackets == 0) numberPackets = 1;
        /// Check if we can send in a single message!
        if (numberPackets == 1)
        {
            std::unique_ptr<BasicOutgoingMessage> outgoingMessage = std::make_unique<BasicOutgoingMessage>();
            outgoingMessage->data.resize(sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(std::byte) * message.data.size());
            SERP_Header serpHeader(clientID, destination, sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(std::byte) * message.data.size(), 0, 1, nextMessageID);
            writeSERPHeader(serpHeader, outgoingMessage->data.data());
            writeSMPHeader(message.smpHeader, outgoingMessage->data.data());
            if (!message.data.empty()) writeData(message.data, outgoingMessage->data.data());
            outgoingMessages.push(std::move(outgoingMessage));
            return true;
        }
        else {
            SERP_Header serpHeader(clientID, destination, maxMessageLength, 0, numberPackets, nextMessageID);
            BasicOutgoingMessage outgoingMessage;
            outgoingMessage.data.resize(maxMessageLength);
            writeSMPHeader(message.smpHeader, outgoingMessage.data.data());
            unsigned int offset = 0;
            for (int i = 0; i < numberPackets - 1; ++i)
            {
                serpHeader.part = i;
                writeSERPHeader(serpHeader, outgoingMessage.data.data());
                std::memcpy(outgoingMessage.data.data() + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header));
                offset += maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header);
                outgoingMessages.push(std::make_unique<BasicOutgoingMessage>(outgoingMessage));
            }
            serpHeader.part = numberPackets - 1;
            unsigned int bytesLeft = dataLength - offset;
            serpHeader.len = bytesLeft + sizeof(SERP_Header) + sizeof(SMP_Header);
            outgoingMessage.data.resize(serpHeader.len);
            writeSERPHeader(serpHeader, outgoingMessage.data.data());
            std::memcpy(outgoingMessage.data.data() + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, bytesLeft);
            outgoingMessages.push(std::make_unique<BasicOutgoingMessage>(outgoingMessage));
            return true;
        }
    }

    bool NetworkData::createBasicOutgoingMessages(const SMP_Message& message, const std::vector<uint16_t>& destinations)
    {
        if (destinations.size() == 0) return false;
        if (destinations.size() == 1) return createBasicOutgoingMessages(message, destinations[0]);
        if (destinations.size() * sizeof(uint16_t) + sizeof(SERP_Header) + sizeof(SMP_Header) >= maxMessageLength) return false;
        unsigned int dataLength = message.data.size();
        unsigned int numberPackets = dataLength / (maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinations.size() * sizeof(uint16_t));
        if (dataLength % (maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinations.size() * sizeof(uint16_t)) != 0) numberPackets++;
        if (numberPackets == 0) numberPackets = 1;
        /// Check if we can send in a single message!
        if (numberPackets == 1)
        {
            std::unique_ptr<BasicOutgoingMessage> outgoingMessage = std::make_unique<BasicOutgoingMessage>();
            outgoingMessage->data.resize(sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(std::byte) * message.data.size() + sizeof(uint16_t) * destinations.size());
            SERP_Header serpHeader(clientID, SERP_DST_MULTICAST, sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(std::byte) * message.data.size(), 0, 1, nextMessageID);
            writeSERPHeader(serpHeader, outgoingMessage->data.data());
            writeSMPHeader(message.smpHeader, outgoingMessage->data.data());
            if (!message.data.empty()) writeData(message.data, outgoingMessage->data.data());
            writeDestinations(message.data.size(), destinations, outgoingMessage->data.data());
            outgoingMessages.push(std::move(outgoingMessage));
            return true;
        }
        else {
            SERP_Header serpHeader(clientID, SERP_DST_MULTICAST, maxMessageLength - destinations.size() * sizeof(uint16_t), 0, numberPackets, nextMessageID);
            BasicOutgoingMessage outgoingMessage;
            outgoingMessage.data.resize(maxMessageLength);
            writeSMPHeader(message.smpHeader, outgoingMessage.data.data());
            unsigned int offset = 0;
            unsigned int destinationsSizeBytes = destinations.size() * sizeof(uint16_t);
            unsigned int destinationsOffset = maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinationsSizeBytes;
            for (int i = 0; i < numberPackets - 1; ++i)
            {
                serpHeader.part = i;
                writeSERPHeader(serpHeader, outgoingMessage.data.data());
                std::memcpy(outgoingMessage.data.data() + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinationsSizeBytes);
                writeDestinations(destinationsOffset, destinations, outgoingMessage.data.data());
                offset += destinationsOffset;
                outgoingMessages.push(std::make_unique<BasicOutgoingMessage>(outgoingMessage));
            }
            serpHeader.part = numberPackets - 1;
            unsigned int bytesLeft = message.data.size() - offset;
            serpHeader.len = bytesLeft + sizeof(SERP_Header) + sizeof(SMP_Header);
            outgoingMessage.data.resize(serpHeader.len + sizeof(uint16_t) * destinations.size());
            writeSERPHeader(serpHeader, outgoingMessage.data.data());
            std::memcpy(outgoingMessage.data.data() + sizeof(SERP_Header) + sizeof(SMP_Header), message.data.data() + offset, bytesLeft);
            writeDestinations(bytesLeft, destinations, outgoingMessage.data.data());
            outgoingMessages.push(std::make_unique<BasicOutgoingMessage>(outgoingMessage));
            return true;
        }
    }

    void NetworkData::handleServerMessageAdvertisement(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        switch (smpHeader.options)
        {
        case static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT):
        {
            connectedToSERPServer = false;
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
                    std::memcpy(reinterpret_cast<std::byte*>(&clientID) + i, networkBuffer.data() + sizeof(SERP_Header) + sizeof(SMP_Header) + i, sizeof(std::byte));
                }
                clientID = ntohs(clientID);
                connectedToSERPServer = true;
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
    void NetworkData::handleServerMessageEcho(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        switch (smpHeader.options)
        {
        case static_cast<uint16_t>(MESSAGE_OPTION_ECHO::ECHO_REPLY):
        {
            break;
        }
        case static_cast<uint16_t>(MESSAGE_OPTION_ECHO::ECHO_REQUEST):
        {
            SERP_Header replySERPHeader(clientID, 0, serpHeader.len, 0, 1, nextMessageID++);
            SMP_Header replySMPHeader(MESSAGE_TYPE::ECHO, static_cast<uint16_t>(MESSAGE_OPTION_ECHO::ECHO_REPLY));
            writeSERPHeader(replySERPHeader, networkBuffer.data());
            writeSMPHeader(replySMPHeader, networkBuffer.data());
            sendMessage(clientSocket, serverAdress, serpHeader.len, reinterpret_cast<char*>(networkBuffer.data()));
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
    void NetworkData::handleServerMessageError(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
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
                errorLogger << static_cast<uint8_t>(networkBuffer[i]);
            }
        }
        errorLogger << LOGGER::ENDL;
    }

    void NetworkData::handleIncomingMessageInNetworkBuffer()
    {
        // Extract message headers
        SERP_Header serpHeader = readSERPHeader();
        SMP_Header smpHeader = readSMPHeader();
        // DEBUG: Print message to console
        ///*
        infoLogger << LOGGER::BEGIN << "Received message with the following headers: "
            << "SERP(src: " << static_cast<unsigned int>(serpHeader.src)
            << ", dst: " << static_cast<unsigned int>(serpHeader.dst)
            << ", len: " << static_cast<unsigned int>(serpHeader.len)
            << ", part: " << static_cast<unsigned int>(serpHeader.part)
            << ", total: " << static_cast<unsigned int>(serpHeader.total)
            << ", id: " << static_cast<unsigned int>(serpHeader.id)
            << "), SMP(type: " << static_cast<unsigned int>(smpHeader.type)
            << ", options: " << static_cast<unsigned int>(smpHeader.options) << ")" << LOGGER::ENDL;
        //*/
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
        else if (smpHeader.type == static_cast<uint16_t>(MESSAGE_TYPE::MESSAGE_RECEIVED))
        {
            // A client is notifying us that a message was received
            auto it = safeOutgoingMessageDatas.find(serpHeader.id);
            if (it != safeOutgoingMessageDatas.end()) {
                it->second.notifyReceivedMessagePart(serpHeader.src, serpHeader.part);
                if (it->second.dataPartsNotReceivedCount <= 0)
                {
                    // We can delete the outgoing message!
                    safeOutgoingMessageDatas.erase(it);
                }
            }
        }
        else {
            // If safe send is active, we need to check first if the message was already received
            if (serpHeader.getFlag(SERP_Header::FlagType::SAFE_SEND))
            {
                auto it = safeIncomingMessageData.find(serpHeader.id);
                if (it != safeIncomingMessageData.end()) {
                    auto it2 = it->second.find(serpHeader.src);
                    if (it2 != it->second.end()) {
                        // We have already received this message. Send a MESSAGE_RECEIVED message to the source
                        sendMessageReceivedMessage(serpHeader);
                        return;
                    }
                }
            }
            // If the message consists of only a single packet, we can put it directly into the finished message buffer
            if (serpHeader.total <= 1) {
                insertMessageFromNetworkBufferIntoFinishedMessageBuffer(serpHeader, smpHeader);
                // If safe send is active, we need to insert the message into the safeIncomingMessageData map and
                // send a messageReceived message!
                if (serpHeader.getFlag(SERP_Header::FlagType::SAFE_SEND)) {
                    insertIntoSafeIncomingMessagesData(serpHeader, 0.0);
                    sendMessageReceivedMessage(serpHeader);
                }
            }
            // If the message consitst of more than one part, we have to put it into the unfinished message buffer
            else {
                insertMessageFromNetworkBufferIntoUnfinishedMessageBuffer(serpHeader, smpHeader);
            }
        }
    }

    void NetworkData::sendMessageReceivedMessage(const SERP_Header& serpHeader)
    {
        SERP_Header replySERPHeader = serpHeader;
        replySERPHeader.src = clientID;
        replySERPHeader.dst = serpHeader.src;
        replySERPHeader.len = sizeof(SERP_Header) + sizeof(SMP_Header);
        SMP_Header replySMPHeader(MESSAGE_TYPE::MESSAGE_RECEIVED, 0);
        writeSERPHeader(replySERPHeader, networkBuffer.data());
        writeSMPHeader(replySMPHeader, networkBuffer.data());
        sendMessage(clientSocket, serverAdress, replySERPHeader.len, reinterpret_cast<char*>(networkBuffer.data()));
    }

    void NetworkData::sendDisconnectMessageToServer()
    {
        SERP_Header serpHeader(clientID, 0, sizeof(SERP_Header) + sizeof(SMP_Header), 0, 1, nextMessageID);
        SMP_Header smpHeader(MESSAGE_TYPE::ADVERTISEMENT, static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::DISCONNECT));
        writeSERPHeader(serpHeader, networkBuffer.data());
        writeSMPHeader(smpHeader, networkBuffer.data());
        sendMessage(clientSocket, serverAdress, serpHeader.len, reinterpret_cast<char*>(networkBuffer.data()));
    }

    void NetworkData::sendConnectionRequestToServer()
    {
        SERP_Header serpHeader(clientID, 0, sizeof(SERP_Header) + sizeof(SMP_Header), 0, 1, nextMessageID);
        SMP_Header smpHeader(MESSAGE_TYPE::ADVERTISEMENT, static_cast<uint16_t>(MESSAGE_OPTION_ADVERTISEMENT::REQUEST));
        writeSERPHeader(serpHeader, networkBuffer.data());
        writeSMPHeader(smpHeader, networkBuffer.data());
        sendMessage(clientSocket, serverAdress, serpHeader.len, reinterpret_cast<char*>(networkBuffer.data()));
    }

    void NetworkData::insertIntoSafeIncomingMessagesData(const SERP_Header& serpHeader, double timeSinceFirstReceive)
    {
        auto it = safeIncomingMessageData.find(serpHeader.id);
        if (it == safeIncomingMessageData.end()) {
            it = safeIncomingMessageData.insert(std::make_pair(serpHeader.id, std::unordered_map<uint16_t, SafeIncomingMessageData>())).first;
        }
        it->second[serpHeader.src] = SafeIncomingMessageData{ timeSinceFirstReceive };
    }

    void NetworkData::insertMessageFromNetworkBufferIntoFinishedMessageBuffer(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        if (serpHeader.len > sizeof(SERP_Header) + sizeof(SMP_Header))
        {
            std::vector<std::byte> data(min(serpHeader.len, maxMessageLength) - sizeof(SERP_Header) - sizeof(SMP_Header));
            std::memcpy(data.data(), networkBuffer.data() + sizeof(SERP_Header) + sizeof(SMP_Header), data.size());
            incomingMessages.push_back(std::move(SMP_Message(serpHeader.src, smpHeader, std::move(data))));
        }
        else
        {
            incomingMessages.push_back(std::move(SMP_Message(serpHeader.src, smpHeader)));
        }
    }

    void NetworkData::insertMessageFromNetworkBufferIntoUnfinishedMessageBuffer(const SERP_Header& serpHeader, const SMP_Header& smpHeader)
    {
        auto it = incomingUnfinishedMessageDatas.find(serpHeader.id);
        if (it == incomingUnfinishedMessageDatas.end()) {
            it = incomingUnfinishedMessageDatas.insert(std::make_pair(serpHeader.id, std::unordered_map<uint16_t, IncomingUnfinishedMessageData>())).first;
        }
        auto it2 = it->second.find(serpHeader.src);
        if (it2 == it->second.end()) {
            it2 = it->second.insert(std::make_pair(serpHeader.src, IncomingUnfinishedMessageData(serpHeader.id, serpHeader.src, serpHeader.flags, smpHeader, serpHeader.total))).first;
        }
        /// Actually copy the data into the buffer
        it2->second.receiveDataPart(serpHeader.part, networkBuffer.data() + sizeof(SERP_Header) + sizeof(SMP_Header), serpHeader.len - sizeof(SERP_Header) - sizeof(SMP_Header));
        if (it2->second.dataPartsNotReceivedCount <= 0)
        {
            // The message was completely received. Insert into finished incoming message buffer!
            incomingMessages.push_back(std::move(it2->second.constructSMP_Message()));
            if (getFlag(it2->second.flags, SERP_Header::FlagType::SAFE_SEND))
            {
                // If safe send was active, we need to insert the message into the finished safe message buffer
                insertIntoSafeIncomingMessagesData(serpHeader, it2->second.timeSinceFirstReceive);
            }
            // Delete the message from the unfinished incoming message map
            it->second.erase(it2->first);
            if (it->second.empty()) incomingUnfinishedMessageDatas.erase(it->first);
        }
        // If safe send was active, notify the sender that the message was received
        if (serpHeader.getFlag(SERP_Header::FlagType::SAFE_SEND))
        {
            sendMessageReceivedMessage(serpHeader);
        }
    }

    void NetworkData::updateOutgoingMessageQueue(double dt)
    {
        accumulativeTimeSinceLastSend += dt;
        unsigned int bytesToSend = bytesPerSecondSend * accumulativeTimeSinceLastSend;
        while (!outgoingMessages.empty())
        {
            auto bytesSent = outgoingMessages.front()->send(*this, bytesToSend);
            if (!bytesSent.has_value()) {
                accumulativeTimeSinceLastSend = static_cast<double>(bytesToSend) / static_cast<double>(bytesPerSecondSend);
                return;
            }
            outgoingMessages.pop();
            bytesToSend -= bytesSent.value();
        }
        accumulativeTimeSinceLastSend = 0.0;
    }

    std::optional<int> NetworkData::recieveMessage(int sock, sockaddr& remoteAddr, int& remoteAddrLen)
    {
        int ret = recvfrom(sock, reinterpret_cast<char*>(networkBuffer.data()), maxMessageLength * sizeof(std::byte), 0, &remoteAddr, &remoteAddrLen);
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

    void NetworkData::updateContainers(double dt)
    {
        // Process safeOutgoingMessageDatas
        auto itSafeOutgoingMessages = safeOutgoingMessageDatas.begin();
        while (itSafeOutgoingMessages != safeOutgoingMessageDatas.end())
        {
            itSafeOutgoingMessages->second.timeSinceFirstSend += dt;
            itSafeOutgoingMessages->second.timeSinceLastSend += dt;
            if (itSafeOutgoingMessages->second.timeSinceFirstSend > timeoutSendingMessagesSafeMode) {
                itSafeOutgoingMessages = safeOutgoingMessageDatas.erase(itSafeOutgoingMessages);
            }
            else
            {
                if (itSafeOutgoingMessages->second.timeSinceLastSend > durationBeforeResendSafeMode) {
                    itSafeOutgoingMessages->second.pushPacketsToSendQueue(*this);
                }
                itSafeOutgoingMessages++;
            }
        }

        // Process incomingUnfinishedMessageDatas
        auto itUnfinishedMessages = incomingUnfinishedMessageDatas.begin();
        while (itUnfinishedMessages != incomingUnfinishedMessageDatas.end())
        {
            auto it = itUnfinishedMessages->second.begin();
            while (it != itUnfinishedMessages->second.end())
            {
                it->second.timeSinceFirstReceive += dt;
                if (it->second.timeSinceFirstReceive > timeoutIncomingMessages) {
                    it = itUnfinishedMessages->second.erase(it);
                }
                else {
                    it++;
                }
            }
            if (itUnfinishedMessages->second.empty()) {
                itUnfinishedMessages = incomingUnfinishedMessageDatas.erase(itUnfinishedMessages);
            }
            else {
                itUnfinishedMessages++;
            }
        }

        // Process safeIncomingMessageData
        auto itSafeIncomingMessage = safeIncomingMessageData.begin();
        while (itSafeIncomingMessage != safeIncomingMessageData.end())
        {
            auto it = itSafeIncomingMessage->second.begin();
            while (it != itSafeIncomingMessage->second.end())
            {
                it->second.timeSinceFirstReceive += dt;
                if (it->second.timeSinceFirstReceive > timeoutIncomingMessages) {
                    it = itSafeIncomingMessage->second.erase(it);
                }
                else {
                    it++;
                }
            }
            if (itSafeIncomingMessage->second.empty()) {
                itSafeIncomingMessage = safeIncomingMessageData.erase(itSafeIncomingMessage);
            }
            else {
                itSafeIncomingMessage++;
            }
        }

        // TODO: Process incomingMessages if they are not collected after some time!
    }

}