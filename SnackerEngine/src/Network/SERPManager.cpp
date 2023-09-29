#include "Network\SerpManager.h"
#include "Core\Log.h"

#include <stdexcept>

namespace SnackerEngine
{

	void SERPManager::PendingResponse::onMove(PendingResponse* other)
	{
		if (serpManager) {
			auto it = serpManager->pendingResponses.find(response.source);
			if (it != serpManager->pendingResponses.end()) it->second = this;
			else {
				auto it2 = serpManager->outgoingRequests.find(response.source);
				if (it2 != serpManager->outgoingRequests.end()) {
					for (unsigned int i = 0; i < it2->second.size(); ++i) {
						if (it2->second[i].second == other) {
							it2->second[i].second = this;
							return;
						}
					}
				}
			}
		}
	}
	
	SERPManager::PendingResponse::PendingResponse(PendingResponse&& other) noexcept
		: serpManager{ std::move(other.serpManager) }, status{ std::move(other.status) }, 
		timeLeft{ std::move(timeLeft) }, response{ std::move(other.response) }
	{
		onMove(&other);
		other.serpManager = nullptr;
	}

	SERPManager::PendingResponse& SERPManager::PendingResponse::operator=(PendingResponse&& other) noexcept
	{
		onMove(&other);
		other.serpManager = nullptr;
		return *this;
	}

	SERPManager::PendingResponse::~PendingResponse()
	{
		if (serpManager) serpManager->pendingResponses.erase(response.source);
	}

	void SERPManager::handleReceivedMessage(SnackerNet::Buffer& buffer)
	{
		// Parse HTTP
		std::string dataString = buffer.to_string();
		std::optional<SnackerNet::HTTPRequest> request = SnackerNet::HTTPRequest::parse(dataString);
		if (request.has_value()) {
			auto serpRequest = SnackerNet::SERPRequest::parse(request.value());
			if (serpRequest.has_value()) {
				handleIncomingRequest(serpRequest.value());
			}
			else {
				errorLogger << LOGGER::BEGIN << "obtained HTTP request does not follow SERP format!" << LOGGER::ENDL;
			}
		}
		else {
			std::optional<SnackerNet::HTTPResponse> response = SnackerNet::HTTPResponse::parse(dataString);
			if (response.has_value()) {
				auto serpResponse = SnackerNet::SERPResponse::parse(response.value());
				if (serpResponse.has_value()) {
					handleIncomingResponse(serpResponse.value());
				}
				else {
					errorLogger << LOGGER::BEGIN << "obtained HTTP response does not follow SERP format!" << LOGGER::ENDL;
				}
			}
			else {
				errorLogger << LOGGER::BEGIN << "error with incoming message: HTTP version not supported or invalid message format!" << LOGGER::ENDL;
			}
		}
	}

	void SERPManager::handleIncomingRequest(SnackerNet::SERPRequest request)
	{
		if (request.destination != serpID) {
			warningLogger << LOGGER::BEGIN << "received incoming request with wrong destinationID!" << LOGGER::ENDL;
			sendResponse(SnackerNet::SERPResponse(serpID, request.source, 
				SnackerNet::HTTPResponse{ SnackerNet::ResponseStatusCode::BAD_REQUEST, 
				SnackerNet::HTTPMessage { {}, "destinationID did not match serpID!" } }));
		}
		else {
			// Sort message in the correct buffer, based on the path
			std::vector<std::string> path = request.request.splitPath();
			if (path.empty()) {
				handleIncomingManagerRequest(request);
			}
			std::queue<SnackerNet::SERPRequest>* bestResult = nullptr;
			std::string currentPath = "";
			for (unsigned i = 0; i < path.size(); ++i) {
				currentPath.append(path[i]);
				auto it = incomingRequests.find(currentPath);
				if (it != incomingRequests.end()) {
					bestResult = &it->second;
				}
			}
			if (bestResult) {
				bestResult->push(request);
			}
			else {
				handleIncomingManagerRequest(request);
			}

		}
	}

