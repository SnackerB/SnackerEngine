#include "SERP.h"
#include "Utility\Formatting.h"

namespace SnackerEngine
{

	SERPRequest::SERPRequest(SERPID source, SERPID destination, HTTPRequest httpRequest)
		: SERPHeader(source, destination), request(httpRequest)
	{
		request.headers.push_back(HTTPHeader("sourceID", to_string(source)));
		request.headers.push_back(HTTPHeader("destinationID", to_string(destination)));
	}

	std::optional<SERPRequest> SERPRequest::parse(HTTPRequest httpRequest)
	{
		SERPRequest result{};
		// Parse destinationID
		auto destinationIDString = httpRequest.getHeaderValue("destinationID");
		if (!destinationIDString.has_value()) return {};
		auto destinationID = parseSERPID(std::string(destinationIDString.value()));
		if (!destinationID.has_value()) return {};
		// Parse sourceID
		auto sourceIDString = httpRequest.getHeaderValue("sourceID");
		if (!sourceIDString.has_value()) return {};
		auto sourceID = parseSERPID(std::string(sourceIDString.value()));
		if (!sourceID.has_value()) return {};
		// Finalize message
		return SERPRequest(sourceID.value(), destinationID.value(), httpRequest);
	}

	SERPResponse::SERPResponse(SERPID source, SERPID destination, HTTPResponse httpResponse)
		: SERPHeader(source, destination), response(httpResponse)
	{
		response.headers.push_back(HTTPHeader("sourceID", to_string(source)));
		response.headers.push_back(HTTPHeader("destinationID", to_string(destination)));
	}

	std::optional<SERPResponse> SERPResponse::parse(HTTPResponse httpResponse)
	{
		// Parse destinationID
		auto destinationIDString = httpResponse.getHeaderValue("destinationID");
		if (!destinationIDString.has_value()) return {};
		auto destinationID = parseSERPID(std::string(destinationIDString.value()));
		if (!destinationID.has_value()) return {};
		// Parse sourceID
		auto sourceIDString = httpResponse.getHeaderValue("sourceID");
		if (!sourceIDString.has_value()) return {};
		auto sourceID = parseSERPID(std::string(sourceIDString.value()));
		if (!sourceID.has_value()) return {};
		// Finalize message
		return SERPResponse(sourceID.value(), destinationID.value(), httpResponse);
	}

	void SERPHeader::setSourceUpdateHeaders(SERPID source, std::vector<HTTPHeader>& headers)
	{
		this->source = source;
		for (auto& header : headers) {
			if (header.name == "sourceID") {
				header.value = to_string(source);
				return;
			}
		}
		headers.push_back({ "sourceID", to_string(source) });
	}

	void SERPHeader::setDestinationUpdateHeaders(SERPID destination, std::vector<HTTPHeader>& headers)
	{
		this->destination = destination;
		for (auto& header : headers) {
			if (header.name == "destinationID") {
				header.value = to_string(destination);
				return;
			}
		}
		headers.push_back({ "destinationID", to_string(destination) });
	}

}

