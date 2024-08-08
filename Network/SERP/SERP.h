#pragma once

#include "Network\HTTP\Request.h"
#include "Network\HTTP\Response.h"
#include "Network\SERP\SERPID.h"

#include <optional>

namespace SnackerEngine
{

	class SERPHeader
	{
	private:
		SERPID source;
		SERPID destination;
		/// Unique ID for every SERP Message sent by a instance of SnackerEngine.
		/// A SERP Response must reuse the messageID of the SERP request.
		std::size_t messageID;
	protected:
		/// Constructor
		SERPHeader(SERPID source, SERPID destination, std::size_t messageID)
			: source(source), destination(destination), messageID(messageID) {}
		/// Setters
		void setSourceUpdateHeaders(SERPID source, std::vector<HTTPHeader>& headers);
		void setDestinationUpdateHeaders(SERPID destination, std::vector<HTTPHeader>& headers);
	public:
		virtual std::string toString() const { return ""; };
		/// Getters
		SERPID getSource() const { return source; }
		SERPID getDestination() const { return destination; }
		std::size_t getMessageID() const { return messageID; }
	};

	class SERPRequest : public SERPHeader
	{
	public:
		HTTPRequest request;
	protected:
		SERPRequest(SERPID source, SERPID destination, std::size_t messageID, const HTTPRequest& httpRequest)
			: SERPHeader(source, destination, messageID), request(httpRequest) {}
	public:
		/// Default constructor.
		SERPRequest() = default;
		/// Creates a (outgoing) SERPRequest from the given HTTPRequest, 
		/// adding the source and destination SERPIDs and a new messageID to the headers
		SERPRequest(SERPID source, SERPID destination, HTTPRequest httpRequest);
		/// Parses a SERPRequest from the given (incoming) HTTPRequest,
		/// extracting the source and destination SERPIDs and the messageID from the headers
		static std::optional<SERPRequest> parse(HTTPRequest httpRequest);
		std::string toString() const override;
		/// Setters
		void setSource(SERPID source) { SERPHeader::setSourceUpdateHeaders(source, request.headers); }
		void setDestination(SERPID destination) { SERPHeader::setDestinationUpdateHeaders(destination, request.headers); }
	};

	class SERPResponse : public SERPHeader
	{
	public:
		HTTPResponse response;
	protected:
		SERPResponse(SERPID source, SERPID destination, std::size_t messageID, const HTTPResponse& httpResponse)
			: SERPHeader(source, destination, messageID), response(httpResponse) {}
	public:
		/// Default constructor
		SERPResponse() = default;
		/// Creates a (outgoing) SERPResponse from the given HTTPResponse, 
		/// adding the source and destination SERPIDs and a new messageID to the headers
		SERPResponse(SERPID source, SERPID destination, HTTPResponse httpResponse);
		/// Parses a SERPResponse from the given (incoming) HTTPResponse,
		/// extracting the source and destination SERPIDs and the messageID from the headers
		static std::optional<SERPResponse> parse(HTTPResponse httpResponse);
		std::string toString() const override;
		/// Setters
		void setSource(SERPID source) { SERPHeader::setSourceUpdateHeaders(source, response.headers); }
		void setDestination(SERPID destination) { SERPHeader::setDestinationUpdateHeaders(destination, response.headers); }
	};

}