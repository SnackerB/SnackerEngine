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
	protected:
		/// Constructor
		SERPHeader(SERPID source = 0, SERPID destination = 0)
			: source(source), destination(destination) {}
		/// Setters
		void setSourceUpdateHeaders(SERPID source, std::vector<HTTPHeader>& headers);
		void setDestinationUpdateHeaders(SERPID destination, std::vector<HTTPHeader>& headers);
	public:
		virtual std::string toString() const { return ""; };
		/// Getters
		SERPID getSource() const { return source; }
		SERPID getDestination() const { return destination; }
	};

	class SERPRequest : public SERPHeader
	{
	public:
		HTTPRequest request;
		/// Default constructor
		SERPRequest() = default;
		/// Creates a (outgoing) SERPRequest from the given HTTPRequest, 
		/// adding the source and destination SERPIDs to the headers
		SERPRequest(SERPID source, SERPID destination, HTTPRequest httpRequest);
		/// Parses a SERPRequest from the given (incoming) HTTPRequest,
		/// extracting the source and destination SERPIDs from the headers
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
		/// Default constructor
		SERPResponse() = default;
		/// Creates a (outgoing) SERPResponse from the given HTTPResponse, 
		/// adding the source and destination SERPIDs to the headers
		SERPResponse(SERPID source, SERPID destination, HTTPResponse httpResponse);
		/// Parses a SERPResponse from the given (incoming) HTTPResponse,
		/// extracting the source and destination SERPIDs from the headers
		static std::optional<SERPResponse> parse(HTTPResponse httpResponse);
		std::string toString() const override;
		/// Setters
		void setSource(SERPID source) { SERPHeader::setSourceUpdateHeaders(source, response.headers); }
		void setDestination(SERPID destination) { SERPHeader::setDestinationUpdateHeaders(destination, response.headers); }
	};

}