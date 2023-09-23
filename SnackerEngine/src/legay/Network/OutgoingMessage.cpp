#include "OutgoingMessage.h"
#include "Network/NetworkData.h"
#include "Network/Network.h"
#include "SERP.h"

namespace SnackerEngine
{

	std::optional<unsigned int> BasicOutgoingMessage::send(NetworkData& networkData, unsigned int maxLengthBytes)
	{
		if (data.size() > maxLengthBytes) return {};
		networkData.sendMessageToServer(data.data(), static_cast<unsigned>(data.size()));
		return static_cast<int>(data.size());
	}

	std::optional<unsigned int> SafeOutgoingMessage::send(NetworkData& networkData, unsigned int maxLengthBytes)
	{
		auto it = networkData.safeOutgoingMessageDatas.find(messageID);
		if (it == networkData.safeOutgoingMessageDatas.end()) return true;
		return it->second.sendMessagePart(messagePart, networkData, maxLengthBytes);
	}

    /// Writes data to the buffer
    static void writeData(const std::vector<std::byte>& data, std::byte* buffer)
    {
        std::memcpy(buffer, data.data(), data.size());
    }

    std::optional<SafeOutgoingMessageData> SafeOutgoingMessageData::CreateSafeOutgoingMessageData(const NetworkData& networkData, const SMP_Message& message, const std::vector<uint16_t>& destinations)
    {
        if (!networkData.connectedToSERPServer) return {};
        if (destinations.size() == 0) return {};
        if (destinations.size() * sizeof(uint16_t) + sizeof(SERP_Header) + sizeof(SMP_Header) >= networkData.maxMessageLength) return {};
        unsigned int dataLength = static_cast<unsigned>(message.data.size());
        unsigned int dataPartSize = static_cast<unsigned>(networkData.maxMessageLength - sizeof(SERP_Header) - sizeof(SMP_Header) - destinations.size() * sizeof(uint16_t));
        unsigned int numberPackets = dataLength / dataPartSize;
        if (dataLength % dataPartSize != 0) numberPackets++;
        if (numberPackets == 0) numberPackets = 1;
        /// Check if we can send in a single message!
        SafeOutgoingMessageData result;
        // Copy data to the buffers
        if (numberPackets == 1)
        {
            result.dataParts.resize(1);
            result.dataParts[0].resize(message.data.size());
            if (!message.data.empty()) std::memcpy(result.dataParts[0].data(), message.data.data(), message.data.size());
        }
        else {
            result.dataParts.resize(numberPackets);
            unsigned int offset = 0;
            for (unsigned i = 0; i < numberPackets - 1; ++i)
            {
                result.dataParts[i].resize(dataPartSize);
                std::memcpy(result.dataParts[i].data(), message.data.data() + offset, dataPartSize);
                offset += dataPartSize;
            }
            unsigned int bytesLeft = static_cast<unsigned>(message.data.size() - offset);
            result.dataParts.back().resize(bytesLeft);
            std::memcpy(result.dataParts.back().data(), message.data.data() + offset, bytesLeft);
        }
        // Setup the resto of the messageData
        result.messageID = networkData.nextMessageID;
        result.flags = 0 | (0b1 << static_cast<unsigned int>(SERP_Header::FlagType::SAFE_SEND));
        result.smpHeader = message.smpHeader;
        result.destinations = destinations;
        result.receivedDataPart = std::vector<std::pair<int, std::vector<bool>>>(numberPackets, std::make_pair<int, std::vector<bool>>(static_cast<unsigned>(destinations.size()), std::vector<bool>(static_cast<unsigned>(destinations.size()), false)));
        result.dataPartsNotReceivedCount = numberPackets;
        result.dataPartsNotReceivedCountPerClient = std::vector<int>(destinations.size(), numberPackets);
        for (unsigned int i = 0; i < destinations.size(); ++i)
        {
            result.clientIDToIndex[destinations[i]] = i;
        }
        return std::move(result);
    }

