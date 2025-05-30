#include "Network\SERP\SERPEndpoint.h"

namespace SnackerEngine
{

	std::pair<std::vector<std::unique_ptr<SERPMessage>>, ConstantBufferView> SERPEndpoint::parseMessages(ConstantBufferView buffer)
	{
		std::vector<std::unique_ptr<SERPMessage>> result;
		while (true) {
			if (tempMessageHeader.has_value()) {
				// We are currently waiting for more data for a already obtained message
				if (buffer.size() == 0) return std::make_pair<>(std::move(result), buffer);
				if (tempMessageBytesLeftToParse <= buffer.size()) {
					// All bytes still left to parse are in the buffer. We can finalize the message
					memcpy(&tempMessageBuffer[tempMessageBuffer.size() - tempMessageBytesLeftToParse], &buffer[0], tempMessageBytesLeftToParse);
					std::unique_ptr<SERPMessage> tempMessage = SERPMessage::parse(tempMessageHeader.value(), tempMessageBuffer.getBufferView());
					if (tempMessage) result.push_back(std::move(tempMessage));
					// Advance buffer
					buffer = buffer.getBufferView(tempMessageBytesLeftToParse);
					// Cleanup
					tempMessageHeader = std::nullopt;
					tempMessageBuffer = Buffer{};
					tempMessageBytesLeftToParse = 0;
				}
				else {
					// We still do not have enough bytes to finalize message.
					memcpy(&tempMessageBuffer[tempMessageBuffer.size() - tempMessageBytesLeftToParse], &buffer[0], buffer.size());
					tempMessageBytesLeftToParse -= buffer.size();
					// Advance buffer
					buffer = buffer.getBufferView(buffer.size());
				}
			}
			else {
				// This is a new message!
				if (buffer.size() < sizeof(SERPHeader)) return std::make_pair<>(std::move(result), buffer);
				tempMessageHeader = SERPHeader::parse(buffer.getBufferView(0, sizeof(SERPHeader)));
				if (!tempMessageHeader.has_value()) {
					// Invalid header. Return result and clear remaining buffer
					return std::make_pair<>(std::move(result), buffer.getBufferView(buffer.size()));
				}
				if (tempMessageHeader.value().contentLength == 0) {
					std::unique_ptr<SERPMessage> tempMessage = SERPMessage::parse(tempMessageHeader.value(), tempMessageBuffer.getBufferView());
					if (tempMessage) result.push_back(std::move(tempMessage));
					// Cleanup
					tempMessageHeader = std::nullopt;
					tempMessageBuffer = Buffer{};
					tempMessageBytesLeftToParse = 0;
				}
				else {
					tempMessageBuffer = Buffer(tempMessageHeader.value().contentLength);
					tempMessageBytesLeftToParse = tempMessageHeader.value().contentLength;
				}
				buffer = buffer.getBufferView(sizeof(SERPHeader));
			}
		}
	}

	std::vector<std::unique_ptr<SERPMessage>> SERPEndpoint::receiveMessages()
	{
		Buffer resultBuffer = endpointTCP.receiveData();
		if (storageBuffer.empty()) storageBuffer = std::move(resultBuffer);
		else {
			Buffer temp = Buffer(storageBuffer.size() + resultBuffer.size());
			memcpy(&temp[0], &storageBuffer[0], storageBuffer.size());
			if (!resultBuffer.empty()) memcpy(&temp[storageBuffer.size()], &resultBuffer[0], resultBuffer.size());
			storageBuffer = std::move(temp);
		}
		auto result = parseMessages(storageBuffer.getBufferView());
		storageBuffer = Buffer(result.second.copyBytes());
		return std::move(result.first);
	}

	void SERPEndpoint::finalizeAndSendMessage(SERPMessage& message, bool setMessageID)
	{
		if (message.isRequest() && setMessageID) message.header.messageID = nextMessageID++;
		message.finalize();
		Buffer buffer = message.serialize();
		endpointTCP.sendData(buffer.getBufferView());
	}

	void SERPEndpoint::finalizeMessage(SERPMessage& message, bool setMessageID)
	{
		if (message.isRequest() && setMessageID) message.header.messageID = nextMessageID++;
		message.finalize();
	}

	void SERPEndpoint::sendMessage(SERPMessage& message)
	{
		Buffer buffer = message.serialize(); // TODO: I think we can do better here, just serialize the message ONCE!
		endpointTCP.sendData(buffer.getBufferView());
	}

}