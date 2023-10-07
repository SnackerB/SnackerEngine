#pragma once

#include "Network\HTTP\Header.h"
#include "Utility\Buffer.h"

#include <vector>
#include <memory>

namespace SnackerEngine
{

	/// A HTTP Message contains headers and a messageBody
	struct HTTPMessage
	{
	public:
		/// There are two types of HTTP messages: requests and responses!
		enum class MessageType
		{
			REQUEST,
			RESPONSE,
		};
	private:
		MessageType messageType;
	public:
		/// Collection of headers
		std::vector<HTTPHeader> headers;
		/// The body of the message
		std::string body;
	protected:
		/// Constructor
		HTTPMessage(MessageType messageType = MessageType::REQUEST, const std::vector<HTTPHeader>& headers = std::vector<HTTPHeader>{}, const std::string& body = "")
			: messageType(messageType), headers(headers), body(body) {}
	public:
		/// Parses a message from a buffer
		std::unique_ptr<HTTPMessage> parse(const ConstantBufferView& buffer);
		/// Parses only the request/response string and the headers, but not the message body.
		/// Additionally returns the position of the first byte of the message body
		static std::optional<std::pair<std::unique_ptr<HTTPMessage>, std::size_t>> parseHeader(const ConstantBufferView& buffer);
		/// Transforms the message to a string
		std::string toString() const;
		/// Transforms the message to a buffer
		Buffer toBuffer() const;
		/// Gets the value of a given header key, if the header exists
		std::optional<std::string_view> getHeaderValue(const std::string& key) const;
		/// Removes the header with the given key, if it exists. Returns true on success.
		bool removeHeader(const std::string& key);
		/// Returns the message type
		MessageType getMessageType() const { return messageType; };
		/// Adds the content length header
		void addContentLengthHeader();
	};

}