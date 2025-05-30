#include "Network\SERP\SERP.h"
#include <WinSock2.h>
#include "Utility\Formatting.h"

namespace SnackerEngine
{

	bool SERPHeader::getResponseFlag() const
	{
		return flags & 0b1000000000000000;
	}
	
	void SERPHeader::setResponseFlag(bool response)
	{
		if (response) {
			flags |= 0b1000000000000000;
		}
		else {
			flags &= 0b0111111111111111;
		}
	}

	bool SERPHeader::getLargeMessageFlag() const
	{
		return flags & 0b0100000000000000;
	}

	void SERPHeader::setLargeMessageFlag(bool isLargeMessage)
	{
		if (isLargeMessage) {
			flags |= 0b0100000000000000;
		}
		else {
			flags &= 0b1011111111111111;
		}
	}

	void SERPHeader::toNetworkByteOrder()
	{
		source = htons(source);
		destination = htons(destination);
		messageID = htonl(messageID);
		contentLength = htonl(contentLength);
		flags = htons(flags);
		statusCode = htons(statusCode);
	}

	void SERPHeader::toHostByteOrder()
	{
		source = ntohs(source);
		destination = ntohs(destination);
		messageID = ntohl(messageID);
		contentLength = ntohl(contentLength);
		flags = ntohs(flags);
		statusCode = ntohs(statusCode);
	}

	bool SERPHeader::serialize(BufferView buffer)
	{
		if (buffer.size() != sizeof(SERPHeader)) return {};
		toNetworkByteOrder();
		memcpy(&buffer[0], this, sizeof(SERPHeader));
		toHostByteOrder();
		return true;
	}

	template<>
	std::string to_string(const SERPHeader& header)
	{
		return "source: " + to_string(header.source) + "\n" +
			"destination: " + to_string(header.destination) + "\n" +
			"messageID: " + to_string(header.messageID) + "\n" +
			"status code: " + to_string(header.statusCode);
	}

	std::optional<SERPHeader> SERPHeader::parse(ConstantBufferView buffer)
	{
		if (buffer.size() != sizeof(SERPHeader)) return {};
		SERPHeader result{};
		memcpy(&result, &buffer[0], sizeof(SERPHeader));
		result.toHostByteOrder();
		return result;
	}

	std::unique_ptr<SERPMessage> SERPMessage::parse(const SERPHeader& header, ConstantBufferView buffer)
	{
		if (header.contentLength != buffer.size()) return nullptr;
		if (header.getResponseFlag()) {
			return SERPResponse::parse(header, buffer);
		}
		else {
			return SERPRequest::parse(header, buffer);
		}
	}

	SERPMessage::SERPMessage(const SERPMessage& other)
		: header(other.header) {}

	SERPMessage& SERPMessage::operator=(const SERPMessage& other)
	{
		this->header = other.header;
		return *this;
	}

	bool SERPMessage::isResponse() const
	{
		return header.getResponseFlag();
	}

	bool SERPMessage::isRequest() const
	{
		return !header.getResponseFlag();
	}

	void SERPRequest::finalize()
	{
		header.contentLength = static_cast<uint32_t>(target.size() + 1 + content.size());
	}

	SERPRequest::SERPRequest(SERPHeader header, const std::string& target, ConstantBufferView content)
		: SERPMessage(header), target(target), content(content.copyBytes()) {}

	std::unique_ptr<SERPRequest> SERPRequest::parse(const SERPHeader& header, ConstantBufferView buffer)
	{
		// Find first newline to parse target string
		std::size_t newLinePos = buffer.find_first_of(static_cast<std::byte>('\n'));
		if (newLinePos == std::string::npos) return nullptr;
		// Check if content has correct length
		if (header.contentLength != buffer.size()) return nullptr;
		// Return new message
		return std::unique_ptr<SERPRequest>(new SERPRequest(header, buffer.getBufferView(0, newLinePos).to_string(), buffer.getBufferView(newLinePos + 1)));
	}

