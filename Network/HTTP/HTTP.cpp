#include "HTTP.h"

#include <iostream>
#include <chrono>

namespace SnackerEngine
{

	static std::optional<unsigned int> from_string(const std::string& string)
	{
		try
		{
			return static_cast<unsigned>(std::stoul(string));
		}
		catch (const std::exception&)
		{
			return {};
		}
	}

	/// Helper function for receiveMessages(). Input Parameters: The currently remaining buffer (can contain multiple seperate HTTP messages)
	std::unique_ptr<HTTPMessage> handleNewHTTPMessage(const ConstantBufferView& buffer, std::unique_ptr<HTTPMessage>& currentMessage, std::size_t& remainingSize, std::size_t& bufferOffset)
	{
		// We first need to read the headers to know if the message is
		// larger/smaller than our buffer
		auto httpMessage = HTTPMessage::parseHeader(buffer);
		if (!httpMessage.has_value()) {
			bufferOffset = 0; // Set bufferOffset to zero to signal that we're done with this buffer
			std::cout << "Received invalid HTTP message!" << std::endl;
		}
		else {
			// Check if the content length header exists
			auto contentLengthString = httpMessage.value().first->getHeaderValue("content-length");
			if (contentLengthString.has_value()) {
				auto contentLength = from_string(std::string(contentLengthString.value()));
				if (!contentLength.has_value()) {
					std::cout << "Received message with invalid content-length: \"" << contentLengthString.value() << "\"" << std::endl;
				}
				else {
					std::size_t remainingBufferSize = buffer.size() - httpMessage.value().second;
					if (contentLength.value() > remainingBufferSize) {
						// Message is to large for buffer and becomes the new currentMessage
						currentMessage = std::move(httpMessage.value().first);
						currentMessage->body.reserve(contentLength.value());
						currentMessage->body.append(buffer.getBufferView(httpMessage.value().second).to_string());
						remainingSize = contentLength.value() - remainingBufferSize;
						bufferOffset = 0; // Set bufferOffset to zero to signal that we're done with this buffer
						return nullptr;
					}
					else {
						// Message is complete in the buffer, but there is still some data remaining
						httpMessage.value().first->body = buffer.getBufferView(httpMessage.value().second, contentLength.value()).to_string();
						bufferOffset += httpMessage.value().second + contentLength.value(); // Increment bufferOffset
						if (bufferOffset >= buffer.size()) bufferOffset = 0;  // Set bufferOffset to zero to signal that we're done with this buffer
						return std::move(httpMessage.value().first);
					}
				}
			}
			else {
				// No content header exists.
				// Parse the message body from the remaining buffer data
				if (httpMessage.value().second < buffer.size())
					httpMessage.value().first->body = buffer.getBufferView(httpMessage.value().second).to_string();
				// Push the message into the result vector
				bufferOffset = 0; // Set bufferOffset to zero to signal that we're done with this buffer
				return std::move(httpMessage.value().first);
			}
		}
	}

	bool didHitWallTime(std::chrono::steady_clock::time_point startTime, double wallTimeSeconds)
	{
		std::chrono::steady_clock::time_point now = std::chrono::high_resolution_clock::now();
		long long microseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime).count();
		double dt = (double(microseconds) / 1'000'000'000.0f);
		return dt >= wallTimeSeconds;
	}

	std::vector<std::unique_ptr<HTTPMessage>> HTTPEndpoint::receiveMessages(double wallTimeSeconds)
	{
		std::vector<std::unique_ptr<HTTPMessage>> result;
		auto startTime = std::chrono::high_resolution_clock::now();
		do {
			if (tempBufferOffset == 0 || tempBufferOffset >= tempBufferSize) {
				// We want to receive a new chunk of data
				std::optional<Buffer> message{};
				if (blocking) message = receiveFrom(socket, tempBuffer);
				else message = receiveFromNonBlocking(socket, tempBuffer);
				// If we did not receive anything, we can return for now
				if (!message.has_value()) return result;
				if (currentMessage) {
					// Copy bytes from the buffer to the current message!
					currentMessage->body.append(message.value().to_string());
					if (remainingSize > message.value().size()) {
						// We are still waiting for more data.
						remainingSize -= message.value().size();
					}
					else if (remainingSize < message.value().size()) {
						// The message is complete, but we received more data: This will be our next message!
						result.emplace_back(std::move(currentMessage));
						currentMessage = nullptr;
						tempBufferOffset = message.value().size() - remainingSize;
						tempBufferSize = message.value().size();
					}
					else {
						// The message is complete, and we have no more received data
						result.emplace_back(std::move(currentMessage));
						currentMessage = nullptr;
					}
				}
				else {
					// We are currently not waiting for more data, which means that the received message
					// is a new message!
					auto tempResult = handleNewHTTPMessage(message.value().getBufferView(), currentMessage, remainingSize, tempBufferOffset);
					if (tempBufferOffset != 0 && tempBufferOffset < message.value().size()) tempBufferSize = message.value().size();
					if (tempResult) result.emplace_back(std::move(tempResult));
				}
			}
			else {
				// There is data in the tempBuffer we need to take care of. Begin by parsing the HTTPMessage
				auto tempResult = handleNewHTTPMessage(tempBuffer.getBufferView(tempBufferOffset, tempBufferSize - tempBufferOffset), currentMessage, remainingSize, tempBufferOffset);
				if (tempResult) result.emplace_back(std::move(tempResult));
			}
		} while (wallTimeSeconds == 0.0 || !didHitWallTime(startTime, wallTimeSeconds));
		return result;
	}

}