	void SERPManager::handleIncomingManagerRequest(SnackerNet::SERPRequest request)
	{
		if (request.request.requestMethod == request.request.HTTP_GET && request.request.path == "/ping") {
			sendResponse(SnackerNet::SERPResponse(serpID, request.source,
				SnackerNet::HTTPResponse{ SnackerNet::ResponseStatusCode::OK,
				SnackerNet::HTTPMessage{} }));
		}
		else {
			warningLogger << "received request with unknown path \"" << request.request.path << "\"" << LOGGER::ENDL;
			sendResponse(SnackerNet::SERPResponse(serpID, request.source,
				SnackerNet::HTTPResponse{ SnackerNet::ResponseStatusCode::NOT_FOUND,
				SnackerNet::HTTPMessage { {}, "path" + request.request.path + "was not found!" } }));
		}
	}

	void SERPManager::handleIncomingResponse(SnackerNet::SERPResponse response)
	{
		auto it = pendingResponses.find(response.source);
		if (it != pendingResponses.end()) {
			it->second->status = PendingResponse::Status::OBTAINED;
			it->second->response = response;
		}
		else {
			warningLogger << "Received response without request or after timeout!" << LOGGER::ENDL;
		}
	}

	SERPManager::SERPManager()
		: connected{ false }, serpID{ 0 }, socket{}, incomingRequests{}, pendingResponses{}, outgoingRequests{}, 
		serpIDResponse{ nullptr }, incomingMessageBuffer{ 4000 }
	{
		auto result = SnackerNet::createSocketTCP();
		if (!result.has_value()) {
			errorLogger << "Was not able to create socket!" << LOGGER::ENDL;
			return;
		}
		socket = std::move(result.value());
	}

	SERPManager::SERPManager(SERPManager&& other) noexcept
		: connected{ std::move(other.connected) }, serpID{ std::move(other.serpID) }, socket{ std::move(other.socket) }, 
		incomingRequests{ std::move(other.incomingRequests) }, pendingResponses{ std::move(other.pendingResponses) }, 
		outgoingRequests{ std::move(other.outgoingRequests) }, serpIDResponse{ std::move(other.serpIDResponse) },
		incomingMessageBuffer{ std::move(other.incomingMessageBuffer) }
	{
		for (const auto& pendingResponse : pendingResponses) {
			pendingResponse.second->serpManager = this;
		}
	}

	bool SERPManager::connectToSERPServer()
	{
		bool result = SnackerNet::connectTo(socket, SnackerNet::getSERPServerAdressTCP());
		if (result) {
			connected = true;
			serpIDResponse = std::make_unique<PendingResponse>(sendRequest(SnackerNet::SERPRequest(0, 0, { SnackerNet::HTTPRequest::RequestMethod::HTTP_GET, "/serpID" })));
			if (!SnackerNet::setToNonBlocking(socket)) {
				errorLogger << "Was not able to set socket to non-blocking!" << LOGGER::ENDL;
				socket = SnackerNet::SocketTCP{};
				return false;
			}
			return true;
		}
		errorLogger << "Was not able to connect to SERP Server!" << LOGGER::ENDL;
		return false;
	}

	void SERPManager::registerPathForIncomingRequests(const std::string& path)
	{
		if (incomingRequests.find(path) == incomingRequests.end())
			incomingRequests.insert(std::make_pair<>(path, std::queue<SnackerNet::SERPRequest>()));
	}