	Buffer SERPRequest::serialize()
	{
		Buffer result(sizeof(SERPHeader) + target.size() + 1 + content.size());
		header.serialize(result.getBufferView(0, sizeof(SERPHeader)));
		std::size_t offset = sizeof(SERPHeader);
		if (!target.empty()) memcpy(&result[offset], &target[0], target.size());
		result.set(offset + target.size(), static_cast<std::byte>('\n'));
		offset += target.size() + 1;
		if (!content.empty()) memcpy(&result[offset], &content[0], content.size());
		return result;
	}

	std::vector<std::string> SERPRequest::splitTargetPath(const std::string& target)
	{
		if (target.empty()) return {};
		std::vector<std::string> result;
		std::size_t lastPos = target[0] == '/' ? 1 : 0;
		for (std::size_t i = lastPos; i < target.length(); ++i) {
			if (target[i] == '/') {
				result.push_back(target.substr(lastPos, i - lastPos));
				lastPos = i + 1;
			}
		}
		if (!target.ends_with('/')) {
			result.push_back(target.substr(lastPos));
		}
		return result;
	}

	SERPRequest::SERPRequest(const SERPRequest& other)
		: SERPMessage(other), target(other.target), content(other.content.copyBytes()) {}

	SERPRequest& SERPRequest::operator=(const SERPRequest& other)
	{
		SERPMessage::operator=(other);
		target = other.target;
		content = other.content.copyBytes();
		return *this;
	}

	SERPRequest::SERPRequest(SERPID destination, RequestStatusCode requestStatusCode, const std::string& target, const Buffer& content)
		: SERPMessage(SERPHeader{ 0, static_cast<uint16_t>(destination), 0, 0, 0b0000000000000000, static_cast<uint16_t>(requestStatusCode)}), target(target), content(content.copyBytes()) {}

	RequestStatusCode SERPRequest::getRequestStatusCode() const
	{
		return static_cast<RequestStatusCode>(header.statusCode);
	}

	template<>
	std::string to_string(const SERPRequest& request)
	{
		return to_string(request.getHeader()) + "\n" +
			"target: " + request.target + "\n" +
			"content: " + request.content.to_string();
	}

	void SERPResponse::finalize()
	{
		header.contentLength = static_cast<uint32_t>(content.size());
	}

	SERPResponse::SERPResponse(SERPHeader header, ConstantBufferView content)
		: SERPMessage(header), content(content.copyBytes()) {}

	SERPResponse::SERPResponse(uint32_t messageID)
		: SERPMessage(SERPHeader{ 0, 0, messageID, 0, 0b1000000000000000, 0 }), content(content.copyBytes()) {}

	std::unique_ptr<SERPResponse> SERPResponse::parse(const SERPHeader& header, ConstantBufferView buffer)
	{
		// Check if content has correct length
		if (header.contentLength != buffer.size()) return nullptr;
		// Return new message
		return std::unique_ptr<SERPResponse>(new SERPResponse(header, buffer));
	}

	Buffer SERPResponse::serialize()
	{
		Buffer result(sizeof(SERPHeader) + content.size());
		header.serialize(result.getBufferView(0, sizeof(SERPHeader)));
		std::size_t offset = sizeof(SERPHeader);
		if (!content.empty()) memcpy(&result[offset], &content[0], content.size());
		return result;
	}

	SERPResponse::SERPResponse(const SERPResponse& other)
		: SERPMessage(other), content(other.content.copyBytes()) {}

	SERPResponse& SERPResponse::operator=(const SERPResponse& other)
	{
		SERPMessage::operator=(other);
		content = other.content.copyBytes();
		return *this;
	}

	SERPResponse::SERPResponse(const SERPRequest& request, ResponseStatusCode responseStatusCode, const Buffer& content)
		: SERPMessage(SERPHeader{ 0, request.getHeader().source, request.getHeader().messageID, 0, 0b1000000000000000, static_cast<uint16_t>(responseStatusCode)}), content(content.copyBytes()) {}

	ResponseStatusCode SERPResponse::getResponseStatusCode() const
	{
		return static_cast<ResponseStatusCode>(header.statusCode);
	}

	template<>
	std::string to_string(const SERPResponse& response)
	{
		return to_string(response.getHeader()) + "\n" +
			"content: " + response.content.to_string();
	}

}