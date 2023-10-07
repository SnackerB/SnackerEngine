#pragma once

#include "Network\HTTP\Header.h"
#include "Network\HTTP\Message.h"
#include "Network\TCP\TCP.h"

namespace SnackerEngine
{

	/// struct representing a HTTP request
	struct HTTPRequest : public HTTPMessage
	{
		/// The RequestMethod defines the desired action to be performed
		enum RequestMethod
		{
			HTTP_GET,
			HTTP_POST,
			HTTP_HEAD,
			HTTP_PUT,
			HTTP_DELETE
		};
		/// The request method of this request
		RequestMethod requestMethod;
		/// The filepath
		std::string path;
		/// Constructor
		HTTPRequest(RequestMethod requestMethod = HTTP_GET, const std::string& path = "", const std::vector<HTTPHeader>& headers = {}, const std::string& body = "")
			: HTTPMessage(MessageType::REQUEST, headers, body), requestMethod{ requestMethod }, path{ path } {}
		/// Parses only the request line from a bufferView.
		/// Additionally returns the index of the byte after the request line (first byte of the headers)
		static std::optional<std::pair<HTTPRequest, std::size_t>> parseRequestLine(const ConstantBufferView& buffer);
		/// Transforms the request to a string
		std::string toString() const;
		/// Transforms the request to a buffer
		Buffer toBuffer() const;
		/// Splits the path into individual folders / names
		std::vector<std::string> splitPath() const;
	};

	/// Sends a HTTPRequest over the given socket
	void sendRequest(const SocketTCP& socket, const HTTPRequest& request);

}