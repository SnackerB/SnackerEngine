#pragma once

#include <string>
#include <vector>

namespace SnackerEngine
{

	struct HTTPHeader
	{
		std::string name;
		std::string value;
	};

	/// class representing a single HTTP request
	class HTTPRequest
	{
	public:
		/// The RequestMethod defines the desired action to be performed
		enum RequestMethod
		{
			GET,
			HEAD,
			PUT,
			DELETE,
		};
	private:
		/// The request method of this request
		RequestMethod requestMethod;
		/// Collection of headers
		std::vector<HTTPHeader> headers;
		/// The body of the message
		std::string body;
	};

	/// class representing a single HTTP response
	class HTTPResponse
	{
	public:
		/// The Response
		enum ResponseStatusCode
		{

		};
	};

}