    void SafeOutgoingMessageData::notifyReceivedMessagePart(uint16_t clientID, uint16_t messagePartIndex)
    {
        auto it = clientIDToIndex.find(clientID);
        if (it == clientIDToIndex.end()) return;
        if (messagePartIndex >= receivedDataPart.size() || receivedDataPart[messagePartIndex].second[it->second]) return;
        receivedDataPart[messagePartIndex].second[it->second] = true;
        receivedDataPart[messagePartIndex].first--;
        if (receivedDataPart[messagePartIndex].first == 0)
        {
            // The given messagePart was received by all clients!
            dataPartsNotReceivedCount--;
        }
        dataPartsNotReceivedCountPerClient[it->second]--;
    }

    std::optional<unsigned int> SafeOutgoingMessageData::sendMessagePart(uint16_t messagePartIndex, NetworkData& networkData, unsigned int maxMessageLength)
    {
        if (messagePartIndex >= dataParts.size()) return 0;
        if (receivedDataPart[messagePartIndex].first <= 0) return 0;
        std::vector<uint16_t> currentDestinations;
        for (unsigned int i = 0; i < destinations.size(); ++i)
        {
            if (!receivedDataPart[messagePartIndex].second[i]) {
                // Client has not received message part yet!
                currentDestinations.push_back(destinations[i]);
            }
        }
        if (currentDestinations.empty()) return 0;
        if (currentDestinations.size() == 1)
        {
            // We can send the message as singlecast message
            unsigned int messageLength = static_cast<unsigned>(sizeof(SERP_Header) + sizeof(SMP_Header) + dataParts[messagePartIndex].size());
            if (maxMessageLength > 0 && messageLength > maxMessageLength) return {};
            SERP_Header serpHeader(networkData.clientID, currentDestinations.front(), messageLength, messagePartIndex, static_cast<uint16_t>(dataParts.size()), messageID);
            serpHeader.setFlag(SERP_Header::FlagType::SAFE_SEND);
            networkData.sendMessageToServerSinglecast(serpHeader, smpHeader, dataParts[messagePartIndex]);
            timeSinceLastSend = 0.0;
            return messageLength;
        }
        else
        {
            // We need to send the message as multicast message
            unsigned int messageLength = static_cast<unsigned>(sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(std::byte) * dataParts[messagePartIndex].size() + sizeof(uint16_t) * currentDestinations.size());
            if (maxMessageLength > 0 && messageLength > maxMessageLength) return false;
            SERP_Header serpHeader(networkData.clientID, SERP_DST_MULTICAST, static_cast<uint16_t>(sizeof(SERP_Header) + sizeof(SMP_Header) + sizeof(std::byte) * dataParts[messagePartIndex].size()), messagePartIndex, static_cast<uint16_t>(dataParts.size()), messageID);
            serpHeader.setFlag(SERP_Header::FlagType::SAFE_SEND);
            networkData.sendMessageToServerMulticast(serpHeader, smpHeader, dataParts[messagePartIndex], currentDestinations);
            timeSinceLastSend = 0.0;
            return messageLength;
        }
    }

    void SafeOutgoingMessageData::pushPacketsToSendQueue(NetworkData& networkData, unsigned int sendMaxPackets)
    {
        for (unsigned int i = 0; i < dataParts.size(); ++i)
        {
            if (receivedDataPart[i].first > 0)
            {
                networkData.outgoingMessages.push(std::move(std::make_unique<SafeOutgoingMessage>(messageID, i)));
                if (sendMaxPackets > 0) {
                    sendMaxPackets--;
                    if (sendMaxPackets == 0) return;
                }
            }
        }
    }

    SafeOutgoingMessageData::SafeOutgoingMessageData()
        : timeSinceFirstSend(0.0), timeSinceLastSend(0.0), messageID(0), flags(0), smpHeader(), destinations(), 
        dataParts(), receivedDataPart(), dataPartsNotReceivedCount(0), dataPartsNotReceivedCountPerClient(), clientIDToIndex()
    {
    }

}
