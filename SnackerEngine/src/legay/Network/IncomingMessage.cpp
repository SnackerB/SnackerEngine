#include "Network/IncomingMessage.h"
#include "Network/NetworkData.h"
#include "Network/Network.h"

namespace SnackerEngine
{

    void IncomingUnfinishedMessageData::receiveDataPart(uint16_t dataPart, const std::byte* data, unsigned int dataLengthBytes)
    {
        if (dataPart < dataParts.size()) {
            if (dataParts[dataPart].empty()) {
                // Copy data into buffer
                dataParts[dataPart].resize(dataLengthBytes);
                std::memcpy(dataParts[dataPart].data(), data, dataLengthBytes);
                // Update the count
                dataPartsNotReceivedCount--;
            }
        }
    }

    SMP_Message IncomingUnfinishedMessageData::constructSMP_Message()
    {
        unsigned int totalSize = 0;
        for (const auto& dataPart : dataParts) {
            totalSize += static_cast<unsigned>(dataPart.size());
        }
        std::vector<std::byte> data(totalSize);
        unsigned int offset = 0;
        for (const auto& dataPart : dataParts) {
            std::memcpy(data.data() + offset, dataPart.data(), dataPart.size());
            offset += static_cast<unsigned>(dataPart.size());
        }
        return SMP_Message(src, smpHeader, std::move(data));
    }

    IncomingUnfinishedMessageData::IncomingUnfinishedMessageData(uint32_t messageID, uint16_t src, uint32_t flags, SMP_Header smpHeader, uint16_t total)
        : timeSinceFirstReceive(0.0), messageID(messageID), src(src), flags(flags), smpHeader(smpHeader), dataParts(total), dataPartsNotReceivedCount(total) {}

}