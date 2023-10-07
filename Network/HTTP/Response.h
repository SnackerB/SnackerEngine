#pragma once

#include "Network\HTTP\ResponseStatusCode.h"
#include "Network\HTTP\Header.h"
#include "Network\HTTP\Message.h"
#include "Network\TCP\TCP.h"

namespace SnackerEngine
{

	/// Class representing a HTTP response
	class HTTPResponse : public HTTPMessage
	{
	public:
		/// The status code
		ResponseStatusCode responseStatusCode;
		/// Constructor
		HTTPResponse(ResponseStatusCode responseStatusCode = OK, const std::vector<HTTPHeader>& headers = {}, const std::string& body = "")
			: HTTPMessage(MessageType::RESPONSE, headers, body), responseStatusCode{ responseStatusCode } {}
		/// Parses only the response line from a bufferView.
		/// Additionally returns the index of the byte after the response line (first byte of the headers)
		static std::optional<std::pair<HTTPResponse, std::size_t>> parseResponseLine(const ConstantBufferView& buffer);
		/// Transforms the header to a string
		std::string toString() const;
		/// Transforms the request to a buffer
		Buffer toBuffer() const;

	};

	/// Sends a HTTPResponse over the given socket
	void sendResponse(const SocketTCP& socket, const HTTPResponse& response);

}