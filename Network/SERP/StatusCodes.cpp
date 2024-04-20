#include "SERP\StatusCodes.h"
#include "Utility\Formatting.h"

namespace SnackerEngine
{

	bool isSuccessfullResponse(ResponseStatusCode code)
	{
		return code == ResponseStatusCode::OK;
	}

	bool isErrorResponse(ResponseStatusCode code)
	{
		return code != ResponseStatusCode::OK;
	}

	template<>
	std::string to_string(const RequestStatusCode& requestStatusCode)
	{
		switch (requestStatusCode)
		{
		case RequestStatusCode::GET: return "GET";
		case RequestStatusCode::PUT: return "PUT";
		case RequestStatusCode::POST: return "POST";
		case RequestStatusCode::DELETE: return "DELETE";
		case RequestStatusCode::CONNECT: return "CONNECT";
		case RequestStatusCode::DISCONNECT: return "DISCONNECT";
		default: return std::to_string(static_cast<unsigned>(requestStatusCode));
		}
	}

	template<>
	std::string to_string(const ResponseStatusCode& responseStatusCode)
	{
		switch (responseStatusCode)
		{
		case ResponseStatusCode::OK: return "OK";
		case ResponseStatusCode::BAD_REQUEST: return "BAD_REQUEST";
		case ResponseStatusCode::UNAUTHORIZED: return "UNAUTHORIZED";
		case ResponseStatusCode::FORBIDDEN: return "FORBIDDEN";
		case ResponseStatusCode::NOT_FOUND: return "NOT_FOUND";
		case ResponseStatusCode::REQUEST_TIMEOUT: return "REQUEST_TIMEOUT";
		default: return std::to_string(static_cast<unsigned>(responseStatusCode));
		}
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const RequestStatusCode& requestStatusCode)
	{
		ostream << to_string(requestStatusCode);
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& ostream, const ResponseStatusCode& responseStatusCode)
	{
		ostream << to_string(responseStatusCode);
	}

}