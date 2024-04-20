#pragma once
#include <inttypes.h>

#ifdef DELETE
#undef DELETE
#endif

namespace SnackerEngine
{

	enum class RequestStatusCode : uint16_t
	{
		GET,
		PUT,
		POST,
		DELETE,
		CONNECT,
		DISCONNECT,
	};

	enum class ResponseStatusCode : uint16_t
	{
		OK = 200,
		BAD_REQUEST = 400,
		UNAUTHORIZED = 401,
		FORBIDDEN = 403,
		NOT_FOUND = 404,
		REQUEST_TIMEOUT = 408,
	};

	/// Checks if the given response status code is a successfull response code
	bool isSuccessfullResponse(ResponseStatusCode code);
	/// Checks if the given response status code is a client or server error response code
	bool isErrorResponse(ResponseStatusCode code);

}