#include "HTTP/ResponseStatusCode.h"

namespace SnackerEngine
{

	bool isInformationResponse(ResponseStatusCode code)
	{
		return code >= 100 && code < 200;
	}

	bool isSuccessfullResponse(ResponseStatusCode code)
	{
		return code >= 200 && code < 300;
	}

	bool isErrorResponse(ResponseStatusCode code)
	{
		return code >= 300 && code < 500;
	}

	bool isClientErrorResponse(ResponseStatusCode code)
	{
		return code >= 300 && code < 400;
	}

	bool isServerErrorResponse(ResponseStatusCode code)
	{
		return code >= 400 && code < 500;;
	}

	std::optional<ResponseStatusCode> parseResponseStatusCode(const std::string& string)
	{
		try
		{
			unsigned result = std::stoul(string);
			switch (result)
			{
			case 100: return CONTINUE;
			case 102: return PROCESSING;
			case 200: return OK;
			case 201: return CREATED;
			case 400: return BAD_REQUEST;
			case 401: return UNAUTHORIZED;
			case 403: return FORBIDDEN;
			case 404: return BAD_REQUEST;
			case 408: return REQUEST_TIMEOUT;
			case 418: return IM_A_TEAPOT;
			case 500: return INTERNAL_SERVER_ERROR;
			case 501: return NOT_IMPLEMENTED;
			case 502: return BAD_GATEWAY;
			case 503: return SERVICE_UNAVAILABLE;
			case 505: return HTTP_VERSION_NOT_SUPPORTED;
			default: return {};
			}
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	std::string to_string(ResponseStatusCode responseStatusCode)
	{
		return std::to_string(static_cast<unsigned>(responseStatusCode));
	}

}