#pragma once

#include "Utility/Buffer.h"
#include "StatusCodes.h"
#include "SERPID.h"

#include <inttypes.h>
#include <string>
#include <unordered_set>

namespace SnackerEngine
{

	struct SERPHeader
	{
		uint16_t source;		// SERPID of source
		uint16_t destination;	// SERPID of destination, or number of destination clients if multisend flag is set.
		uint32_t messageID;		// unique (per sent message of source) message identifier
		uint32_t contentLength;	// Content length in bytes
		uint16_t flags;			// Additional boolean information about the message
		uint16_t statusCode;	// SERP Status Code
		bool getResponseFlag() const;
		void setResponseFlag(bool response);
		bool getLargeMessageFlag() const;
		void setLargeMessageFlag(bool isLargeMessage);
		bool getMultiSendFlag() const;
		void setMultiSendFlag(bool isMultiSend);
	private:
		/// Helper functions used during serialization / deserialization
		void toNetworkByteOrder();
		void toHostByteOrder();
	public:
		/// Serializes the header into the given buffer. Turns all integers to network byte order.
		/// Returns true on success and false on failure. Only works if the buffer has the correct size
		/// of sizeof(SERPHeader)
		bool serialize(BufferView buffer);
		/// Tries to parse a Header from the given buffer. Returns an empty optional on failure.
		/// Only works when the buffer has the size sizeof(SERPHeader). 
		static std::optional<SERPHeader> parse(ConstantBufferView buffer);
		/// Getters
		SERPID getSource() { return SERPID(source); };
		SERPID getDestination() { return SERPID(destination); };
	};

	class SERPMessage
	{
	private:
		friend class SERPEndpoint;
		/// Finalizes the message. Is called by SERP Endpoint when the message is sent
		virtual void finalize() {};
		/// Tries to parse a SERPMessage from a header and a content buffer. Returns nullptr on failure
		static std::unique_ptr<SERPMessage> parse(const SERPHeader& header, ConstantBufferView buffer);
		/// Serializes this message into a buffer
		virtual Buffer serialize() { return Buffer{}; }
	protected:
		SERPHeader header;
		std::unordered_set<uint16_t> destinations; // set of destination SERPIDs in case of multisend.
		/// Constructors
		SERPMessage(const SERPHeader& header)
			: header(header), destinations{} {};
		SERPMessage(const SERPHeader& header, std::unordered_set<uint16_t> destinations)
			: header(header), destinations{ std::move(destinations) } {};
		/// Helper function that copies the destinations into the buffer. Only works if the buffer has size
		/// destinations.size() * sizeof(uint16_t)
		bool serializeDestinations(BufferView buffer);
		/// Helper function that tries to parse destinations from the given buffer.
		/// Returns set of destinations in host byte order
		static std::unordered_set<uint16_t> parseDestinations(ConstantBufferView bufferView, std::uint16_t count);
	public:
		// Copy constructor and assignment operator
		SERPMessage(const SERPMessage& other);
		SERPMessage& operator=(const SERPMessage& other);
		// Move constructor and assignment operator
		SERPMessage(SERPMessage&& other) = default;
		SERPMessage& operator=(SERPMessage&& other) = default;
		bool isResponse() const;
		bool isRequest() const;
		const SERPHeader& getHeader() const { return header; }
		SERPHeader& getHeader() { return header; }
		const std::unordered_set<uint16_t>& getDestinations() const { return destinations; }
		void addDestination(SERPID destination);
		void removeDestination(SERPID destination);
		void clearDestinations();
	};

	class SERPRequest : public SERPMessage
	{
	private:
		friend class SERPEndpoint;
		friend class SERPMessage;
		// Finalizes the message. Is called by SERP Endpoint when the message is sent.
		void finalize() override;
		/// Constructors used in parse function
		SERPRequest(SERPHeader header, const std::string& target, ConstantBufferView content);
		SERPRequest(SERPHeader header, const std::string& target, ConstantBufferView content, std::unordered_set<uint16_t> destinations);
		/// Tries to parse a SERPRequest from a header and a content buffer. Returns nullptr on failure
		static std::unique_ptr<SERPRequest> parse(const SERPHeader& header, ConstantBufferView buffer);
		/// Serializes this message into a buffer
		virtual Buffer serialize() override;
	public:
		/// Helper function for splitting the target path at backslash characters
		static std::vector<std::string> splitTargetPath(const std::string& target);
		// Copy constructor and assignment operator
		SERPRequest(const SERPRequest& other);
		SERPRequest& operator=(const SERPRequest& other);
		// Move constructor and assignment operator
		SERPRequest(SERPRequest&& other) = default;
		SERPRequest& operator=(SERPRequest&& other) = default;
		std::string target;
		Buffer content;
		/// Creates a new request
		SERPRequest(SERPID destination, RequestStatusCode requestStatusCode, const std::string& target = "", const Buffer& content = Buffer{});
		/// Getters
		RequestStatusCode getRequestStatusCode() const;
	};

	class SERPResponse : public SERPMessage
	{
	private:
		friend class SERPEndpoint;
		friend class SERPMessage;
		friend class SERPManager;
		// Finalizes the message. Is called by SERP Endpoint when the message is sent
		void finalize() override;
		/// Constructor used in parse function
		SERPResponse(SERPHeader header, ConstantBufferView content);
		SERPResponse(SERPHeader header, ConstantBufferView content, std::unordered_set<uint16_t> destinations);
		SERPResponse(uint32_t messageID);
		/// Tries to parse a SERPResponse from a header and a content buffer. Returns nullptr on failure
		static std::unique_ptr<SERPResponse> parse(const SERPHeader& header, ConstantBufferView buffer);
		/// Serializes this message into a buffer
		virtual Buffer serialize() override;
	public:
		// Copy constructor and assignment operator
		SERPResponse(const SERPResponse& other);
		SERPResponse& operator=(const SERPResponse& other);
		// Move constructor and assignment operator
		SERPResponse(SERPResponse&& other) = default;
		SERPResponse& operator=(SERPResponse&& other) = default;
		Buffer content;
		/// Creates a new response as an answer to the given request
		SERPResponse(const SERPRequest& request, ResponseStatusCode responseStatusCode, const Buffer& content = Buffer{});
		/// Getters
		ResponseStatusCode getResponseStatusCode() const;
	};


}