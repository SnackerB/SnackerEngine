#include "Network/HTTPResponseStatusCode.h"

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

}