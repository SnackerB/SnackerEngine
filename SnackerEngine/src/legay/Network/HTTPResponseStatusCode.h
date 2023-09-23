#pragma once

namespace SnackerEngine
{

	/// Information taken from https://developer.mozilla.org/en-US/docs/Web/HTTP/Status#information_responses

	enum ResponseStatusCode
	{
		/// Information responses
		CONTINUE = 100,						/// This interim response indicates that the client should continue the request or ignore the response if the request is already finished.
		PROCESSING = 102,					/// This code indicates that the server has received and is processing the request, but no response is available yet.
		/// Successfull responses
		OK = 200,							/// The request succeeded.The result meaning of "success" depends on the HTTP method
		CREATED = 201,						/// The request succeeded, and a new resource was created as a result
		/// Client Error responses
		BAD_REQUEST = 400,					/// The server cannot or will not process the request due to something that is perceived to be a client error (e.g., malformed request syntax, invalid request message framing, or deceptive request routing).
		UNAUTHORIZED = 401,					/// Although the HTTP standard specifies "unauthorized", semantically this response means "unauthenticated". That is, the client must authenticate itself to get the requested response.
		FORBIDDEN = 403,					/// The client does not have access rights to the content; that is, it is unauthorized, so the server is refusing to give the requested resource. Unlike 401 Unauthorized, the client's identity is known to the server.
		NOT_FOUND = 404,					/// The server cannot find the requested resource
		REQUEST_TIMEOUT = 408,				/// This response is sent on an idle connection by some servers, even without any previous request by the client. It means that the server would like to shut down this unused connection.
		IM_A_TEAPOT = 418,					/// Debug error code
		/// Server Error responses
		INTERNAL_SERVER_ERROR = 500,		/// The server has encountered a situation it does not know how to handle.
		NOT_IMPLEMENTED = 501,				/// The request method is not supported by the server and cannot be handled.
		BAD_GATEWAY = 502,					/// This error response means that the server, while working as a gateway to get a response needed to handle the request, got an invalid response.
		SERVICE_UNAVAILABLE = 503,			/// The server is not ready to handle the request. Common causes are a server that is down for maintenance or that is overloaded.
		HTTP_VERSION_NOT_SUPPORTED = 505,	/// The HTTP version used in the request is not supported by the server.
	};

	/// Checks if the given response status code is an information response code
	bool isInformationResponse(ResponseStatusCode code);
	/// Checks if the given response status code is a successfull response code
	bool isSuccessfullResponse(ResponseStatusCode code);
	/// Checks if the given response status code is a client or server error response code
	bool isErrorResponse(ResponseStatusCode code);
	/// Checks if the given response status code is a client error response code
	bool isClientErrorResponse(ResponseStatusCode code);
	/// Checks if the given response status code is a server error response code
	bool isServerErrorResponse(ResponseStatusCode code);

}