	bool SERPManager::areIncomingRequests(const std::string& path)
	{
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) return false;
		return !it->second.empty();
	}

	std::queue<SnackerNet::SERPRequest>& SERPManager::getIncomingRequests(const std::string& path)
	{
		auto it = incomingRequests.find(path);
		if (it == incomingRequests.end()) {
			throw std::out_of_range("\"" + path + "\" was not a registered path!");
		}
		return it->second;
	}

	SERPManager::PendingResponse SERPManager::sendRequest(SnackerNet::SERPRequest request, double timeout)
	{
		// Construct pendingResponse object
		PendingResponse pendingResponse(this, timeout, request.destination);
		// Check if we are already wating for a response from the destination
		auto it = pendingResponses.find(request.destination);
		if (it == pendingResponses.end()) {
			// We are not currently waiting for a response from destination
			// We can therefore send the message directly!
			pendingResponses.insert(std::make_pair<>(request.destination, &pendingResponse));
			SnackerNet::sendRequest(socket, request.request);
		}
		else {
			// We are currently waiting for a response from destination
			// We must push the request in the correct outgoing requests queue
			auto it2 = outgoingRequests.find(request.destination);
			if (it2 == outgoingRequests.end()) {
				it2->second.push_back(std::make_pair<>(request, &pendingResponse));
			}
			else {
				auto result = outgoingRequests.insert(std::make_pair<>(request.destination, std::vector<std::pair<SnackerNet::SERPRequest, PendingResponse*>>()));
				if (result.second) result.first->second.push_back(std::make_pair<>(request, &pendingResponse));
			}
		}
		return pendingResponse;
	}

	void SERPManager::sendResponse(SnackerNet::SERPResponse response)
	{
		SnackerNet::sendResponse(socket, response.response);
	}

	void SERPManager::update(double dt)
	{
		if (!connected) return;
		// Check if we have not yet received our serpID from the server
		if (serpID == 0) {
			if (!serpIDResponse || serpIDResponse->getStatus() == PendingResponse::Status::TIMEOUT) {
				// Send another serpID request
				serpIDResponse = nullptr;
				serpIDResponse = std::make_unique<PendingResponse>(sendRequest(SnackerNet::SERPRequest(0, 0, { SnackerNet::HTTPRequest::RequestMethod::HTTP_GET, "/serpID" })));
			}
			else if (serpIDResponse->getStatus() == PendingResponse::Status::OBTAINED) {
				const SnackerNet::SERPResponse& response = serpIDResponse->getResponse();
				if (response.response.responseStatusCode == SnackerNet::ResponseStatusCode::OK) {
					auto result = SnackerNet::parseSERPID(response.response.message.body);
					if (result.has_value()) {
						serpID = result.value();
					}
				}
				if (serpID == 0) {
					serpIDResponse = nullptr;
					serpIDResponse = std::make_unique<PendingResponse>(sendRequest(SnackerNet::SERPRequest(0, 0, { SnackerNet::HTTPRequest::RequestMethod::HTTP_GET, "/serpID" })));
				}
			}
		}
		// Check if any new incoming messages are here
		std::optional<SnackerNet::Buffer> receivedMessage = SnackerNet::receiveFromNonBlocking(socket, incomingMessageBuffer);
		while (receivedMessage.has_value()) {
			handleReceivedMessage(receivedMessage.value());
			receivedMessage = SnackerNet::receiveFromNonBlocking(socket, incomingMessageBuffer);
		}
		// Check if we are able to send anything from the outgoingRequest queues
		for (auto it = outgoingRequests.begin(); it != outgoingRequests.end(); ) {
			if (pendingResponses.find(it->first) == pendingResponses.end()) {
				pendingResponses.insert(std::make_pair<>(it->first, it->second.front().second));
				SnackerNet::sendRequest(socket, it->second.front().first.request);
				it->second.erase(it->second.begin());
				if (it->second.empty()) {
					outgoingRequests.erase(it++);
				}
				else {
					it++;
				}
			}
		}
		// Update the time of all pending responses
		for (auto& pendingResponse : pendingResponses) {
			pendingResponse.second->timeLeft -= dt;
			if (pendingResponse.second->timeLeft <= 0) {
				pendingResponse.second->timeLeft = 0;
				pendingResponse.second->status = PendingResponse::Status::TIMEOUT;
			}
		}
		for (auto& outgoingRequest : outgoingRequests) {
			for (auto& request : outgoingRequest.second) {
				auto& pendingResponse = request.second;
				pendingResponse->timeLeft -= dt;
				if (pendingResponse->timeLeft <= 0) {
					pendingResponse->timeLeft = 0;
					pendingResponse->status = PendingResponse::Status::TIMEOUT;
				}
			}
		}
	}

}