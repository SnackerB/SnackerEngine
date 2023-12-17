#include "HTTP/Message.h"
#include "HTTP/Request.h"
#include "HTTP/Response.h"


#include <sstream>

namespace SnackerEngine
{
	
	std::unique_ptr<HTTPMessage> HTTPMessage::parse(const ConstantBufferView& buffer)
	{
		auto result = parseHeader(buffer);
		if (!result.has_value()) return {};
		// Parse message body
		if (result.value().second < buffer.size()) {
			result.value().first->body = buffer.getBufferView(result.value().second).to_string();
		}
		return std::move(result.value().first);
	}

	std::optional<std::pair<std::unique_ptr<HTTPMessage>, std::size_t>> HTTPMessage::parseHeader(const ConstantBufferView& buffer)
	{
		std::unique_ptr<HTTPMessage> result = nullptr;
		std::size_t offset = 0;
		// Parse the first line to figure out if we have a request or response
		auto request = HTTPRequest::parseRequestLine(buffer);
		if (request.has_value()) {
			result = std::make_unique<HTTPRequest>(std::move(request.value().first));
			offset = request.value().second;
		}
		else {
			auto response = HTTPResponse::parseResponseLine(buffer);
			if (response.has_value()) {
				result = std::make_unique<HTTPResponse>(std::move(response.value().first));
				offset = response.value().second;
			}
			else {
				return {};
			}
		}
		// Parse Headers until an empty line is encountered
		std::size_t lastPos = offset - 1;
		std::size_t newPos = buffer.find_first_of(static_cast<std::byte>('\n'), offset);
		while (true) {
			if (newPos == std::string::npos) return {};
			std::size_t size = newPos - lastPos - 1;
			if (size == 0) {
				return std::make_pair(std::move(result), newPos + 1);
			}
			auto header = HTTPHeader::parse(buffer.getBufferView(lastPos + 1, size));
			if (header.has_value()) {
				result->headers.emplace_back(std::move(header.value()));
				lastPos = newPos;
				newPos = buffer.find_first_of(static_cast<std::byte>('\n'), newPos + 1);
			}
			else {
				return {};
			}
		}
		return std::make_pair<>(std::move(result), newPos + 1);
	}

	std::string HTTPMessage::toString() const
	{
		std::string result;
		for (const auto& header : headers) result.append(header.toString()).append("\n");
		result.append("\n");
		result.append(body);
		return result;
	}

	Buffer HTTPMessage::toBuffer() const
	{
		return Buffer(toString());
	}

	std::optional<std::string_view> HTTPMessage::getHeaderValue(const std::string& key) const
	{
		for (const auto& header : headers) {
			if (header.name == key) return header.value;
		}
		return {};
	}

	bool HTTPMessage::removeHeader(const std::string& key)
	{
		for (auto it = headers.begin(); it != headers.end(); ++it) {
			if (it->name == key) {
				headers.erase(it);
				return true;
			}
		}
		return false;
	}

	void HTTPMessage::addContentLengthHeader()
	{
		headers.push_back({ "content-length", std::to_string(body.length()) });
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const HTTPMessage& message)
	{
		return ostream << message.toString();
	}

}
